
/*
 Example sketch for the RFCOMM/SPP Bluetooth library - developed by Kristian Lauszus
 For more information visit my blog: http://blog.tkjelectronics.dk/ or
 send me an e-mail:  kristianl@tkjelectronics.com
 */

#include <SPP.h>
#include <usbhub.h>
// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif

#define ENABLE_UHS_DEBUGGING 1

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside

BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
/* You can create the instance of the class in two ways */
SPP SerialBT(&Btd); // This will set the name to the defaults: "Arduino" and the pin to "0000"
//SPP SerialBT(&Btd, "Lauszus's Arduino", "1234"); // You can also set the name and pin like so

// Declare Constants
boolean firstMessage = true;
const int ledPin = 6; // Built-in LED
const int sensorPin = 2; // PIR Sensor is attached to digital pin 2
//const int ledPin = 13; // Built-in LED
const int ledBlinkTime = 500; // Blink one for half a second while calibrating
// Wait for the seonsor to calibrate (20 - 60 seconds according to datasheet)
// 60 Seconds in milliseconds
const unsigned int calibrationTime = 60000;

int motion_count = 0;
int highflag = 0;

void setup() {

  pinMode(ledPin, OUTPUT);
  pinMode(sensorPin, INPUT);

  Serial.begin(115200);
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nSPP Bluetooth Library Started"));

  // We need to wait one minute for the sensor to calibrate
  // Get out of view of the sensor for this duration!
  // Blink the LED while calibrating
  for (unsigned int i = 0; i < calibrationTime; i += ledBlinkTime * 2) {
    digitalWrite(ledPin, HIGH);
    delay(ledBlinkTime);
    digitalWrite(ledPin, LOW);
    delay(ledBlinkTime);
  }
}

void loop() {

  Usb.Task(); // The SPP data is actually not send until this is called, one could call SerialBT.send() directly as well

  // Constantly check the state of pin 2
  // If it is HIGH the sensor is detecting motion
  if (digitalRead(sensorPin) == HIGH) {
    if (!highflag) {
      motion_count++;
      highflag = 1;
    }
  } else
    highflag = 0;

  if (SerialBT.connected) {

    if (firstMessage) {
      firstMessage = false;
      SerialBT.println(F("Hello from Arduino")); // Send welcome message
    }

    if (SerialBT.available()) {

      char c = SerialBT.read();

      if (c == 'a') {
        SerialBT.println(motion_count, DEC);
      } else if (c == 'b') {
        motion_count = 0;
        SerialBT.println(motion_count, DEC);
      } else
        SerialBT.println("command error");

    }
  }
  else
    firstMessage = true;
}
