```mermaid
flowchart TD
terminalStart([Start])
sonar(do sonar function)
lineSensor(do lineSensor function)
button(do button function)
potentiometer(do potentiometer function)
infraredControl(do infrared function)
SDCard(do sdCard function)

terminalStart --> sonar
sonar --> lineSensor
lineSensor --> button
button --> potentiometer
potentiometer --> infraredControl
infraredControl --> SDCard
SDCard --> sonar
```


When someone has entered the first range (50 centimeters) it will sound 1 buzz from the piezo and slow its passive wag by half (DC Motor). When someone enters the second range (35 cm) it will buzz twice and stop tail wag (DC Motor). If someone enters trigger range (25 cm) it will begin a constant buzzing.
```mermaid
flowchart TD
terminalStart([Start])
terminalEnd([End])
setSonarRead(sonarPins = pins)
setPiezoPin(piezoPin = pin)
setTail(tailMotorPins = pins)
setRedYellowLight(trafficLightRedYellowPins = pins)
thresholdSetOne(distanceThresholdOne = 50)
thresholdSetTwo(distanceThresholdTwo = 35)
thresholdSetThree(distanceThresholdThree = 25)
currentDistanceReading(distanceRead = response from sonar)
ifDistanceLessThanThresholdOne{distanceRead < distanceThresholdOne}
ifDistanceLessThanThresholdTwo{distanceRead < distanceThresholdTwo}
ifDistanceLessThanThresholdThree{distanceRead < distanceThresholdThree}
piezoOneBuzz(write piezoPin HIGH, delay 1000 write piezoPin LOW)
piezoTwoBuzz(write piezoPin HIGH, delay 1000 write piezoPin LOW, write piezoPin HIGH, delay 1000 write piezoPin LOW)
piezoConstant(write piezoPin HIGH)
sonarLightAlarm(trafficLightPins Red & Yellow HIGH)
sonarLightAlarmReset(trafficLightPins Red & Yellow LOW)
piezoReset(write piezoPin LOW)
tailSlow(tailWag = slow by 50%)
tailStop(tailWag = halt)


terminalStart --> setSonarRead
setSonarRead --> setPiezoPin
setPiezoPin --> setTail
setTail --> setRedYellowLight
setRedYellowLight --> thresholdSetOne
thresholdSetOne --> thresholdSetTwo
thresholdSetTwo --> thresholdSetThree
thresholdSetThree --> piezoReset
piezoReset --> sonarLightAlarmReset
sonarLightAlarmReset --> currentDistanceReading
currentDistanceReading --> ifDistanceLessThanThresholdOne
ifDistanceLessThanThresholdOne --> |True| piezoOneBuzz
piezoOneBuzz --> tailSlow
tailSlow --> terminalEnd
ifDistanceLessThanThresholdOne --> |False| ifDistanceLessThanThresholdTwo
ifDistanceLessThanThresholdTwo --> |True| piezoTwoBuzz
piezoTwoBuzz --> tailStop
tailStop --> terminalEnd
ifDistanceLessThanThresholdTwo --> |False| ifDistanceLessThanThresholdThree
ifDistanceLessThanThresholdThree --> |True| piezoConstant
piezoConstant --> sonarLightAlarm
sonarLightAlarm --> terminalEnd
ifDistanceLessThanThresholdThree --> |False| terminalEnd
```


It will follow a line sensor path, if it is blocked it will turn on all lights and try the path again after a few seconds.
```mermaid
flowchart TD
terminalStart([Start])
terminalEnd([End])
declareLineSensor(set lineSensor pins)
setGreenLight(set green traffic light pin)
lineSensorData(onLine = response from sensor)
resetLight(write greenLight LOW)
ifOnLine{is onLine?}
isOnLine(write greenLight HIGH)
isNotOnLine(write greenLight HIGH delay 100 write greenLight LOW)

terminalStart --> declareLineSensor
declareLineSensor --> setGreenLight
setGreenLight --> lineSensorData
lineSensorData --> resetLight
resetLight --> ifOnLine
ifOnLine --> |True| isOnLine
isOnLine --> terminalEnd
ifOnLine --> |False| isNotOnLine
isNotOnLine --> terminalEnd
```

A button can be used to disable all functionality/turn off the dog, and can then turn it back on.
```mermaid
flowchart TD
terminalStart([Start])
terminalEnd([End])
setButton(set buttonPins)
setBoolean(set buttonBoolean)
buttonData(buttonBoolean true or false flip via button)
ifBooleanTrueFalse{isButtonBoolean true or false}
ifBooleanTrue(allow function of other systems)
ifBooleanFalse(do not allow function of other systems)

terminalStart --> setButton
setButton --> setBoolean
setBoolean --> buttonData
buttonData --> ifBooleanTrueFalse
ifBooleanTrueFalse --> |True| ifBooleanTrue
ifBooleanTrue --> terminalEnd
ifBooleanTrueFalse --> |False| ifBooleanFalse
ifBooleanFalse --> terminalEnd
```

A potentiometer will be able to control the volume of its buzzer output.
```mermaid
flowchart TD
terminalStart([Start])
terminalEnd([End])
setVolumePot(set potentiometerPins)
piezoInfo(piezoPins already set)
readVolumePot(volumeData = data from volumePot)
convertOutput(convert volumeData to voltageNum)
outputVolt(output voltageNum to piezo)

terminalStart --> setVolumePot
setVolumePot --> piezoInfo
piezoInfo --> readVolumePot
readVolumePot --> convertOutput
convertOutput --> outputVolt
outputVolt --> terminalEnd
```

An infrared remote can be used to override the security protocol and allow access to its area of guarding.
The infrared controller can also command the robot dog to rotate its head with a servo motor.
```mermaid
flowchart TD
terminalStart([Start])
terminalEnd([End])
setDogRemote(set infraredPins)
readInfrared(readControllerOutput)
whatButtonPressed{which of the three was pressed}
leftButton(turnHead left 10degrees with servo)
rightButton(turnHead right 10degrees with servo)
middleButton(change buttonBoolean)

terminalStart --> setDogRemote
setDogRemote --> readInfrared
readInfrared --> whatButtonPressed
whatButtonPressed --> |Left| leftButton
whatButtonPressed --> |Right| rightButton
whatButtonPressed --> |Middle| middleButton
leftButton --> terminalEnd
rightButton --> terminalEnd
middleButton --> terminalEnd
```

Whenever the dog has any triggers from sonar, line or infrared it will save its current data (sensor info) to an sd card log (what range target is at, if line is broken or if infrared has been activated). 
```mermaid
flowchart TD
terminalStart([Start])
terminalEnd([End])
setSDPins(set SDCardPins)
checkTrig(check currentTriggerStatus)
ifTrigger{is triggered?}
trigTrue(write current sensorStatus to card)

terminalStart --> setSDPins
setSDPins --> checkTrig
checkTrig --> ifTrigger
ifTrigger --> |True| trigTrue
trigTrue --> terminalEnd
ifTrigger --> |False| terminalEnd
```