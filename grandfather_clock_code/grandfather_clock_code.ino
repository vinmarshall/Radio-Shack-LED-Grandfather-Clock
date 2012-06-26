/* LED Grandfather Clock v1.0
 *
 * A Grandfather Clock that chimes the quarter, half, and full hours
 * by way of RGB LEDs.  There is no external time display other than 
 * these periodic chimes - it communicates the passing of time, not 
 * the time itself.
 *
 * This code is for the Radio Shack LED Grandfather Clock project in the
 * July 2012 issue of PopSci.  
 *
 * Please check GitHub for the most recent version of this code.  The
 * code and other project files live on GitHub at:
 * https://github.com/vinmarshall/Radio-Shack-LED-Grandfather-Clock
 * 
 * Copyright (c) 2012 Vin Marshall (vlm@2552.com, www.2552.com)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE. 
 * 
 */

#include <Time.h>  
#include <Wire.h>  
#include <DS1307RTC.h> 

#define RED_PIN 9
#define GREEN_PIN 11
#define BLUE_PIN 10
#define FADE_STEP 1.2
#define FADE_TIME_MS 50
#define CHIME_PAUSE_MS 1000

struct RGB { 
  byte red;
  byte green;
  byte blue;
};

/*
 * Set these AM and PM color ratios to get the colors you want.  
 * Values for each color can be anything from 0 to 255 but it works
 * best to keep these values relatively small so that the "chime" will
 * be noticably brighter than the normal glow.
 */
RGB am_color = {12, 2, 7};
RGB pm_color = {2, 12, 7};

void setup() {     
  // Start Serial comm
  Serial.begin(9600);  
  
  // Set the clock based on this code's compile time.  Make
  // sure the compiling computer's clock is set correctly.
  setTimeByCompileTime();
  
  // DEBUGGING
  // Just before the 10 AM hour chime
  //setTime(9, 59, 55, 1, 1, 2012); 
  // Just before the 4 PM hour chime
  //setTime(15, 59, 55, 1, 1, 2012);
  // Just before a quarter hour chime
  //setTime(0, 14, 55, 1, 1, 2012);
  // Just before a half hour chime
  //setTime(0, 29, 55, 1, 1, 2012);
  
  // Ensure we have a time set.
  if(timeStatus()!= timeSet)
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");
  
  // initialize the digital pin as an output.
  pinMode(RED_PIN, OUTPUT);     
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

RGB current_color;
int last_minute;

void loop() {
  
  time_t time = now();
  
  // Choose the glow color by AM/PM
  if (isAM(time)) { current_color = am_color; }
  else { current_color = pm_color; }
  setLEDs(current_color);
  
  // Chime on quarter, half, and full hours
  // Check every time through the loop for a new minute
  if (minute(time) != last_minute) {
    if (minute(time) == 0) { chime(current_color, hourFormat12(time)); }
    else if (minute(time) == 30) { chime(current_color, 2); }
    else if (minute(time) % 15 == 0) { chime(current_color, 1); }
  }

  // Take a rest.  This clock is not a study in punctuality.
  last_minute = minute(time);
  delay(500);
}

/*
 * chime
 * Fades an RGB LED up and down keeping the color mix ratio
 * consistent.  
 * color struct: red, green, blue 0-255 starting color values
 * num: number of times to cycle the fade up and down
 *
 * Expects FADE_STEP to be defined
 */
void chime(struct RGB color, int num) {
  unsigned int init_red = color.red;
  unsigned int init_green = color.green;
  unsigned int init_blue = color.blue;
  
  // Fade up and down 'num' times
  for (int i=0; i < num; i++) {
    
    // start at the initial color mix ratio
    unsigned int red = init_red;
    unsigned int green = init_green;
    unsigned int blue = init_blue;
    
    // Fade up, keeping the ratio intact.  Step when the 
    // brightest color component reaches full power.
    while (red < 255 && green < 255 && blue < 255) {
      red = ceil(red * FADE_STEP);
      green = ceil(green * FADE_STEP);
      blue = ceil(blue * FADE_STEP);
      
      // and adjust the LEDs to the current values
      setLEDs(red, green, blue);
      delay(FADE_TIME_MS);
    }
    
    // Pause to accentuate the chime
    delay(CHIME_PAUSE_MS);
    
    // Fade down, keeping the ratio intact.  Step when the 
    // brightest color component reaches the original power level.
    while (red > init_red && green > init_green && blue > init_blue) {
      red /= FADE_STEP;
      green /= FADE_STEP;
      blue /= FADE_STEP;
      
      // and adjust the LEDs to the current values
      setLEDs(red, green, blue);
      delay(FADE_TIME_MS);
    }
    
    // Pause to accentuate the chime
    delay(CHIME_PAUSE_MS);
  }
}

/*
 * setLEDs
 *
 * Expects RED_PIN, GREEN_PIN, and BLUE_PIN to be defined and
 * configured as output pins.  
 */
void setLEDs(int red, int green, int blue) {
  analogWrite(RED_PIN, limitTo(red, 0, 255));
  analogWrite(GREEN_PIN, limitTo(green, 0, 255));
  analogWrite(BLUE_PIN, limitTo(blue, 0, 255));
}

void setLEDs(struct RGB color) {
  setLEDs(color.red, color.green, color.blue);
}

/*
 * limitTo
 * Limits the supplied val to be within min and max.  Returns 
 * the constrained value.
 */
int limitTo(int val, int min, int max) {
  if (val < min) { val = min; }
  else if (val > max) { val = max; }
  return val;
}
  
/*
 * setTimeByCompileTime
 * Sets the connected RTC based on the time and date at which this program
 * was compiled which is a reasonably accurate way to grab the time.  
 * That clever trick is courtesy of Adafruit:
 * http://www.ladyada.net/learn/breakoutplus/ds1307rtc.html
 */
void setTimeByCompileTime() {
  
  int hh, mm, ss;
  int MM, DD, YYYY;
  char MM_name[4];
  
  // Split the time and date
  // __TIME__ format: hh:mm:ss
  // __DATE__ format: Mon DD YYYY 
  sscanf(__TIME__, "%d:%d:%d", &hh, &mm, &ss);
  sscanf(__DATE__, "%s %d %d", &MM_name, &DD, &YYYY);

  // find month number for the UTC month short name
  for (MM = 0; MM < 13; MM++) {
    if (strcmp(monthShortStr(MM), MM_name) == 0)
      break;
  }
  
  // And set the clock to the parsed compile time and date
  setTime(hh, mm, ss, DD, MM, YYYY);
}

