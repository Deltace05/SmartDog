#include <L298N.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

RTC_Millis rtc;
DateTime rightNow;

// Traffic Lights - LED Outputs
#define ledRed A0
#define ledYellow A1
#define ledGreen A2
// Pin definition
const unsigned int IN1 = 5;
const unsigned int IN2 = 6;
// Create one motor instance
L298N motor (IN1, IN2);
// Servo
#include <Servo.h>
Servo myservo;
//Potentiometer
#define pot A3
// Piezo Buzzer
#define piezoPin 8
// Sonar - HC-SR04
#define echoPin A4
#define trigPin 2
// Line Sensor
#define lineSensorPin 3
// Crash Sensor / Button
#define crashSensor 4

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(1);
  }
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println("initialization done.");
  logEvent("System Initialisation...");

  // Traffic Lights - LED Outputs
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  // DC Motor & Motor Module - L298N
  motor.setSpeed(70);
  // Servo
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  //Potentiometer
  pinMode(pot, INPUT);
  // Piezo Buzzer
  pinMode(piezoPin, OUTPUT);
  // Sonar - HC-SR04
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  // Line Sensor
  pinMode(lineSensorPin, OUTPUT);
  // Crash Sensor / Button
  pinMode(crashSensor, INPUT);

}

void loop() {

}

void logEvent(String dataToLog) {
  /*
     Log entries to a file on an SD card.
  */
  // Get the updated/current time
  DateTime rightNow = rtc.now();

  // Open the log file
  File logFile = SD.open("events.csv", FILE_WRITE);
  if (!logFile) {
    Serial.print("Couldn't create log file");
    abort();
  }

  // Log the event with the date, time and data
  logFile.print(rightNow.year(), DEC);
  logFile.print(",");
  logFile.print(rightNow.month(), DEC);
  logFile.print(",");
  logFile.print(rightNow.day(), DEC);
  logFile.print(",");
  logFile.print(rightNow.hour(), DEC);
  logFile.print(",");
  logFile.print(rightNow.minute(), DEC);
  logFile.print(",");
  logFile.print(rightNow.second(), DEC);
  logFile.print(",");
  logFile.print(dataToLog);

  // End the line with a return character.
  logFile.println();
  logFile.close();
  Serial.print("Event Logged: ");
  Serial.print(rightNow.year(), DEC);
  Serial.print(",");
  Serial.print(rightNow.month(), DEC);
  Serial.print(",");
  Serial.print(rightNow.day(), DEC);
  Serial.print(",");
  Serial.print(rightNow.hour(), DEC);
  Serial.print(",");
  Serial.print(rightNow.minute(), DEC);
  Serial.print(",");
  Serial.print(rightNow.second(), DEC);
  Serial.print(",");
  Serial.println(dataToLog);
}
