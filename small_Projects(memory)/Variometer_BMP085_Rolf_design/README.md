Original Code(https://www.rcgroups.com/forums/showthread.php?1749208-DIY-simple-and-inexpensive-Arduino-based-sailplane-variometer)

I first saw this through iforce2d's video on Youtube. But didn't try it.
And with little modification utilizing Adafruit's BMP085 library, you can use
BMP085 with this sketch.

July 7th(Saturday), 2018
was the day I made this sketch.

I also found out that 'ddsAcc' variable, which I think was meant to be 'DISTANCE[m]' thing, was Overflowing constantly. Because it is 16bit, it overflows near 32k point. And so when rising, it would overflow much faster, and it creates that unique variometer sound that way.

I don't know if that is intentional or not, but pretty cool to see it working. And of course, if you change ddsAcc to unsigned long(32 bit), it DOESN'T have that unique sound. Because it doesn't overflow, and it never goes Negative....

I also got to learn about how 'Tone' library works, and that it only support Timer 2 for now. And it uses timer copare match interrupt!! Pretty exciting stuff.