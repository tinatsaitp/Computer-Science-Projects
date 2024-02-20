// DHT22 Temperature & Humidity Sensor Library
// Website: https://www.arduino.cc/reference/en/libraries/dht-sensor-library/
#include <DHT.h>
#include <Wire.h>

// LCD Display Screen (16x2) 12C Library
// Website: https://www.arduino.cc/reference/en/libraries/liquidcrystal-i2c/
#include <LiquidCrystal_I2C.h>

// Constant values (won't change)
#define DHTPIN 2     // Sensor Pin
#define DHTTYPE DHT22   // Define which sensor we are using (DHT 22)
DHT dht(DHTPIN, DHTTYPE);   // Initialize DHT sensor for Arduino

// Define which sensor we are using (DHT 22)
LiquidCrystal_I2C lcd(0x27,20,4);

// Pins for the RGB LED
#define red_light_pin 10
#define green_light_pin 9
#define blue_light_pin 6

// Pin for the switch
#define switch_pin 12

// Pin for the fan
const int fan_control_pin = 5;

// Pins for pushbuttons
const int  Up_buttonPin = 8;
const int  Down_buttonPin = 7;
const int  Reset_buttonPin = 4;
const int  Start_buttonPin = 11;

// Variables (will change)
int switchStat = 0;   // the switch state

float hum;  // Stores humidity value
float temp; // Stores temperature value as Celsius
float fah;  // Stores temperature value as Fahrenheit
float hif;  // Compute heat index in Fahrenheit (the default)

int hiMax;    // The maximum HI for the fan
int hiMin;    // The minimum HI for the fan
int fanSpeed; // Current fan speed

int buttonPushCounter = 100;   // Counter for the number of button presses

// Count buttons
int up_buttonState = 0;           // Current state of the Up button
int up_lastButtonState = 0;       // Previous state of the Up button
int down_buttonState = 0;         // Current state of the Down button
int down_lastButtonState = 0;     // Previous state of the Down button
bool countPress = false;          // Set Count function haven't been pressed yet

int reset_buttonState = 0;         // Current state of the Reset button
int reset_lastButtonState = 0;     // Previous state of the Reset button
bool resetPress = false;           // Set Reset function haven't been pressed yet

int start_buttonState = 0;         // Current state of the Start button
int start_lastButtonState = 0;     // Previous state of the Start button
bool startPress = false;           // Set Start function haven't been pressed yet

bool blinking = false;             // Set Blinking function haven't been called yet



void setup(){
  Serial.begin(9600);

  // sensor begin
  dht.begin();
  // initialize the lcd
  lcd.init();

  // declare push buttons as input
  pinMode(Up_buttonPin , INPUT_PULLUP);
  pinMode(Down_buttonPin , INPUT_PULLUP);
  pinMode(Reset_buttonPin , INPUT_PULLUP);
  pinMode(Start_buttonPin , INPUT_PULLUP);

  // declare the LED as output
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);
  // turn off the LED
  digitalWrite(red_light_pin, HIGH);
  digitalWrite(green_light_pin, HIGH);
  digitalWrite(blue_light_pin, HIGH);

  // declare the toggle switch as input
  pinMode(switch_pin, INPUT);

  // declare the fan as output
  pinMode(fan_control_pin, OUTPUT);
  // turn off the fan
  digitalWrite(fan_control_pin, LOW);
}

void loop(){
    // read switch's state
    switchStat = digitalRead(switch_pin);

    //the switch turns on
    if (switchStat == HIGH){
      // Turn on the display:
      lcd.display();
      lcd.backlight();
      // Print current Heat Index to the LCD
      showCurrentHI(); 
      // Print the default input value to the LCD
      lcd.setCursor(0,1);
      lcd.print("Set Value: ");
      lcd.print(buttonPushCounter);
      lcd.print((char)223);
      lcd.print("F");

      // check if the Up and Down button are pressed
      checkUp();
      checkDown();
      if(countPress){
         countPress = false;
         // Print user input value to the LCD
         lcd.setCursor(11,1);
         lcd.print("     ");
         lcd.setCursor(11,1);
         lcd.print(buttonPushCounter);
      }
      
      // check if the Reset button are pressed
      checkReset();
      if(resetPress){
         resetPress = false;
         // Print the default input value to the LCD
         lcd.setCursor(11,1);
         lcd.print("     ");
         lcd.setCursor(11,1);
         lcd.print(buttonPushCounter);
      }

      // check if the Start button are pressed
      checkStart();  
      if (startPress) {
        // turn on the fan          
        turnOnFan();

        // LED stops blinking
        blinking = false;

        // LED stays on
        // Green Light
        if(hif < 90.0){
          digitalWrite(red_light_pin, HIGH);
          digitalWrite(green_light_pin, LOW);
          digitalWrite(blue_light_pin, HIGH);
         }
         // Yellow Light
         else if(hif < 103.0){
          digitalWrite(red_light_pin, LOW);
          digitalWrite(green_light_pin, LOW);
          digitalWrite(blue_light_pin, HIGH);
         }
         // Orange Light
         else if(hif < 125.0){
          digitalWrite(red_light_pin, 5);
          digitalWrite(green_light_pin, 215);
          digitalWrite(blue_light_pin, HIGH);
         }
         // Red Light
         else{
          digitalWrite(red_light_pin, LOW);
          digitalWrite(green_light_pin, HIGH);
          digitalWrite(blue_light_pin, HIGH);
         }
      } else {
        // LED is blinking
        blinking = true;
      }

      // LED blinking function
      if (blinking){
        // Blue light
        digitalWrite(red_light_pin, HIGH);
        digitalWrite(green_light_pin, HIGH);
        digitalWrite(blue_light_pin, LOW);
        delay(50);
        digitalWrite(red_light_pin, HIGH);
        digitalWrite(green_light_pin, HIGH);
        digitalWrite(blue_light_pin, HIGH);
        delay(50);
      }
   }
   else{
    // turn off the LED
    digitalWrite(red_light_pin, HIGH);
    digitalWrite(green_light_pin, HIGH);
    digitalWrite(blue_light_pin, HIGH);
    
    // Turn off the display
    lcd.noDisplay();
    // Turn off the screen's back light
    lcd.noBacklight();

    // re-state the Start button
    startPress = false;
    
    // Turn off the fan
    digitalWrite(fan_control_pin, LOW);
  }
}

void showCurrentHI(){
  // Read data and store it to variables
  hum = dht.readHumidity(); // Get humidity value
  temp = dht.readTemperature(); // Get temperature value in Celsius
  fah = dht.readTemperature(true); // Get temperature value in Fahrenheit
  hif = dht.computeHeatIndex(fah, hum); // Get heat index in Fahrenheit

  // Print current Heat Index to the LCD
  lcd.setCursor(0,0);
  lcd.print("Curr HI: ");
  lcd.print(hif, 1);
  lcd.print((char)223);
  lcd.print("F");
}

void turnOnFan(){
  // The maximum HI for the fan
  hiMax = buttonPushCounter;
  // The minimum HI for the fan, which is 40 interval with the maximum HI
  hiMin = hiMax - 40;

  // Make fan move in certain conditions
  if (hif > hiMax){
    // turn on the fan with the maximum fan speed
    digitalWrite(fan_control_pin, HIGH);
  } 
  else if (hif >= hiMin){
    // fan speed based on current HI
    fanSpeed = map(hif, hiMin, hiMax, 155, 255);
    Serial.println("Current Fan Speed: ");
    Serial.println(fanSpeed);
    analogWrite(fan_control_pin, fanSpeed);
  }
  else{
    // turn off the fan
    digitalWrite(fan_control_pin, LOW);
  }
}

void checkUp(){
  // check button state
  up_buttonState = digitalRead(Up_buttonPin);

  // compare the buttonState to its previous state
  if (up_buttonState != up_lastButtonState) {
    // if the state has changed, increment the counter
    if (up_buttonState == LOW) {
      countPress = true;
      // if the current state is LOW then the button went from off to on:

      // increase the input value
      buttonPushCounter++;
      
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);
    } else {
      // if the current state is HIGH then the button went from on to off:
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }

  // save the current state as the last state, for next time through the loop
  up_lastButtonState = up_buttonState;
}

void checkDown(){
  // check button state
  down_buttonState = digitalRead(Down_buttonPin);

  // compare the buttonState to its previous state
  if (down_buttonState != down_lastButtonState) {
    // if the state has changed, decrement the counter
    if (down_buttonState == LOW) {
      countPress = true;
      // if the current state is LOW then the button went from off to on:

      // decrease the input value
      buttonPushCounter--;   

      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);
    } else {
      // if the current state is HIGH then the button went from on to off:
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  down_lastButtonState = down_buttonState;
}

void checkReset(){
  // check button state
  reset_buttonState = digitalRead(Reset_buttonPin);
 
  // compare the buttonState to its previous state
  if (reset_buttonState != reset_lastButtonState) {
    // if the state has changed, reset user input value
    if (reset_buttonState == LOW) {
      resetPress = true;
      // if the current state is LOW then the button went from off to on:

      // reset the input value to be default
      buttonPushCounter = 100;   

      Serial.println("on");
      Serial.print("reset user input value: ");
      Serial.println(buttonPushCounter);
    } else {
      // if the current state is HIGH then the button went from on to off:
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  reset_lastButtonState = reset_buttonState;
}

void checkStart(){
  // check button state
  start_buttonState = digitalRead(Start_buttonPin);
  
  // compare the buttonState to its previous state
  if (start_buttonState != start_lastButtonState) {
    // if the state has changed, start the fan
    if (start_buttonState == LOW) {
      startPress = true;
      // if the current state is LOW then the button went from off to on:
      Serial.println("on");
      Serial.print("start the fan ");
    } else {
      // if the current state is HIGH then the button went from on to off:
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  start_lastButtonState = start_buttonState;
}
