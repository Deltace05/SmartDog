#include <L298N.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <Servo.h>
#include <IRremote.h>
#define IR_INPUT_PIN 2
IRrecv irrecv(IR_INPUT_PIN);
decode_results results;

Servo myservo;
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
//Potentiometer
#define pot A3
// Piezo Buzzer
#define piezoPin 8
// Sonar - HC-SR04
#define echoPin A4
#define trigPin 7
// Line Sensor
#define lineSensorPin 3
// Crash Sensor / Button
#define crashSensor 4

boolean dogPowerStatus;
int potVolume;

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
  pinMode(lineSensorPin, INPUT);
  // Crash Sensor / Button
  pinMode(crashSensor, INPUT);
  irrecv.enableIRIn();
}

void loop() {
  sonarSystem(); //Sonar, LED Red Yellow, Piezo, DC Motor. (warning, using this with remote decode causes problems)
  lineSensSystem(); //Line Sensor, LED Green.
  powerButtonSystem(); //Button or Crash Sensor
  potVolumeSystem(); //potentiometer for volume of piezo
  remoteDecode(); //IR remote, servo.
  delay(100);
}

/*
   When someone enters the first threshold (50), it will buzz once with the piezo and wag it's tail for a second (DC Motor
   When someone enters second threshold (35), it will buzz twice with the piezo and tail wag constant (DC Motor)
   When someone enters third threshold(25), it will constantly buzz the piezo.
   @param
   @return
*/
void sonarSystem() {
  long duration;
  int distance;
  int distanceThresOne = 50;
  int distanceThresTwo = 25;
  int distanceThresThree = 15;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  if (dogPowerStatus == 1) {
    if (distance <= distanceThresThree) {
      tone(piezoPin, potVolume + 1500);
      digitalWrite(ledRed, HIGH);
      digitalWrite(ledYellow, LOW);
      delay (15);
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, HIGH);
      motor.forward();
      logEvent("Sonar Triggered");
    } else {
      if (distance <= distanceThresTwo) {
        tone(piezoPin, potVolume + 1000);
        digitalWrite(ledYellow, LOW);
        motor.backward();
      } else {
        if (distance <= distanceThresOne) {
          tone(piezoPin, potVolume + 500);
          motor.stop();
        } else {
          noTone(piezoPin);
          digitalWrite(ledRed, LOW);
          digitalWrite(ledYellow, LOW);
          motor.stop();
        }
      }
    }
  }
}

/*
  It will use line sensor
  if it is blocked it will turn on green light
   @param
   @return
*/
void lineSensSystem() {
  int lineSensorValue = digitalRead(lineSensorPin);
  //Serial.println(lineSensorValue);
  if (dogPowerStatus == 1) {
    if (lineSensorValue == 0) {
      digitalWrite(ledGreen, HIGH);
      logEvent("Line Sensor Triggered");
    }
    if (lineSensorValue == 1) {
      digitalWrite(ledGreen, LOW);
    }
  }
}

/*
  A button can be used to disable all
  functionality/turn off the dog, and can then turn it back on.
   @param
   @return
*/
void powerButtonSystem() {
  dogPowerStatus = digitalRead(crashSensor);
  //Serial.println(dogPowerStatus);
}
/*
  A potentiometer will be able to control
  the volume of the buzzer output
   @param
   @return
*/

void potVolumeSystem() {
  potVolume = analogRead(pot);            // reads the value of the potentiometer (value between 0 and 1023)
  //Serial.println(potValue);
}
/*
  An infrared remote can be used to override the security protocol
  and allow access to its area of guarding. The infrared controller can
  also command the robot dog to rotate its head with a servo motor.
   Gets the value given by the Keyes IR remote.
   Code values are:
   Up     : 25245
   Left   : 8925
   Right  : -15811
   Ok     : 765
   @params: None
   @return: void
*/
void remoteDecode() {

  if (irrecv.decode(&results)) {
    int code = results.value;
    //Serial.println(code);
    if (code == 765) {
      Serial.println("Ok");
      dogPowerStatus == 1;
      delay(10000);
      logEvent("Power Button remote");
    } else {
      dogPowerStatus == 0;
    }
    if (code == 8925) {
      Serial.println("Left");
      myservo.write(0);
      logEvent("Servo Left");
    }
    if (code == -15811) {
      Serial.println("Right");
      myservo.write(180);
      logEvent("Servo Right");
    }
    if (code == 25245) {
      Serial.println("Up");
      myservo.write(90);
      logEvent("Servo centered");
    }
    irrecv.resume();
  }
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
