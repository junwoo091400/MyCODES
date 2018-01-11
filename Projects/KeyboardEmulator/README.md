2018 / 01 / 12

Created this because I didn't have seperate Keyboard with USB cable.

This program, when uploaded to Pro-Micro (32U4 chip), gets TTL-Serial from RX1 pin,

then writes it to USB-port(Keyboard class).

* Also, I needed 'Arrow-down' button, so I dedicated Pin-4 as a INPUT_PULLUP Switch for that.

 => It was pretty fun doing it! (now it is 6:27 AM. Gotta sleep, I first started playing with Raspberry-pi at around 00:30 AM.)

 **I learned that 'Keyboard' library isn't supported on Arduino-UNO(Atmega328P chip). B/c it doesn't have USB-emulating circuit (internally.)