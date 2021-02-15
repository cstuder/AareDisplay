# Aaredisplay

Digital Uhr Wecker "No Time Left" Blau by Karlsson (Ebay, 9.4.2006)

=> als Aare-Temperatur-Anzeige verwenden, Daten per MQTT an einen ESP8266 im Gehäuse schicken.

Gehäuse: 14 x 7.5 x 6.5 cm (Aussen, Dicke Wände 2 mm)

## Eingebaut

4-stellige 7-Segment-Anzeige mit Punkten. 9 x 2.5 cm (1 inch)

Spannung auf dem Display: 1V

30 Pins = 4 x 7 + GND + Punkte in der Mitte + Punkt oben links = 31?
Wahrscheinlich sind die Punkte in der Mitte nicht geschaltet, aber woher haben die den Strom?

Nur 24 Pins sind am Display angeschlossen.

GND ganz rechts bei der 30

Segmentanzeige: P-955-B-D

Hinten:
FTTL-955TBEN-RB
1-03-6409100-00
TS20406

### TODO

- Flachbandkabel auf der Kontrollerplatine auslösen. Sollte genau aufs Breadboard passen

- Wie komme ich zu 1V-Spannung?
- Wie multiplexe ich jetzt das? => SPI Portexpander, I2C, mehrere

-> Charlieplexing: 31 LEDs könnten mit 7 Pins gesteuert.

http://www.instructables.com/id/Charlieplexing-LEDs--The-theory/?ALLSTEPS
http://www.instructables.com/id/Charlieplexing-with-the-Raspberry-Pi/?ALLSTEPS

-> Multiplexing:

http://www.instructables.com/id/Multiplexing-with-Arduino-and-the-74HC595/

**Gefunden**: Das alte Display ist direkt mit dem Zeitmacher-Chip LM8560 verlinkt und hat ein 50 Hz-Multiplexing auf zwei Pins, jeder Pin ist danach für zwei Segmente zuständig. Auf dem 10-Stunde kann man effektiv nur die Segmente ansteuern, welche auch benötigt werden für 1/2.

Datenblatt: http://www.paulanders.com/G5-LED/ver1/datablad.pdf

## I2C-Variante

ESP kommuniziert über I2C mit einem MCP23017 Port Expander, welcher 16 Input/Output-Pins anbietet.

**Ziel**: Das alte Display mit Hilfe zweier MCP's ansteuern.

### Zwischenschritt: Zwei LEDs ansteuern

Proof of Concept um den Port Expander zu testen: `blink2LedsI2C`

Einführung: http://www.instructables.com/id/Adding-an-MCP23017-IO-Extender-to-Arduino-or-ESP82/

Es gibt zwei Libraries: Wire und Adafruit MCP.

SDA und SCK können mit der Wire-Library auf beliebige Pins gesetzt werden: `Wire.pins(SDA, SCK)`

| ESP-Pin    | Funktion | MCP-Pin |
| :--------- | :------- | :------ |
| D2 (GPIO4) | SCK      | 12      |
| D1 (GPIO5) | SDA      | 13      |
| 3V3        | V_DD     | 9       |
| GND        | V_SS     | 10      |
| GND        | A0       | 15      |
| GND        | A1       | 16      |
| GND        | A2       | 17      |
| 3V3        | RESET    | 18      |

Adressierungspins `A0, A1, A2` alle auf Low, d.h. Adresse `0` bzw. `0x20`.

Danach LEDs mit GP-Pins verbinden.

Reset muss immer auf HIGH sein.

### Zwischenschritt: Sparkfun-Display

Proof of Concept, benötigt nur 12 Pins und muss nicht herausgelötet werden.

Projekt: `fourDigit12C`

Datasheet: `20001952C.pdf`

| Display | Segment | MCP-Pin | MCP-Name |
| :------ | :------ | :------ | :------- |
| 1       | E       | 5       | GPB4     |
| 2       | D       | 4       | GPB3     |
| 3       | .       | 8       | GPB7     |
| 4       | C       | 3       | GPB2     |
| 5       | G       | 7       | GPB6     |
| 6       | D4      | 24      | GPA3     |
| 7       | B       | 2       | GPB1     |
| 8       | D3      | 23      | GPA2     |
| 9       | D2      | 22      | GPA1     |
| 10      | F       | 6       | GPB5     |
| 11      | A       | 1 (.)   | GPB0     |
| 12      | D1      | 21      | GPA0     |

Funktioniert!

### Altes Display ansteuern mit zwei Chips

TODO continue here

## Neues Display

[Sparkfun (Blau, 2.95$, Common Cathode)](https://www.sparkfun.com/products/11408)

Common Cathode heisst hier: Die Digits mit Vorwiderstand versehen.

Anleitung: [https://www.hackster.io/meljr/sparkfun-com-11408-4-digit-7-segment-display-hookup-guide-4b4d9e](https://www.hackster.io/meljr/sparkfun-com-11408-4-digit-7-segment-display-hookup-guide-4b4d9e)

Arduino-Libraries: - https://github.com/renes/fourDigitLCDDisplay - http://playground.arduino.cc/Main/SevenSegmentLibrary

**Pin-Hookup**

| Display | Segment | Arduino   | ESP |
| :------ | :------ | :-------- | :-- |
| 1       | E       | 16 ~~6~~  | D0  |
| 2       | D       | 5         | D1  |
| 3       | .       | -         | -   |
| 4       | C       | 4         | D2  |
| 5       | G       | 14 ~~8~~  | D5  |
| 6       | D4      | 13        | D7  |
| 7       | B       | 9 ~~3~~   | SD2 |
| 8       | D3      | 12        | D6  |
| 9       | D2      | 15 ~~11~~ | D8  |
| 10      | F       | 0 ~~7~~   | D3  |
| 11      | A       | 2         | D4  |
| 12      | D1      | 10        | SD3 |

=> Funktioniert irgendwie nicht...

### One Digit Test

Source: oneDigit.ino

Neu Belegung:

| Segment | ESP-Pin | Arduino-Pin |
| :------ | :------ | :---------- |
| A       | D8      | 15          |
| B       | D7      | 13          |
| C       | D6      | 12          |
| D       | D5      | 14          |
| E       | D4      | 2           |
| F       | D3      | 0           |
| G       | D2      | 4           |
| .       | D1      | 5           |
| D1      | D0      | 16          |
| D2      | SD2     | 9           |
| D3      | SD3     | 10          |
| D4      | -       | -           |

-> Funktioniert!

Kamiquasi / about 4 years ago / https://www.sparkfun.com/products/11406#comment-5191aa37757b7fba27000000

I do agree that datasheets can be cryptic at times, so let me try and clarify this one a bit because all the information you need does appear to be in there, specifically in sections 3 and 4.

In section 3 you’ll find a mechanical outline that also indicates the location of pin 1. With the display facing up, and the decimal points at the bottom, pin 1 is on the bottom-left. The crossmark in that drawing is way off, but let’s look past that and just go with ‘bottom left’. There’s two rows of pins, one at the bottom and one at the top. Pin counting is generally done in a counter-clockwise fashion. So if the pin on the bottom row on the far left is pin 1, then the pin to the right of it is pin 2, the one to the right of that is pin 3 and so on until you get to pin 6. Then you skip up to the top row, all the way to the right, that’s pin 7, and then the pin to the left of that is pin 8, etc. until pin 12.
If you look very, very closely at the bottom of this component, pins 1 and 12 should actually be marked out - but it’s pretty tiny and under a layer of some murky plastic so that may be hard to spot :)

Then there’s the (in this case) yellow drawing of the digits with labels DIG1 through DIG4 - the digits, D1 through D4 - the decimal points, and on DIG4 there’s some extra labels, A through G - the segments. Those are basically for reference. The segment labels A though G also apply to DIG1 through DIG3.

Then there’s the actual schematic of how this component is internally wired up. You can probably tell there’s 4 distinct sections to it for each digit and also labeled as such. Let’s focus on just the left-most one for now.
At the top of that one you see a label “12” with below it “COM”. That tells us that the pin number being indicated is pin 12 (the top-left one!), and that’s the common pin. Even without the “COM” that should be pretty easy to see as it is connected to every single LED’s cathode in that group.
Then at the bottom you see pins 11, 7, 4, 2, 1, 10, 5 and 3, which are hooked up to the LEDs labeled A through G and DP respectively. Those A through G labels are the same labels used for the segments in the yellow drawing. If we take pin 11 (right next to pin 12), it tells us that’s hooked up to segment A, and the yellow drawing tells us that’s the top segment of the 7-segment digit. If we take the one next to it, pin 7, that’s hooked up to segment B which according to the yellow drawing is the top-right segment of the 7-segment digit.
We can go through all of those until we get to the one labeled DP. There’s no ‘DP’ in the yellow drawing, but it’s referring to the decimal point. Since we’re looking at DIG1, DP in this case is actually referring to D1.

Now you could take the continuity tester of your multimeter (if you have one - if you don’t, please get one!), or your 5V power AND a 180 Ohm or greater resistor in series with that power to test these segments. The reason you’d want a 180 Ohm or greater resistor is because these LEDs have a 1.9V forward voltage drop (see description), and applying 5V will quickly make the LEDs burn out. The 180 Ohm resistor drops the excess voltage and keeps current down to well below the 20mA maximum (again, see description - the datasheet mentions 35mA, but I wouldn’t generally risk it for continuous power.)

Now let’s say you’re hooking this up on a breadboard and want to make the letter H. If you look at the yellow drawing, you’ll see that you’d need segments F, B, G, E and C. If you then look at the circuit diagram, you can trace those segments back to pins 10, 7, 5, 1 and 4. So all you’d have to do is connect the positive bit of your power supply (remember to use the resistor! 1 resistor per segment would be even better.) to those 5 pins, and the negative to the common pin, pin 12.

Now let’s say you wanted to display the letter H on the second digit instead. If you look at the circuit diagram closely, you’ll notice that pin 11 doesn’t just go to segment A on the first digit, but also to segment A on the second, third and fourth digits. The same applies to the other segment pins. The only one that actually changes, is the common pin - instead of connecting the negative power supply to pin 12, you’d connect it to pin 9 instead. If you try this on your breadboard, you should notice that the first digit goes out, and the second one displays the ‘H’.
The same thing applies to the third and fourth digits - the segment pins remain the same, only the common pin changes.

This is also how most of these multi-digit displays are driven by a microcontroller. Rather than applying power to each digit simultaneously, they just cycle through the 4, powering each digit one at a time by changing which common pin gets to be connected to the power, and changing the segment pins' states at the same time in order to display, say, the time, or some rudimentary text.

Now that may still sound a bit convoluted; for our brain it would certainly be a lot easier if each LED were simply 2 pins on the component. Of course the component would have 64 pins as a result, wasting a lot of space on a PCB. But a microcontroller doesn’t much care that it sounds convoluted, and a microcontroller with a 7-segment library where all you have to do is tell it which pin is what and from thereon just use the library to write out the desired numbers, means a lot less headache for us :)

If there’s still something unclear, feel free to ask! If not, have fun - and don’t be afraid to experiment (I’ve had to on mystery dot matrix displays without a datasheet at all before - talk about infuriating, but at least I’d know 100% for sure which pin was what). Just keep in mind that you don’t want to apply 5V straight to this :)

Müsste 1.2" sein, 4-digits, wenn möglich mit I2C.

Angebote:

- [https://www.adafruit.com/products/1270](https://www.adafruit.com/products/1270) (Rot, 17.5$, out of stock)
- [https://www.adafruit.com/products/1269](https://www.adafruit.com/products/1269) (Gelb, 17.5$)
- [https://www.adafruit.com/products/1268](https://www.adafruit.com/products/1268) (Grün, 18.75$)
- [https://www.sparkfun.com/products/11647](https://www.sparkfun.com/products/11647) (Blau, 16.95$, mit Controller)
- [https://www.sparkfun.com/products/11408](https://www.sparkfun.com/products/11408) (Blau, 2.95$, Common Cathode)
- [https://de.aliexpress.com/item/4PCS-Anode-1Bit-Digital-Tube-7-segment-1-2-inch-Red-LED-Display/32713215239.html](https://de.aliexpress.com/item/4PCS-Anode-1Bit-Digital-Tube-7-segment-1-2-inch-Red-LED-Display/32713215239.html) (4 einzelne Elemente, 2.21$, Common Anode)
- [https://de.aliexpress.com/item/4PCS-Anode-1Bit-Digital-Tube-7-segment-1-2-inch-Red-LED-Display/32711109448.html](https://de.aliexpress.com/item/4PCS-Anode-1Bit-Digital-Tube-7-segment-1-2-inch-Red-LED-Display/32711109448.html) (4 einzelne Element, 2.49$, Common Anode)

Adafruit kostet mindestens 15$ Porto.

Sparkfun sieht interessant aus, ohne Controller gibt's eine Anleitung hier: https://www.hackster.io/meljr/sparkfun-com-11408-4-digit-7-segment-display-hookup-guide-4b4d9e (Library hier: https://github.com/renes/fourDigitLCDDisplay)
