/*
___.           ________                .__   .________ _______________  _______  _______       ___  ___        
\_ |__ ___.__. \_____  \_______   ____ |  |  |   ____//  _____/\   _  \ \   _  \ \   _  \     /  /  \  \    /\ 
 | __ <   |  |  /   |   \_  __ \_/ __ \|  |  |____  \/   __  \ /  /_\  \/  /_\  \/  /_\  \   /  /    \  \   \/ 
 | \_\ \___  | /    |    \  | \/\  ___/|  |__/       \  |__\  \\  \_/   \  \_/   \  \_/   \ (  (      )  )  /\ 
 |___  / ____| \_______  /__|    \___  >____/______  /\_____  / \_____  /\_____  /\_____  /  \  \    /  /   )/ 
     \/\/              \/            \/            \/       \/        \/       \/       \/    \__\  /__/       
*/

#include <FastLED.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

/* CONFIG */
#define LED_PIN     	7
#define BUTTON_PIN  	2
#define NUM_LEDS    	40 	// Number of leds
#define TIMEZONE		2 	// Your country timezone, beacuse the GPS gets the global time
#define KMPH 				// comment this line if you use mp/h
#define SPEEDMULTIPLY 	1.6 // This is the top speed which will display red leds, 1.6 means 100kmp/h or 100mp/h, by this calculation: 160 / (KMP/H or MP/H to top speed) = This value (Speed measurment is depends on the previous define)
#define AFTERNOON 		15 	// "Night time" - The time when the leds will start display the interior leds (Don't forget to set the timezone value)
#define MORNING 		6 	// "Morning time" - The time when the leds won't work until the night time (Don't forget to set the timezone value & Remember that it's still possible to be activated with a button click)

//#define DEBUG 			// debug mode

static const int RXPin = 3, TXPin = 4;
static const uint32_t GPSBaud = 9600;

/* CONFIG END */

CRGB leds[NUM_LEDS];
// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void SetLedColors(int h);
bool forceDisable = false;

void setup() {
	#if defined(DEBUG)
		Serial.begin(115200);
	#endif
	pinMode(BUTTON_PIN, INPUT_PULLUP);
	ss.begin(GPSBaud);
	
	FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
	fill_solid(leds, NUM_LEDS, CRGB::Black);
	FastLED.show();
	
	forceDisable = EEPROM.read(0) == 1;
	#if defined(DEBUG)
		Serial.print("Reading EEPROM, forceDisable: ");
		Serial.println(forceDisable);
	#endif
}

int lastColor = 160;
int hour = -1;
bool effectStarted;
void StartEffect()
{
  effectStarted = true;
  for(int i = 255; i > 160; i--)
  {
    SetLedColors(i);
    delay(50);
  }
  delay(5000);
}

void loop() {
  // Dispatch incoming characters
  while (ss.available() > 0)
    gps.encode(ss.read());

  while (!effectStarted)
  {
    if(digitalRead(BUTTON_PIN) == LOW) // Button press when morning time or gps didn't get the time yet
    {
	  #if defined(DEBUG)
			Serial.println("Button pressed");
	  #endif
      EEPROM.update(0, 0);
      StartEffect();
      continue;
    }
    while (ss.available() > 0)
      gps.encode(ss.read());

    if(gps.time.isUpdated())
    {
      int second = gps.time.second();
      if (second == 0)
        continue;
      hour = gps.time.hour() + TIMEZONE;
	  #if defined(DEBUG)
		  Serial.print("Hour: ");
		  Serial.println(hour);
		  Serial.println("Second: ");
		  Serial.println(second);
	  #endif
	  
      if(hour > AFTERNOON || hour < MORNING)
      {
		#if defined(DEBUG)
			Serial.println("Night");
		#endif
        StartEffect();
      }
      else
      {
        delay(3000);
      }
    }     

  }
  if(digitalRead(BUTTON_PIN) == LOW) // Button press after getting the time
  {
    forceDisable = !forceDisable;
    if(forceDisable)
    {
      // Turn off all LEDs
	  #if defined(DEBUG)
		Serial.println("Turning off all LEDs");
	  #endif
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
    }
    else
    {
      // Turn on all LEDs
	  #if defined(DEBUG)
		Serial.println("Turning on");
	  #endif
      StartEffect();
    }
    EEPROM.write(0, (forceDisable ? 1 : 0));
    delay(1500);
  }

  if (!forceDisable && gps.speed.isUpdated()) // When gps updates the speed, we update the color
  {
	#if defined(KMPH)
    double speed = gps.speed.kmph();
	#else
	double speed = gps.speed.mph();
	#endif
    SetColorsToSpeed(speed);
  }
  

}

void SetColorsToSpeed(float speed)
{
  float color = 160 - (speed * SPEEDMULTIPLY);
  if (color < 1) color = 1;
  int h = (int)color;
  if (h > lastColor) // Slowing down
  {
    for(; lastColor < h; lastColor++)
    {
      SetLedColors(lastColor);
      delay(10);
    }
  }
  else if (h < lastColor) // Speeding up
  {
    for(; lastColor > h; lastColor--)
    {
      SetLedColors(lastColor);
      delay(10);
    }
  }
  lastColor = h;
}

void SetLedColors(int h)
{
  if(forceDisable) return;
  fill_solid(leds, NUM_LEDS, CHSV(h, 255, 255));
  FastLED.show();
  FastLED.setCorrection(TypicalLEDStrip);
}