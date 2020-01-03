### ID EEPROM

Install J1 (or solder a zero-ohm resistor in R1) to defeat EEPROM
write protect.

Edit `/boot/config.txt`, adding this line temporarily so the ID bus is visible
after boot, and reboot:
```
# Enable ID EEPROM temporarily
dtparam=i2c_vc=on
```

Verify EEPROM is found on bus 0 at address 50
```console
$ sudo apt install i2c-tools
$ i2cdetect 0
WARNING! This program can confuse your I2C bus, cause data loss and worse!
I will probe file /dev/i2c-0.
I will probe address range 0x03-0x77.
Continue? [Y/n] y
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: 50 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --
```

Make the HAT EEPROM utils and copy `eepmake`, `eepdump`, and `eepflash.sh`
somewhere in your PATH (like `/usr/local/bin`).
```console
$ git clone https://github.com/raspberrypi/hats.git
$ cd hats/eepromutils
$ make
$ sudo cp eepflash.sh eepdump eepmake /usr/local/bin/
```

Back in this directory, make the image and flash the chip:
```console
$ make
eepmake board.txt board.eep
Opening file board.txt for read
UUID=3691bbab-1f52-42c7-9cac-01fc81d12688
Done reading
Writing out...
Done.
$ make flash
sudo eepflash.sh -w -f=board.eep -t=24c32 -d=0 -a=50
This will attempt to talk to an eeprom at i2c address 0x50 on bus 0. Make sure there is an eeprom at this address.
This script comes with ABSOLUTELY no warranty. Continue only if you know what you are doing.
Do you wish to continue? (yes/no): yes
Writing...
0+1 records in
0+1 records out
105 bytes copied, 0.388654 s, 0.3 kB/s
Closing EEPROM Device.
Done.
```

Edit `/boot/config.txt`, removing the line added above, then reboot.
Then, again from this directory, run:
```console
$ make verify
vendor:      WorlickWerx
product:     pi-parport
product_id:  0x0001
product_ver: 0x0003
uuid:        3691bbab-1f52-42c7-9cac-01fc81d12688
```

Yay it's a HAT.

References:

[Sparkfun i2c tutorial](https://learn.sparkfun.com/tutorials/raspberry-pi-spi-and-i2c-tutorial/i2c-0-on-40-pin-pi-boards)
