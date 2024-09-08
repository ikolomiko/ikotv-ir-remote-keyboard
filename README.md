# ikoTV IR Remote Keyboard

This project is an amalgamation of [ir-keytable](https://linux.die.net/man/1/ir-keytable), [Composite USB Gadgets on the Raspberry Pi Zero](https://www.isticktoit.net/?p=1383) and [this GitHub Gist containing USB HID keycodes](https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2).
The IR receiver connected to the GPIO pins of Raspberry Pi Zero receives IR signals and those signals gets decoded by the LIRC modules in the kernel. These IR events are accessible from `/dev/lirc0` and `/dev/input/event0`. This program reads events from these files
and creates corresponding USB HID reports if the received IR remote key is mapped to a specific USB keyboard key. These mappings are hard-coded into the source code to avoid unnecessary complexity. Then, the USB HID reports are transmitted to the host PC via writing them
to the device file `/dev/hidg0`. Essentially, this program makes the Raspberry Pi Zero act like a IR-controlled USB keyboard.

## Build
`<PREFIX>` is a placeholder for the actual installation prefix path. The default is `/usr/local` if none is supplied. Should be set to `/usr` when creating a system package.

```sh
meson setup build --prefix=<PREFIX>
ninja -C build
```

## Install

```sh
ninja -C build install
```

## Run
Enable and start the systemd unit.

```sh
systemctl enable --now ikotv-usb
```
