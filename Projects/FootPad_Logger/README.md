This project had purpose of 'Logging People Stepping on Plates'

I installed the Hardware in our school's Cafeteria to see 'Congestion' in Real-TIME.

Hardware I made [materials] : 
1) 5 Foot-Pads [Cork(laser-cutted) + Acryl(laser-cutted) + Membrane Tactile Switch]
2) Enamel-Wire connection between all 5 pads. Single GROUND & 5-unique WIRE. (Total 6).

The Logging was mostly done between '2017/11/06' ~ '2017/12/12'

<b>Detailed Instructables Available! (https://www.instructables.com/id/FootPadLogger/) </b>

****************************************************************************************

This Project gave me an 'objective' view on our school's congestion, mostly at Lunch Time.
(e.g. 12:25 PM congestion occurs EVERYDAY!)

And it gave me experience on ESP-8266 development board (Wemos D1 mini). The process of
debugging and adding more function(e.g. CLI via Bluetooth(Serial)) was really FUN!

There are alot more improvements that can be made, but I don't have reason to do it now, since
the logging is finished, and I got the Data the I want(Over 1 month!).

But, I might revist this code someday later, and might fix it / improve it!

#Just a reminder on What Error this Code Has#
=> the code only records 20[s] out of 24[s] Server-upload period. This is because client.connect()
and client.print() took 4 seconds. Way to improve this is to use 'Timer Interrupt', which I don't know
how to implement on ESP-8266.