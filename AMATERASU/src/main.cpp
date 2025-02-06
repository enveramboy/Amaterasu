#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Ps3Controller.h>
#include <Adafruit_PWMServoDriver.h>

// SERVO SETUP
Adafruit_PWMServoDriver board = Adafruit_PWMServoDriver(0x40);
#define SERVO_MIN 125
#define SERVO_MAX 575

int angleToPulse(int ang) { return map(ang, 0, 180, SERVO_MIN, SERVO_MAX); }

#define r_front 0
#define l_front 1
#define l_back 2
#define r_back 3
#define l_shoulder 4
#define l_deltoid 5
#define l_bicep 6
#define r_shoulder 7
#define r_deltoid 8
#define r_bicep 9
#define waist 11

// MOTOR SETUP
#define PWMA1 13
#define IN1A1 27
#define IN2A1 12

#define PWMB1 15
#define IN1B1 26
#define IN2B1 25

#define PWMA2 33
#define IN1A2 18
#define IN2A2 4

#define PWMB2 2
#define IN1B2 5
#define IN2B2 23

// GAIT & ANIMATIONS
volatile int thigh_angles[] = {80, 100, 80, 100}; 
volatile int is_changing_height = 0;
volatile int d_pad_states[] = {0, 0, 0, 0};

void initialize() {
  board.setPWM(r_front, 0, angleToPulse(40));
  board.setPWM(l_front, 0, angleToPulse(140));
  board.setPWM(l_back, 0, angleToPulse(40));
  board.setPWM(r_back, 0, angleToPulse(140));
  board.setPWM(r_shoulder, 0, angleToPulse(90));
  board.setPWM(r_deltoid, 0, angleToPulse(30));
  board.setPWM(r_bicep, 0, angleToPulse(90));
  board.setPWM(l_shoulder, 0, angleToPulse(90));
  board.setPWM(l_deltoid, 0, angleToPulse(150));
  board.setPWM(l_bicep, 0, angleToPulse(90));
  board.setPWM(waist, 0, angleToPulse(90));
}

void implosion_stance() {
  board.setPWM(r_shoulder, 0, angleToPulse(90));
  board.setPWM(r_deltoid, 0, angleToPulse(150));
  board.setPWM(r_bicep, 0, angleToPulse(90));
  board.setPWM(l_shoulder, 0, angleToPulse(90));
  board.setPWM(l_deltoid, 0, angleToPulse(30));
  board.setPWM(l_bicep, 0, angleToPulse(90));
  board.setPWM(waist, 0, angleToPulse(90));
}

void forward() {
  digitalWrite(IN1A1, HIGH); digitalWrite(IN2A1, LOW);
  digitalWrite(IN1B1, LOW); digitalWrite(IN2B1, HIGH);

  digitalWrite(IN1A2, LOW); digitalWrite(IN2A2, HIGH);
  digitalWrite(IN1B2, LOW); digitalWrite(IN2B2, HIGH);
}

void backward() {
  digitalWrite(IN1A1, LOW); digitalWrite(IN2A1, HIGH);
  digitalWrite(IN1B1, HIGH); digitalWrite(IN2B1, LOW);

  digitalWrite(IN1A2, HIGH); digitalWrite(IN2A2, LOW);
  digitalWrite(IN1B2, HIGH); digitalWrite(IN2B2, LOW);
}

void right() {
  digitalWrite(IN1A1, HIGH); digitalWrite(IN2A1, LOW);
  digitalWrite(IN1B1, HIGH); digitalWrite(IN2B1, LOW);

  digitalWrite(IN1A2, HIGH); digitalWrite(IN2A2, LOW);
  digitalWrite(IN1B2, LOW); digitalWrite(IN2B2, HIGH);
}

void left() {
  digitalWrite(IN1A1, LOW); digitalWrite(IN2A1, HIGH);
  digitalWrite(IN1B1, LOW); digitalWrite(IN2B1, HIGH);

  digitalWrite(IN1A2, LOW); digitalWrite(IN2A2, HIGH);
  digitalWrite(IN1B2, HIGH); digitalWrite(IN2B2, LOW);
}

void turn_left() {
  digitalWrite(IN1A1, HIGH); digitalWrite(IN2A1, LOW);
  digitalWrite(IN1B1, LOW); digitalWrite(IN2B1, HIGH);
  digitalWrite(IN1A2, HIGH); digitalWrite(IN2A2, LOW);
  digitalWrite(IN1B2, HIGH); digitalWrite(IN2B2, LOW);
}

void turn_right() {
  digitalWrite(IN1A1, LOW); digitalWrite(IN2A1, HIGH);
  digitalWrite(IN1B1, HIGH); digitalWrite(IN2B1, LOW);
  digitalWrite(IN1A2, LOW); digitalWrite(IN2A2, HIGH);
  digitalWrite(IN1B2, LOW); digitalWrite(IN2B2, HIGH);
}

void halt() {
  digitalWrite(IN1A1, LOW); digitalWrite(IN2A1, LOW);
  digitalWrite(IN1B1, LOW); digitalWrite(IN2B1, LOW);
  digitalWrite(IN1A2, LOW); digitalWrite(IN2A2, LOW);
  digitalWrite(IN1B2, LOW); digitalWrite(IN2B2, LOW);
}

void stand() {
  for (int i = 0; i < 4; i++) board.setPWM(i, 0, angleToPulse(thigh_angles[i]));
}

void rise() {
  digitalWrite(IN1A1, HIGH); digitalWrite(IN2A1, LOW);
  digitalWrite(IN1B1, HIGH); digitalWrite(IN2B1, LOW);

  digitalWrite(IN1A2, LOW); digitalWrite(IN2A2, HIGH);
  digitalWrite(IN1B2, HIGH); digitalWrite(IN2B2, LOW);

  for (int i = 0; i < 4; i++) { (i % 2 == 0) ? ( (thigh_angles[i] < 101) ? thigh_angles[i]++ : 0) : ( (thigh_angles[i] > 79) ? thigh_angles[i]-- : 0 ); }
  stand();
}

void lean_forward() {
  digitalWrite(IN1A1, LOW); digitalWrite(IN2A1, LOW);
  digitalWrite(IN1A2, LOW); digitalWrite(IN2A2, LOW);

  digitalWrite(IN1B1, LOW); digitalWrite(IN2B1, HIGH);
  digitalWrite(IN1B2, LOW); digitalWrite(IN2B2, HIGH);

  for (int i = 0; i < 2; i++) { (i % 2 == 0) ? ( (thigh_angles[i] > 39) ? thigh_angles[i]-- : 0 ) : ( (thigh_angles[i] < 141) ? thigh_angles[i]++ : 0 ); }
  stand();
}

void fall() {
  digitalWrite(IN1A1, LOW); digitalWrite(IN2A1, HIGH);
  digitalWrite(IN1B1, LOW); digitalWrite(IN2B1, HIGH);

  digitalWrite(IN1A2, HIGH); digitalWrite(IN2A2, LOW);
  digitalWrite(IN1B2, LOW); digitalWrite(IN2B2, HIGH);

  for (int i = 0; i < 4; i++) { (i % 2 == 0) ? ( (thigh_angles[i] > 39) ? thigh_angles[i]-- : 0 ) : ( (thigh_angles[i] < 141) ? thigh_angles[i]++ : 0 ); }
  stand();
}

void lean_backward() {
  digitalWrite(IN1A1, LOW); digitalWrite(IN2A1, LOW);
  digitalWrite(IN1A2, LOW); digitalWrite(IN2A2, LOW);

  digitalWrite(IN1B1, HIGH); digitalWrite(IN2B1, LOW);
  digitalWrite(IN1B2, HIGH); digitalWrite(IN2B2, LOW);

  for (int i = 0; i < 2; i++) { (i % 2 == 0) ? ( (thigh_angles[i] < 101) ? thigh_angles[i]++ : 0) : ( (thigh_angles[i] > 79) ? thigh_angles[i]-- : 0 ); } 
  stand();
}

void notify() {
  // Light Attack
  if (Ps3.event.button_down.l1) {
    board.setPWM(l_shoulder, 0, angleToPulse(0));
    board.setPWM(l_deltoid, 0, angleToPulse(120));
    board.setPWM(l_bicep, 0, angleToPulse(120));
  }
  if (Ps3.event.button_up.l1) {
    board.setPWM(l_shoulder, 0, angleToPulse(90));
    board.setPWM(l_deltoid, 0, angleToPulse(30));
    board.setPWM(l_bicep, 0, angleToPulse(90));
  }

  if (Ps3.event.button_down.r1) {
    board.setPWM(r_shoulder, 0, angleToPulse(180));
    board.setPWM(r_deltoid, 0, angleToPulse(60));
    board.setPWM(r_bicep, 0, angleToPulse(60));
  }
  if (Ps3.event.button_up.r1) {
    board.setPWM(r_shoulder, 0, angleToPulse(90));
    board.setPWM(r_deltoid, 0, angleToPulse(150));
    board.setPWM(r_bicep, 0, angleToPulse(90));
  }

  // Heavy Attack
  if (Ps3.event.button_down.l2) {
    board.setPWM(waist, 0, angleToPulse(0));
    board.setPWM(l_shoulder, 0, angleToPulse(90));
    board.setPWM(l_deltoid, 0, angleToPulse(0));
    board.setPWM(l_bicep, 0, angleToPulse(90));
    
  }
  if (Ps3.event.button_up.l2) {
    implosion_stance();
  }

  if (Ps3.event.button_down.r2) {
    board.setPWM(waist, 0, angleToPulse(180));
    board.setPWM(r_shoulder, 0, angleToPulse(90));
    board.setPWM(r_deltoid, 0, angleToPulse(180));
    board.setPWM(r_bicep, 0, angleToPulse(90));
  }
  if (Ps3.event.button_up.r2) {
    implosion_stance();
  }

  // Raise/Lower
  if (Ps3.event.button_down.up || Ps3.event.button_down.right || Ps3.event.button_down.down || Ps3.event.button_down.left) {
    analogWrite(PWMA1, 240);
    analogWrite(PWMB1, 240);
    analogWrite(PWMA2, 240);
    analogWrite(PWMB2, 240);
  }
  if (Ps3.data.button.up) rise();
  if (Ps3.data.button.right) lean_forward();
  if (Ps3.data.button.down) fall();
  if (Ps3.data.button.left) lean_backward();

  int lx = Ps3.data.analog.stick.lx;
  int ly = Ps3.data.analog.stick.ly;
  int rx = Ps3.data.analog.stick.rx;
  int ry = Ps3.data.analog.stick.ry;
  if (abs(lx) > 10 || abs(ly) > 10 || abs(rx) > 10 || abs(ry) > 10) {
    if (abs(ry) + abs(rx) < abs(ly) + abs(lx)) {
      int spd = constrain((abs(ly) > abs(lx)) ? abs(ly) * 2 : abs(lx) * 2, 0, 255);
      analogWrite(PWMA1, spd);
      analogWrite(PWMB1, spd);
      analogWrite(PWMA2, spd);
      analogWrite(PWMB2, spd);

      if (abs(ly) > abs(lx)) {
        if (ly > 0) backward();
        else forward();
      }
      else {
        if (lx < 0) left();
        else right();
      }
    }
    else {
      int spd = constrain((abs(ry) > abs(rx)) ? abs(ry) * 2 : abs(rx) * 2, 0, 255);
      analogWrite(PWMA1, spd);
      analogWrite(PWMB1, spd);
      analogWrite(PWMA2, spd);
      analogWrite(PWMB2, spd);

      if (abs(ry) > abs(rx)) {
        if (ry > 0) backward();
        else forward();
      }
      else {
        if (rx < 0) turn_left();
        else turn_right();
      }
    }
  }
  else halt();
}

void onConnect() { 
  Ps3.setPlayer(3); 
  implosion_stance();
}

void setup() {
  // Servo Initialization
  board.begin();
  board.setPWMFreq(60);

  // Motor Initialization
  pinMode(PWMA1, OUTPUT);
  pinMode(IN1A1, OUTPUT);
  pinMode(IN2A1, OUTPUT);
  pinMode(PWMB1, OUTPUT);
  pinMode(IN1B1, OUTPUT);
  pinMode(IN2B1, OUTPUT);
  pinMode(PWMA2, OUTPUT);
  pinMode(IN1A2, OUTPUT);
  pinMode(IN2A2, OUTPUT);
  pinMode(PWMB2, OUTPUT);
  pinMode(IN1B2, OUTPUT);
  pinMode(IN2B2, OUTPUT);

  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);
  pinMode(19, OUTPUT);
  digitalWrite(19, HIGH);

  analogWrite(PWMA1, 240);
  analogWrite(PWMB1, 240);
  analogWrite(PWMA2, 240);
  analogWrite(PWMB2, 240);

  // Controller Initialization
  Ps3.attach(notify);
  Ps3.attachOnConnect(onConnect);
  Ps3.begin("2c:81:58:3a:93:f7");
}

void loop() {
  // Waiting to Pair
  if (!Ps3.isConnected()) {
    return;
  }

  stand();
}