<DESCRIPTION>

Servo library using Timer 2.

Able to support up to 10 servos(limited only due to keep refresh rate over 50Hz).

<WHY>

I created this library, which honestly looks & works very similar to Servo library on Arduino, to test VESC that was reading wrong values from Arduino Nano for my nRF24_Remote project.

***
<More Info>

This library uses 'Fast PWM mode Timer' with 'Overflow Interrupt' AND 'Output Compare Match B Interrupt'.

The reason I used TWO, yes, not just one, TWO!!! interrupts is becasue I wanted to have a reliable Signal Frame, which is Overflow.

And when overflow is reached but needs more pulse width, "OCIE2B" bit is SET at "TIMSK2". And when that interrupt happens, it pre-loads next servo's pulse value to the static global varaibles(servoIdx, Tick, ...). And then, when NEXT overflow happens, the next pulse begins.

***
<Motivation & Ideas>

BLOG(https://dannyelectronics.wordpress.com/2017/02/08/driving-multiple-servos-with-avr-timer2/) helped a lot. This blog provides a code that ONLY uses 'overflow interrupt'. Which is FINE, and much shorter.

ESK8_BUILDER_FORUM(https://www.electric-skateboard.builders/t/help-vesc-not-reading-arduino-ppm-signals-correctly-but-reads-hobby-rx-correctly/60952/8) => My investigation on Arduino generating PWM that VESC can understand... And this library was created for that... haha.

I learned a lot about Timer Interrupts Tho!

***
<Chit Chat>
I still kind of don't understand why I actually used 2 interrupts... Haha. I actually did want to create CONSTANT period pulse, but that didn't happen.//Already done well by Servo library btw :)