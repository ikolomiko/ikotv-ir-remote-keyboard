// Most of the stuff in this file are shamelessly stolen from https://github.com/gjasny/v4l-utils/blob/master/utils/keytable/keytable.c

#include "hid_usb.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/lirc.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

static struct timeval last_event_ts;

static int64_t timeval_diff_ms(struct timeval* start, struct timeval* end)
{
    int64_t seconds = end->tv_sec - start->tv_sec;
    int64_t microseconds = end->tv_usec - start->tv_usec;
    return seconds * 1000 + microseconds / 1000;
}

static void evcode_to_keypress(int32_t evcode)
{
    hid_buf_t buf;
    switch (evcode) {
    case 77: // ch- button on remote = - button on keyboard (for volume down)
        buf = hid_buf_new(0, IKO_KEY_KPMINUS);
        hid_send(buf);
        break;

    case 76: // ch+ button on remote = + button on keyboard (for volume up)
        buf = hid_buf_new(0, IKO_KEY_KPPLUS);
        hid_send(buf);
        break;

    case 88: // up arrow button
        buf = hid_buf_new(0, IKO_KEY_UP);
        hid_send(buf);
        break;

    case 89: // down arrow button
        buf = hid_buf_new(0, IKO_KEY_DOWN);
        hid_send(buf);
        break;

    case 90: // left arrow button
        buf = hid_buf_new(0, IKO_KEY_LEFT);
        hid_send(buf);
        break;

    case 91: // right arrow button
        buf = hid_buf_new(0, IKO_KEY_RIGHT);
        hid_send(buf);
        break;

    case 44: // play button on remote = space on keyboard
        buf = hid_buf_new(0, IKO_KEY_SPACE);
        hid_send(buf);
        break;

    case 92: // ok button on remote = enter on keyboard
        buf = hid_buf_new(0, IKO_KEY_ENTER);
        hid_send(buf);
        break;

    case 10: // back button on remote = backspace on keyboard
        buf = hid_buf_new(0, IKO_KEY_BACKSPACE);
        hid_send(buf);
        break;

    default:
        // Unmapped IR remote key, ignore
        return;
    }
}

static void print_scancodes(const struct lirc_scancode* scancodes, size_t count)
{
    for (size_t i = 0; i < count; i++) {
        uint64_t ts1 = scancodes[i].timestamp / 1000000000ull;
        uint64_t ts2 = (scancodes[i].timestamp % 1000000000ull) / 1000ull;
        printf("%lu.%06lu: scancode = 0x%llx\n", ts1, ts2, scancodes[i].scancode);
    }
}

static void print_eventcodes(const struct input_event* eventcodes, size_t count)
{
    for (size_t i = 0; i < count; i += 2) {
        struct timeval ts = eventcodes[i].time;
        int64_t diff = timeval_diff_ms(&last_event_ts, &ts);
        printf("diff: %li\n", diff);
        if (diff < 250) {
            break;
        }

        last_event_ts = ts;

        int32_t val = eventcodes[i].value;
        const int64_t ts1 = eventcodes[i].time.tv_sec;
        const int64_t ts2 = (eventcodes[i].time.tv_usec);
        printf("%li.%06li: scancode = %d\n", ts1, ts2, eventcodes[i].value);
        evcode_to_keypress(val);
    }
}

static void read_events(const char* lirc_name, int32_t fd)
{
    struct input_event ev[64];
    struct lirc_scancode sc[64];
    int64_t rd;
    int32_t lircfd = -1;

    /* LIRC reports time in monotonic, set event to same */
    {
        uint32_t mode = CLOCK_MONOTONIC;
        ioctl(fd, EVIOCSCLOCKID, &mode);
    }

    uint32_t mode = LIRC_MODE_SCANCODE;
    lircfd = open(lirc_name, O_RDONLY | O_NONBLOCK);
    if (lircfd == -1) {
        perror("Can't open lirc device");
        return;
    }
    ioctl(lircfd, LIRC_SET_REC_MODE, &mode);

    printf("Reading events. Please, press CTRL-C to abort.\n");
    while (1) {
        struct pollfd pollstruct[2] = {
            { .fd = fd, .events = POLLIN },
            { .fd = lircfd, .events = POLLIN },
        };

        if (poll(pollstruct, 2, -1) < 0) {
            if (errno == EINTR)
                continue;

            perror("poll returned error");
        }

        rd = read(lircfd, sc, sizeof(sc));

        if (rd != -1) {
            // print_scancodes(sc, rd / sizeof(struct lirc_scancode));
            (void)print_scancodes;
        } else if (errno != EAGAIN) {
            perror("Error reading lirc scancode");
            return;
        }

        rd = read(fd, ev, sizeof(ev));
        size_t count = (size_t)rd / sizeof(struct input_event);
        print_eventcodes(ev, count);
        printf("here we go again\n");
    }
}

int main(void)
{
    const char* lirc_name = "/dev/lirc0";
    const char* input_name = "/dev/input/event0";

    if (gettimeofday(&last_event_ts, NULL) == -1) {
        perror("gettimeofday");
        return 1;
    }

    int32_t fd = open(input_name, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        perror(input_name);
        return -1;
    }

    read_events(lirc_name, fd);
    return 0;
}
