#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Ultrasonic Sensor Pins
#define FRONT_TRIG 9
#define FRONT_ECHO 8
#define LEFT_TRIG 4
#define LEFT_ECHO 5
#define RIGHT_TRIG 6
#define RIGHT_ECHO 7

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// Timing Control
unsigned long previousMillis = 0;
const unsigned long loopInterval = 150; // 150ms non-blocking delay

// Voice Control
unsigned long lastVoiceTime = 0;
const unsigned long forwardVoiceInterval = 5000; // 5 sec interval for moveforward

// Safety distances (cm)
#define FRONT_SAFE_DIST 50
#define SIDE_SAFE_DIST 50 

void setup() {
  Serial.begin(9600);
  mySoftwareSerial.begin(9600);

  // Initialize Ultrasonic Sensors
  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);
  pinMode(LEFT_TRIG, OUTPUT);
  pinMode(LEFT_ECHO, INPUT);
  pinMode(RIGHT_TRIG, OUTPUT);
  pinMode(RIGHT_ECHO, INPUT);

  // Initialize DFPlayer
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("DFPlayer Mini not connected!");
    while (true);
  }
  myDFPlayer.volume(30); // Volume 0-30
}

void loop() {
  unsigned long currentMillis = millis();

  // Non-blocking 150ms delay
  if (currentMillis - previousMillis >= loopInterval) {
    previousMillis = currentMillis;

    // Read all sensors
    int frontDist = getDistance(FRONT_TRIG, FRONT_ECHO);
    int leftDist = getDistance(LEFT_TRIG, LEFT_ECHO);
    int rightDist = getDistance(RIGHT_TRIG, RIGHT_ECHO);

    Serial.print("Front: "); Serial.print(frontDist);
    Serial.print("cm | Left: "); Serial.print(leftDist);
    Serial.print("cm | Right: "); Serial.print(rightDist);
    Serial.println("cm");

    // Obstacle Detection Logic
    if (frontDist <= FRONT_SAFE_DIST) {
      if (leftDist <= SIDE_SAFE_DIST && rightDist <= SIDE_SAFE_DIST) {
        playVoice(4);  // goback.mp3
      } 
      else if (leftDist <= SIDE_SAFE_DIST) {
        playVoice(3);  // turnright.mp3
      } 
      else if (rightDist <= SIDE_SAFE_DIST) {
        playVoice(1);  // turnleft.mp3
      } 
      else {
        playVoice(2); // chooseleftorright.mp3
      }
    } 
    else if (currentMillis - lastVoiceTime >= forwardVoiceInterval) {
      myDFPlayer.play(5);  // moveforward.mp3
      lastVoiceTime = currentMillis;
    }
  }
}

void playVoice(int track) {
  if (myDFPlayer.readState() == 0) { // Only play if not busy
    myDFPlayer.play(track);
    Serial.print("Playing track: "); Serial.println(track);
  }
}

int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000); // 20ms timeout
  return (duration == 0) ? 999 : duration * 0.034 / 2;
}
ultrafinal.txt
Displaying ultrafinal.txt.
