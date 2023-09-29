#include <SoftwareSerial.h>

//小车相关引脚
#define ENB_R 5   
#define ENA_R 10
#define IN1_3_R 7   
#define IN2_4_R 3   
#define ENB_L 6   
#define ENA_L 11
#define IN1_3_L 8  
#define IN2_4_L 4
//传感器相关引脚
//TODO
/*
  1: 车头超声波
  2: 左车身超声波
  3: 右车身超声波
*/
#define trigPin_1 28
#define echoPin_1 30
#define trigPin_2 22
#define echoPin_2 24
#define trigPin_3 46
#define echoPin_3 44
//移动状态
#define FORWARD 1
#define BACKWARD 2
#define LEFT 3
#define RIGHT 4
#define STOP 5
#define VALIDRANGE 500
// 车灯相关
#define OE 31
#define S0 33
#define S1 35
#define S2 37
#define S3 39
#define OUT 41
//碰撞
#define OUT_1 12
#define OUT_2 13
//运动相关参数
#define TOLERANCE 5 // 可允许靠墙最近距离
#define TOLERANCEFORWARD 15
#define velocity 80 // 
#define velocityLeftRight 80
int status = 0; // 运动状态
String info = "";
//蓝牙相关
#define RX_pin 0
#define TX_pin 1
SoftwareSerial BTSerial(RX_pin, TX_pin);
void setup() {
  // put your setup code here, to run once:
  //车体
  pinMode(ENB_R, OUTPUT);
  pinMode(ENA_R, OUTPUT);
  pinMode(IN1_3_R, OUTPUT);
  pinMode(IN2_4_R, OUTPUT);
  pinMode(ENB_L, OUTPUT);
  pinMode(ENA_L, OUTPUT);
  pinMode(IN1_3_L, OUTPUT);
  pinMode(IN2_4_L, OUTPUT);
  //超声波
  pinMode(trigPin_1, OUTPUT);
  pinMode(echoPin_1, INPUT);
  pinMode(trigPin_2, OUTPUT);
  pinMode(echoPin_2, INPUT);
  pinMode(trigPin_3, OUTPUT);
  pinMode(echoPin_3, INPUT);
  //车灯
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
  //碰撞
  pinMode(OUT_1, INPUT);
  //pinMode(OUT_2, INPUT);
  Serial.begin(9600);
  BTSerial.begin(9600);
  //Mode Pin needs to be added
}

void loop() {
  // put your main code here, to run repeatedly:
  int distanceForward = refreshDevice(trigPin_1, echoPin_1);
  int distanceLeft = refreshDevice(trigPin_2, echoPin_2);
  int distanceRight = refreshDevice(trigPin_3, echoPin_3);
  int collisionState_1 = digitalRead(OUT_1);
  int collisionState_2 = digitalRead(OUT_2);
  move(FORWARD);
  if (collisionState_1 == LOW || collisionState_2 == LOW) {
    //Serial.println("Collision Detected!");  // 如果碰撞传感器检测到碰撞，向串口打印消息
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
  switch (state) {
    case FORWARD:
      //右轮正转
      analogWrite(IN1_3_R, velocity);
      analogWrite(IN2_4_R, 0);
      analogWrite(ENA_R, 200);
      analogWrite(ENB_R, 200);
      //左轮正转
      analogWrite(IN1_3_L, velocity);
      analogWrite(IN2_4_L, 0);
      analogWrite(ENA_R, 200);
      analogWrite(ENB_R, 200);
      status = FORWARD;
      break;
    case BACKWARD:
      analogWrite(IN1_3_R, 0);
      analogWrite(IN2_4_R, velocity);
      analogWrite(ENA_R, 200);
      analogWrite(ENB_R, 200);
      //左轮反转
      analogWrite(IN1_3_L, 0);
      analogWrite(IN2_4_L, velocity);
      analogWrite(ENA_L, 200);
      analogWrite(ENB_L, 200);
      status = BACKWARD;
      break;
    case LEFT:
      //右轮正转
      analogWrite(IN1_3_R, 100);
      digitalWrite(IN2_4_R, LOW);
      //左轮反转
      digitalWrite(IN1_3_L, LOW);
      analogWrite(IN2_4_L, 100);
      status = LEFT;
      break;
    case RIGHT:
      //右轮反转
      digitalWrite(IN1_3_R, LOW);
      analogWrite(IN2_4_R, velocityLeftRight);
      //左轮正转
      analogWrite(IN1_3_L, velocityLeftRight);
      digitalWrite(IN2_4_L, LOW);
      status = RIGHT;
      break;
    case STOP:
      //两个轮都不转
      digitalWrite(IN1_3_R, LOW);
      digitalWrite(IN2_4_R, LOW);
      analogWrite(ENA_R, 200);
      analogWrite(ENB_R, 200);
      digitalWrite(IN1_3_L, LOW);
      digitalWrite(IN2_4_L, LOW);
      analogWrite(ENA_L, 200);
      analogWrite(ENB_L, 200);
      status = STOP;
      break;
  }
}
int refreshDevice(int trigPin, int echoPin) {
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