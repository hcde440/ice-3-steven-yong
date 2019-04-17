/*ICE #3.1
 * 
   DHT22 temperature and humidity sensor demo.
    
   brc 2018
*/

// Adafruit IO Temperature & Humidity Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-temperature-and-humidity
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016-2017 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

//https://io.adafruit.com/steventy/dashboards/hcde-440

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"                                                             //Include the the config.h to access private information.

/************************ Example Starts Here *******************************/
#include <Adafruit_Sensor.h>                                                    //Include the Adafruit Sensor library.
#include <DHT.h>                                                                //Include the DHT library.
#include <DHT_U.h>                                                              //Include the DHT library.
#define DATA_PIN 12                                                             //Assign the name "DATA_PIN" to pin 12.
DHT_Unified dht(DATA_PIN, DHT22);                                               //Create an instance of DHT22, assign it to DATA_PIN, and give it the name "dht."

#include <Wire.h>                                                               //Include the wire library for I2C.
#include <Adafruit_MPL115A2.h>                                                  //Include the library for the barometer.
Adafruit_MPL115A2 mpl115a2;                                                     //Create an instance of MPL115A2 and give it the name "mpl115a2."

#include <SPI.h>                                                                //Include the SPI library for I2C.
#include <Wire.h>                                                               //Include the wire library for I2C.
#include <Adafruit_GFX.h>                                                       //Include the Adafruit graphics library.
#include <Adafruit_SSD1306.h>                                                   //Include the library for the OLED screen.
#define SCREEN_WIDTH 128                                                        //Set the OLED display width to 128 pixels.
#define SCREEN_HEIGHT 32                                                        //Set the OLED display height to 32 pixels.
#define OLED_RESET 13                                                           //Set the OLED reset pin to 13.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);       //Initiate the OLED display with the following parameters.

AdafruitIO_Feed *DHT22_temp_C = io.feed("DHT22_temp_C");                        //Connect the DHT22_temp_C feed.
AdafruitIO_Feed *DHT22_temp_F = io.feed("DHT22_temp_F");                        //Connect the DHT22_temp_F feed.
AdafruitIO_Feed *humidity = io.feed("humidity");                                //Connect the humidity feed.
AdafruitIO_Feed *pressure = io.feed("pressure");                                //Connect the pressure feed.
AdafruitIO_Feed *MPL_temp = io.feed("MPL_temp");                                //Connect the MPL_temp feed.

void setup() {

  Serial.begin(115200);                                                         //Initialize the serial monitor at a rate of 115200 baud.
  while(! Serial);                                                              //Keep looping until the serial monitor responds.

  dht.begin();                                                                  //Initialize DHT22.

  Serial.print("Connecting to Adafruit IO");                                    //Print this in the serial monitor.
  io.connect();                                                                 //Connect to Adafruit IO.

  while(io.status() < AIO_CONNECTED) {                                          //Keep looping the following if Adafruit IO does not connect.
    Serial.print(".");                                                          //Print this in the serial monitor.
    delay(500);                                                                 //Delay by 500 milliseconds.
  }

  Serial.println();                                                             //Print this in the serial monitor.
  Serial.println(io.statusText());                                              //Print the status of the Adafruit IO connection.

  Serial.println("Getting barometric pressure ...");                            //Print this in the serial monitor.
  mpl115a2.begin();                                                             //Initialize MPL115A2.

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {                              //If the display does not respond after generating display voltage from 3.3V internally and addressing 0x3C for 128x32 pixels, execute the following.
    Serial.println(F("SSD1306 allocation failed"));                             //Print this in the serial monitor.
    for(;;);                                                                    //Loop forever.
  }

  display.display();                                                            //Display the Adafruit logo.
  delay(2000);                                                                  //Delay by 2000 milliseconds.
  display.clearDisplay();                                                       //Clear the display.
  display.drawPixel(10, 10, WHITE);                                             //Draw a single white pixel at (10,10).
  display.display();                                                            //Display the white pixel at (10,10).
  
}

void loop() {

  io.run();                                                                     //Maintain connection to Adafruit IO.

  sensors_event_t event;                                                        //Create an instance of sensors_event_t named "event."
  dht.temperature().getEvent(&event);                                           //Retrieve the temperature from the DHT22 sensor.

  float celsius = event.temperature;                                            //Create a float variable named celsius and pass it the reading from the DHT22 sensor.
  float fahrenheit = (celsius * 1.8) + 32;                                      //Create a float variable named fahrenheit and calculate it using the reading from the DHT22 sensor.

  Serial.print("celsius: ");                                                    //Print this in the serial monitor.
  Serial.print(celsius);                                                        //Print celsius in the serial monitor.
  Serial.println("C");                                                          //Print this in the serial monitor.

  Serial.print("fahrenheit: ");                                                 //Print this in the serial monitor.
  Serial.print(fahrenheit);                                                     //Print fahrenheit in the serial monitor.
  Serial.println("F");                                                          //Print this in the serial monitor.

  DHT22_temp_C->save(celsius);                                                  //Save celsius into the DHT22_temp_C feed.
  DHT22_temp_F->save(fahrenheit);                                               //Save fahrenheit into the DHT22_temp_F feed.

  dht.humidity().getEvent(&event);                                              //Retrieve the humidity from the DHT22 sensor.

  Serial.print("humidity: ");                                                   //Print this in the serial monitor.
  Serial.print(event.relative_humidity);                                        //Print the humidity in the serial monitor.
  Serial.println("%");                                                          //Print this in the serial monitor.
  
  humidity->save(event.relative_humidity);                                      //Save humidity into the humidity feed.

  float pressureKPA = 0, temperatureC = 0;                                      //Create two float variables named pressurePA and temperatureC.

  pressureKPA = mpl115a2.getPressure();                                         //Pass the pressure reading from the MPL115A2 sensor to pressureKPA.
  Serial.print("Pressure (kPa): ");                                             //Print this in the serial monitor.
  Serial.print(pressureKPA, 4);                                                 //Print pressureKPA with 4 decimal places.
  Serial.println(" kPa");                                                       //Print this in the serial monitor.
  pressure->save(pressureKPA);                                                  //Save pressureKPA into the pressure feed.

  temperatureC = mpl115a2.getTemperature();                                     //Pass the temperature reading from the MPL115A2 sensor to temperatureC.
  Serial.print("Temp (*C): ");                                                  //Print this in the serial monitor.
  Serial.print(temperatureC, 1);                                                //Print temperatureC with 1 decimal place.
  Serial.println(" *C");                                                        //Print this in the serial monitor.
  MPL_temp->save(temperatureC);                                                 //Save temperatureC into the MPL_temp feed.

  display.clearDisplay();                                                       //Clear the display.
  display.setTextSize(1);                                                       //Set the scale to 1.
  display.setTextColor(WHITE);                                                  //Set the color to white.
  display.setCursor(0,0);                                                       //Set the cursor at (0,0).
  
  display.print(celsius);                                                       //Print celsius to the display.
  display.print(F("C   "));                                                     //Print this to the display.
  display.print(fahrenheit);                                                    //Print fahrenheit to the display.
  display.println(F("F"));                                                      //Print this to the display.
  display.print(event.relative_humidity);                                       //Print event.relative_humidity to the display.
  display.println(F("% Humidity"));                                             //Print this to the display.
  display.print(pressureKPA, 4);                                                //Print pressureKPA to the display with 4 decimal places.
  display.println(F("kPA"));                                                    //Print this to the display.
  display.print(temperatureC, 1);                                               //Print temperatureC to the display with 1 decimal place.
  display.println(F("*C"));                                                     //Print this to the display.
  
  display.display();                                                            //Show the display.
  delay(60000);                                                                 //Delay by 60000 milliseconds.

}
