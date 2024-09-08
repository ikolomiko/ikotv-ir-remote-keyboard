#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "hid_usb.h"

const uint8_t ascii_to_hid_key_map[95][2] = {
    { 0, IKO_KEY_SPACE },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_1 },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_APOSTROPHE },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_3 },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_4 },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_5 },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_7 },
    { 0, IKO_KEY_APOSTROPHE },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_9 },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_0 },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_8 },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_EQUAL },
    { 0, IKO_KEY_COMMA },
    { 0, IKO_KEY_MINUS },
    { 0, IKO_KEY_DOT },
    { 0, IKO_KEY_SLASH },
    { 0, IKO_KEY_0 },
    { 0, IKO_KEY_1 },
    { 0, IKO_KEY_2 },
    { 0, IKO_KEY_3 },
    { 0, IKO_KEY_4 },
    { 0, IKO_KEY_5 },
    { 0, IKO_KEY_6 },
    { 0, IKO_KEY_7 },
    { 0, IKO_KEY_8 },
    { 0, IKO_KEY_9 },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_SEMICOLON },
    { 0, IKO_KEY_SEMICOLON },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_COMMA },
    { 0, IKO_KEY_EQUAL },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_DOT },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_SLASH },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_2 },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_A },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_B },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_C },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_D },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_E },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_F },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_G },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_H },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_I },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_J },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_K },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_L },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_M },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_N },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_O },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_P },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_Q },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_R },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_S },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_T },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_U },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_V },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_W },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_X },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_Y },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_Z },
    { 0, IKO_KEY_LEFTBRACE },
    { 0, IKO_KEY_BACKSLASH },
    { 0, IKO_KEY_RIGHTBRACE },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_6 },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_MINUS },
    { 0, IKO_KEY_GRAVE },
    { 0, IKO_KEY_A },
    { 0, IKO_KEY_B },
    { 0, IKO_KEY_C },
    { 0, IKO_KEY_D },
    { 0, IKO_KEY_E },
    { 0, IKO_KEY_F },
    { 0, IKO_KEY_G },
    { 0, IKO_KEY_H },
    { 0, IKO_KEY_I },
    { 0, IKO_KEY_J },
    { 0, IKO_KEY_K },
    { 0, IKO_KEY_L },
    { 0, IKO_KEY_M },
    { 0, IKO_KEY_N },
    { 0, IKO_KEY_O },
    { 0, IKO_KEY_P },
    { 0, IKO_KEY_Q },
    { 0, IKO_KEY_R },
    { 0, IKO_KEY_S },
    { 0, IKO_KEY_T },
    { 0, IKO_KEY_U },
    { 0, IKO_KEY_V },
    { 0, IKO_KEY_W },
    { 0, IKO_KEY_X },
    { 0, IKO_KEY_Y },
    { 0, IKO_KEY_Z },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_LEFTBRACE },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_BACKSLASH },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_RIGHTBRACE },
    { IKO_KEY_MOD_LSHIFT, IKO_KEY_GRAVE },
};

hid_buf_t hid_buf_new(uint8_t modifier, uint8_t key)
{
    hid_buf_t buf = { 0 };
    buf.modifier = modifier;
    buf.key = key;
    return buf;
}

void hid_send(const hid_buf_t buf)
{
    // Press key
    FILE* f = fopen("/dev/hidg0", "wb");
    fwrite(buf.data, sizeof(char), 8, f);
    fclose(f);

    usleep(10000);

    // Release key
    hid_buf_t nullbuf = { 0 };
    f = fopen("/dev/hidg0", "wb");
    fwrite(nullbuf.data, sizeof(char), 8, f);
    fclose(f);

    usleep(50000);
}

void hid_sendch(uint8_t ch)
{
    if (ch > 127)
        return;
    if (ch < 32)
        return;

    // offset ignore the first 32 symbols in ascii table
    ch = (uint8_t)(ch - 32);

    uint8_t mod = ascii_to_hid_key_map[ch][0];
    uint8_t key = ascii_to_hid_key_map[ch][1];
    hid_buf_t buffer = hid_buf_new(mod, key);

    hid_send(buffer);
}

void hid_sendstr(char* str)
{
    size_t l = strnlen(str, INT32_MAX);
    for (size_t i = 0; i < l; i++) {
        hid_sendch((uint8_t)str[i]);
    }
}
