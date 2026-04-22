# Embedded

This project comprise of all my embedded applications on 3 platforms: atmega, stm32 and raspberry pi.
More details will be added soon.

## Build

This project uses SCons build system. One can install it over pip:
``` sh
pip install scons
```
Then  its needed to install avr compiler:

``` sh
bash scripts/deps.sh
```

Enter scons command to build all targets:
``` sh
scons
```


Troubleshooting

Problem:

Tried to hook up usb but it got disconnected with following message from dmseg:
```
[Tue Mar 12 15:14:58 2024] usb 1-2: usbfs: interface 0 claimed by ch341 while 'brltty' sets config #1
```
Solution:
Executed command
```
for f in /usr/lib/udev/rules.d/*brltty*.rules; do
    sudo ln -s /dev/null "/etc/udev/rules.d/$(basename "$f")"
done
sudo udevadm control --reload-rules
sudo systemctl mask brltty.path
```
Without need of reboot, everything worked fine, device appeared on /dev/ttyUSB0 path

