#include <Pixy2.h>
#include <L298NX2.h>

Pixy2 pixy;

const unsigned int EN_A = 3;
const unsigned int IN1_A = 5;
const unsigned int IN2_A = 6;

const unsigned int IN1_B = 7;
const unsigned int IN2_B = 8;
const unsigned int EN_B = 9;

// For if we need just plain PWM pins
// int motor[2][3] = [[EN_A, IN1_A, IN2_A], [IN1_B, IN2_B, EN_B]];

L298NX2 motors(EN_A, IN1_A, IN2_A, EN_B, IN1_B, IN2_B);

const float deadzone = 0.15;

const int turnHigh = 150;
const int turnLow = 20;

int signature, x, width;

enum TurnSpeed {
  ForwardHigh,
  ForwardLow,
  BackHigh,
  BackLow
};

void setup() {
  Serial.begin(115200);
  Serial.print("Starting...\n");

  pixy.init();
}

void loop() {
  pixy.ccc.getBlocks();

  if (pixy.ccc.numBlocks) {
    debugBlocks();
    focusObject();
  }
}

void debugBlocks() {
  Serial.print("Detected ");
  Serial.println(pixy.ccc.numBlocks);
  for (int i = 0; i < pixy.ccc.numBlocks; i++) {
    Serial.print("  block ");
    Serial.print(i);
    Serial.print(": ");
    pixy.ccc.blocks[i].print();
  }
}
void focusObject() {
  float centre = getCentre();

  if (-deadzone <= centre && centre <= deadzone) {
    centre = 0;
    moveRobot(ForwardLow, ForwardLow);
  } else if (centre < 0) {
    moveRobot(ForwardHigh, BackLow);
  } else if (centre > 0) {
    moveRobot(BackLow, ForwardHigh);
  }
}

float getCentre() {
  signature = pixy.ccc.blocks[0].m_signature;
  x = pixy.ccc.blocks[0].m_x;
  width = pixy.ccc.blocks[0].m_width;

  // Get the centre of the object to keep that in the middle
  float midpointX = (x + (width / 2));

  return map(midpointX, 0, 320, -1, 1);
}

void moveRobot(TurnSpeed a, TurnSpeed b) {
  switch (a) {
    case ForwardHigh:
      motors.setSpeedA(turnHigh);
      motors.forwardA();
      break;
    case ForwardLow:
      motors.setSpeedA(turnLow);
      motors.forwardA();
      break;
    case BackHigh:
      motors.setSpeedA(turnHigh);
      motors.backwardA();
      break;
    case BackLow:
      motors.setSpeedA(turnLow);
      motors.backwardA();
      break;
  }
  switch (b) {
    case ForwardHigh:
      motors.setSpeedB(turnHigh);
      motors.forwardB();
      break;
    case ForwardLow:
      motors.setSpeedB(turnLow);
      motors.forwardB();
      break;
    case BackHigh:
      motors.setSpeedB(turnHigh);
      motors.backwardA();
      break;
    case BackLow:
      motors.setSpeedB(turnLow);
      motors.backwardA();
      break;
  }
}
