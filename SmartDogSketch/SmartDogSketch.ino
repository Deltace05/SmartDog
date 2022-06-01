#include <Adafruit_NECremote.h>
#include <L298N.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <Servo.h>


Servo myservo;
RTC_Millis rtc;
DateTime rightNow;
// IR Remote
#define IRpin 7
Adafruit_NECremote remote(IRpin);
// Traffic Lights - LED Outputs
#define ledRed A0
#define ledYellow A1
#define ledGreen A2
// Pin definition
const unsigned int IN1 = 5;
const unsigned int IN2 = 6;
// Create one motor instance
L298N motor (IN1, IN2);
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

boolean dogPowerStatus;

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
  sonarSystem(); //Sonar, LED Red Yellow, Piezo, DC Motor.
  lineSensSystem(); //Line Sensor, LED Green.
  powerButtonSystem(); //Button or Crash Sensor
  
  delay(100);
}
/*
 * 
 * 
 * @param
 * @return
 */
void sonarSystem() {
  long duration;
  int distance;
  int distanceThresOne = 50;
  int distanceThresTwo = 35;
  int distanceThresThree = 25;
  digitalWrite(piezoPin, LOW);
  digitalWrite(ledRed, LOW);
  digitalWrite(ledYellow, LOW);

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

}

/*
 * 
 * 
 * @param
 * @return
 */
void lineSensSystem() {
  
}

/*
 * 
 * 
 * @param
 * @return
 */
void powerButtonSystem() {
  
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
