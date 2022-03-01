# Videos and pictures:

![Image 1](https://i.imgur.com/eiqfxVu.jpg)
[Video 1](https://www.youtube.com/watch?v=nSZqGfcXmYQ)
[Video 2](https://www.youtube.com/watch?v=1BGZK5MtQjw)

# How does it work?:

- When the Arduino starts, we get the time from the GPS, and if this is nighttime (changeable) the LEDs will be activated.
- Leds' colors are controlled by the speed, which will be represented from 0 KMP/H (Dark Blue) to 100 KMP/H (Red).
(Can be changed to MP/H)
- If this is morning time or the GPS didn't get the time yet, you can press the button and force it to be activated.
- If the interior light is working and you press the button, it turns all the lights off and saves it in the EEPROM so it won't be auto-activated next time you start the Arduino.


# Requirements:
- Adruino
- GPS that supports TinyGPSPlus library
- Addressable leds (Low density is recommended)

## Recommended:

- Button (To control if you want to turn off/on the light mid-drive)

## Available configs:
- Leds pin
- Button pin
- Number of leds
- Timezone
- KMP/H or MP/H
- Top speed which will be displayed as red
- Night time
- Day time
- Debug mode
