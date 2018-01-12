<WHAT THE PROGREM DOES>
1) Reader : reads in PPM-signal from Pin-2 of Arduino UNO, takes average of channel 1 ~ 4 every 0.5[s], prints them out on Serial Port(115200 baud). This way, Most controlled channels can be trimmed, to have 1500 value, when centered.

2) PPM_UnoJoy : reads in PPM-signal, and spits them out through on-board 'mega16u2' chip, which would have to be reprogramed utilizing 'DFU-mode', and it Acts, Seems like a Joystick to the Computer. So you can play Drone-Simulation, with UNO as middle-man, with your RC-Transmitter.

--------------------------------------------
*This code is based on iforce2d's work.
VIDEO(https://www.youtube.com/watch?v=cZc-23NjNHY) / Source-Code(http://www.iforce2d.net/sketches/diyGamepadSketches.zip)

*The Improvememt(?) I've added are
- Averaged-value display for Trim-value Calculation with Ease.(With live-mode, it is so fast, you can't read the 1st digit!)
- When 'using-ppm', don't digitalRead and analogRead unnecessarily. Only PPM variables gives information. So it should be little faster.
- I have 8-channel, so modified that also.
----------------------------------------------------------------------------------------------------------------------

Yesturday, I wondered if there is a way for an Arduino-UNO to have 'Keyboard.h' working on it.

So today, I searched on the Internet, then found that by utilizing 'Device-Firmware-Update', Arduino UNO's

onboard 'mega16u2' chip can be reprogrammed, and it would broadcast itself as for example, 'Joystick' to the computer.

I wanted to use 'UnoJoy' because I've seen it in iforce2d's video && It seemed pretty popular, easy to use.

So after making that decision at 5PM (180112-friday), I was flying a quad simulation (FPV_freerider) with my RC-controller at 10:30 PM.

THE PROCESS was REALLY, REALLY confusing. I followed <https://github.com/AlanChatham/UnoJoy> the manual.

Here are (as I remember) the process of getting UnoJoy working properly.

1. Download UnoJoy zip file from Github(https://github.com/AlanChatham/UnoJoy) and extract it somewhere.

2. Install 'Flip_3.4.7(http://flip.software.informer.com/3.4/)'. (only on Windows OS), this is basically a chip-flasher.

3. Install dfu-programmer(https://sourceforge.net/projects/dfu-programmer/files/dfu-programmer/0.7.2/dfu-programmer-win-0.7.2.zip/download)

4. Inside dfu-programmer, ./dfu-prog-usb-1.2.2/ , there will be a file Named "atmel_usb_dfu.inf", RIGHT-CLICK on it to install the Driver for mega-16u2 => THIS IS VERY IMPROTATNT, if you don't have the driver, basically your computer can't identify UNO at dfu-mode!!!

5. For dfu-programmer to work, it needs one more file(don't know why, and I'm not sure if 'dfu-programmer was ever used in the end ;)') called 'libusb0.dll' => this can be downloaded from (https://sourceforge.net/projects/libusb-win32/files/libusb-win32-releases/1.2.6.0/libusb-win32-bin-1.2.6.0.zip/download). 

6. Inside that libusb-file folder, go to ./bin/x86/ and there will be "libusb0_x86.dll" file, CHANGE it's name to 'libusb0.dll' and put that file on the same directory where 'dfu-programmer.exe' is located at, whihc is the program you would have downloaded from No.3 instruction!

7. You are ALL SET! Check for few things.

1) Look at 'Device-Manager', see if your Arduino-UNO is maked as 'Arduino-UNO'.

2) Now 'short' 2 pins as marked on the picture (https://www.arduino.cc/en/uploads/Hacking/Uno-front-DFU-reset.png)

3) You would see 'Device-Manager' refreshing, because they are not detecting UNO anymore.

4) IF, you have installed the 'mega-16u2 driver' from Step No.4, you should see 'mega16u2' POP-UP as a device, instead of UNO as previousluy it had been.

5) NOW, go to UnoJoy folder you've downloaded. Go to(there are one more folder under the same name)
./UnoJoy : Then, double-click "TurnIntoAJoystick.bat" and see if the command-prompt tells you SUCCESS!

6) You can also double-click "TurnIntoAnArduino.bat" to see if that works too.

7) Basically, you are reprogramming the whole 16-u2 chip by double-clicking the .bat files!!!

8. If you can SUCCESSFULLY reprogram the chip on UNO, objective is complted!!!

**HOW TO UPLOAD ARDUINO-CODE?
1) Simple, in 'DFU-Mode(short two pins for a brief second)', double-click "TurnIntoAnArduino.bat".
2) Unplug the UNO [Because DFU-mode will kept ON]

3) Reconnect the UNO
4) Upload the Sketch with Arduino-IDE as usual(you should be able to see the device under IDE's port tab)

5) Put UNO into DFU-mode(short two pins)
6) double-click "TurnIntoAJoystick.bat"
7) Unplug the UNO [Because DFU-mode will kept ON]

8) Reconnect the UNO, it will be working as a JoyStick NOW, be aware that in this Joystick-mode, Arduino-IDE WON't be able to detect UNO in Ports tab. That is Completely Fine!

-------------------------------------------------------------------

Conclusion : It was pretty overwhelming rabbit-hole-journey, but in the end, it seems like learning how to re-program the on-board 16u2 chip was

definetely worth it. It is a new skill, got to know about Atmel's very old-looking program 'Flip' ... And, I can now fly the drone-simulator with 

my RC-Transmitter! FUN!

-------------------------------------------------------------------