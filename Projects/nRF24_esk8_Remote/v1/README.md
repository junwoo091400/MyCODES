Version 1. WORKS.

Based on 2 projects!

1. SolidGeek's nRF24 remote (https://github.com/SolidGeek/nRF24-Esk8-Remote/tree/master)

2. Rolling Gecko's VESC4 Uart library (https://github.com/RollingGecko/VescUartControl)

<THINGS I CHANGED>
1. Enable SoftwareSerial for the receiver(using pin 2 as RX and 3 as TX for now) for VESC communication.
2. Decreased probability of receiving wrong data by using 'crc16' that VescUart library uses by default.
3. Changed for Transmitter, crc library to use 'PROGMEM'. Because the memory corrupt occured :(
4. Transmitter uses PROGMEM for icons(drawXBMP() on u8g2 library). To save SRAM.
5. Added crude 'Init EEPROM' menu for transmitter, so that I can set values to default easily.
6. I use Pin 5(D5) for Trigger, so changed that.
7. Other specific default values(my hall sensor top/bottom values, motor teeth count, etc.)