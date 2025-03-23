#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Pin Definitions
#define trigPin 9
#define echoPin 8
#define irLeft A0      // Using Analog pins for IR Sensors
#define irRight A1     

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

long duration;
int distance;

// IR Sensor Threshold (For Maximum Range Detection)
int threshold = 200;  // Lower value = More distance detection

// Voice control variables
unsigned long lastVoiceTime = 0;
const unsigned long forwardVoiceInterval = 5000; // 5 sec interval for moveforward

void setup() {
  Serial.begin(9600);
  mySoftwareSerial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(irLeft, INPUT);
  pinMode(irRight, INPUT);

  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("Unable to begin DFPlayer Mini");
    while (true);
  }
  myDFPlayer.volume(30);  // Set maximum volume
}

void loop() {
  distance = getDistance();
  
  // Read analog values of IR sensors
  int leftIR = analogRead(irLeft);
  int rightIR = analogRead(irRight);

  // Detect obstacles at maximum distance (~30cm)
  bool leftObstacle = (leftIR > threshold);
  bool rightObstacle = (rightIR > threshold);

  Serial.print("Distance: "); Serial.println(distance);
  Serial.print("Left IR: "); Serial.print(leftIR); 
  Serial.print(" | Right IR: "); Serial.println(rightIR);

  if (distance <= 50) {  // Front obstacle detected
    if (leftObstacle && rightObstacle) {
      playVoice(4);  // goback.mp3
    } 
    else if (leftObstacle && !rightObstacle) {
      playVoice(3);  // turnright.mp3
    } 
    else if (!leftObstacle && rightObstacle) {
      playVoice(1);  // turnleft.mp3
    } 
    else { // Both left and right free, choose
      playVoice(2); // chooseleftorright.mp3
    }
  } else {
    // Play moveforward.mp3 every 5 sec if no obstacle
    if (millis() - lastVoiceTime > forwardVoiceInterval && myDFPlayer.readState() == 0) {
      myDFPlayer.play(5);  // moveforward.mp3
      lastVoiceTime = millis();
    }
  }

  delay(200);
}

void playVoice(int track) {
  if (myDFPlayer.readState() == 0) {
    myDFPlayer.play(track);
    Serial.print("Playing Track: "); Serial.println(track);
  }
}

long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 20000); // Timeout 20ms
  if (duration == 0) return 999;  // No object detected

  return duration * 0.034 / 2;  // Convert to cm
}
