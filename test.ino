#include <SoftwareSerial.h>

// robot-related pins
#define ENB_R 5
#define ENA_R 10
#define IN1_3_R 7
#define IN2_4_R 3
#define ENB_L 6
#define ENA_L 11
#define IN1_3_L 8
#define IN2_4_L 4

// Sensor-related pins
/*
  1: Front ultrasonic sensor
  2: Left ultrasonic sensor
  3: Right ultrasonic sensor
*/
#define trigPin_1 28
#define echoPin_1 30
#define trigPin_2 22
#define echoPin_2 24
#define trigPin_3 46
#define echoPin_3 44

// Movement states
#define FORWARD 1
#define BACKWARD 2
#define LEFT 3
#define RIGHT 4
#define STOP 5
#define VALIDRANGE 500

// lights
#define OE 31
#define S0 33
#define S1 35
#define S2 37
#define S3 39
#define OUT 41

// Collision sensors
#define OUT_1 12
#define OUT_2 13

// Movement-related parameters
#define TOLERANCEFORWARD 15  // Allowed closest distance to a wall
#define velocity 80
#define velocityRight 80

int status = 0; // Movement status
String info = "";

// Bluetooth related
#define RX_pin 0
#define TX_pin 1
SoftwareSerial BTSerial(RX_pin, TX_pin);

void setup() {
  // Setup code to run once:

  // Robot
  pinMode(ENB_R, OUTPUT);
  pinMode(ENA_R, OUTPUT);
  pinMode(IN1_3_R, OUTPUT);
  pinMode(IN2_4_R, OUTPUT);
  pinMode(ENB_L, OUTPUT);
  pinMode(ENA_L, OUTPUT);
  pinMode(IN1_3_L, OUTPUT);
  pinMode(IN2_4_L, OUTPUT);

  // Ultrasonic sensors
  pinMode(trigPin_1, OUTPUT);
  pinMode(echoPin_1, INPUT);
  pinMode(trigPin_2, OUTPUT);
  pinMode(echoPin_2, INPUT);
  pinMode(trigPin_3, OUTPUT);
  pinMode(echoPin_3, INPUT);

  //  lights
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  // Collision sensors
  pinMode(OUT_1, INPUT);

  Serial.begin(9600);
  BTSerial.begin(9600);
}

void loop() {
  // Main code to run repeatedly:

  // Read distances from ultrasonic sensors
  int distanceForward = refreshDevice(trigPin_1, echoPin_1);
  int distanceLeft = refreshDevice(trigPin_2, echoPin_2);
  int distanceRight = refreshDevice(trigPin_3, echoPin_3);
  int collisionState_1 = digitalRead(OUT_1);
  int collisionState_2 = digitalRead(OUT_2);

  move(FORWARD);

  if (collisionState_1 == LOW || collisionState_2 == LOW) {
    // If collision sensors detect collision, stop the car, report distances, and reverse
    move(STOP);
    send(distanceForward, distanceLeft, distanceRight);
    delay(1000);
    move(BACKWARD);
    send(distanceForward, distanceLeft, distanceRight);
    delay(100);
    move(STOP);
    send(distanceForward, distanceLeft, distanceRight);
    delay(1000);
  }

  if (distanceForward < TOLERANCEFORWARD) {
    // If the robot is too close to an obstacle in front, reverse, evaluate distances, and turn accordingly
    move(BACKWARD);
    send(distanceForward, distanceLeft, distanceRight);
    delay(10);
    distanceLeft = refreshDevice(trigPin_2, echoPin_2);
    distanceRight = refreshDevice(trigPin_3, echoPin_3);
    distanceForward = refreshDevice(trigPin_1, echoPin_1);
    if (distanceLeft > distanceRight) {
      move(LEFT);
      send(distanceForward, distanceLeft, distanceRight);
      delay(700);
    } else {
      move(RIGHT);
      send(distanceForward, distanceLeft, distanceRight);
      delay(1000);
    }
  }
}

void move(int state) {
  // Function to control the robot's movement
  switch (state) {
    case FORWARD:
      // Move the robot forward

      // Right wheel forward
      analogWrite(IN1_3_R, velocity);
      analogWrite(IN2_4_R, 0);
      analogWrite(ENA_R, 200);
      analogWrite(ENB_R, 200);

      // Left wheel forward
      analogWrite(IN1_3_L, velocity);
      analogWrite(IN2_4_L, 0);
      analogWrite(ENA_R, 200);
      analogWrite(ENB_R, 200);

      status = FORWARD;
      break;

    case BACKWARD:
      // Move the robot backward

      analogWrite(IN1_3_R, 0);
      analogWrite(IN2_4_R, velocity);
      analogWrite(ENA_R, 200);
      analogWrite(ENB_R, 200);

      // Left wheel backward
      analogWrite(IN1_3_L, 0);
      analogWrite(IN2_4_L, velocity);
      analogWrite(ENA_L, 200);
      analogWrite(ENB_L, 200);

      status = BACKWARD;
      break;

    case LEFT:
      // Turn the robot left

      // Right wheel forward
      analogWrite(IN1_3_R, 100);
      digitalWrite(IN2_4_R, LOW);

      // Left wheel backward
      digitalWrite(IN1_3_L, LOW);
      analogWrite(IN2_4_L, 100);

      status = LEFT;
      break;

    case RIGHT:
      // Turn the robot right

      // Right wheel backward
      digitalWrite(IN1_3_R, LOW);
      analogWrite(IN2_4_R, velocityRight);

      // Left wheel forward
      analogWrite(IN1_3_L, velocityRight);
      digitalWrite(IN2_4_L, LOW);

      status = RIGHT;
      break;

    case STOP:
      // Stop both wheels

      // Right wheel
      digitalWrite(IN1_3_R, LOW);
      digitalWrite(IN2_4_R, LOW);
      analogWrite(ENA_R, 200);
      analogWrite(ENB_R, 200);

      // Left wheel
      digitalWrite(IN1_3_L, LOW);
      digitalWrite(IN2_4_L, LOW);
      analogWrite(ENA_L, 200);
      analogWrite(ENB_L, 200);

      status = STOP;
      break;
  }
}

int refreshDevice(int trigPin, int echoPin) {
  // Function to measure distance using ultrasonic sensors
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  return distance;
}

void send(int distanceForward, int distanceLeft, int distanceRight) {
  // Send movement and distance information
  info = "";
  info += status;
  info += " ";
  info += distanceForward;
  info += " ";
  info += distanceLeft;
  info += " ";
  info += distanceRight;
  Serial.println(info);
  BTSerial.write(info.c_str());
}
