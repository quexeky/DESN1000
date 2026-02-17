#pragma region Imports
#include <Pixy2.h>
#include <L298NX2.h>
#pragma endregion
#pragma region Pins
const unsigned int EN_A = 3;
const unsigned int IN1_A = 5;
const unsigned int IN2_A = 6;

const unsigned int IN1_B = 7;
const unsigned int IN2_B = 8;
const unsigned int EN_B = 9;

// For if we need just plain PWM pins
// int motor[2][3] = [[EN_A, IN1_A, IN2_A], [IN1_B, IN2_B, EN_B]];

#pragma endregion Pins
#pragma region Control
L298NX2 motors(EN_A, IN1_A, IN2_A, EN_B, IN1_B, IN2_B);
Pixy2 pixy;
#pragma endregion
#pragma region Config
const float DEADZONE = 0.15;

const int TURN_HIGH = 150;
const int TURN_LOW = 20;

const int SCREEN_W = 320;
const int SCREEN_H = 240;
#pragma endregion
#pragma region Typedefs
struct BoundingBox {
  int x;
  int y;
  int width;
  int height;
};

enum TurnSpeed {
  ForwardHigh,
  ForwardLow,
  BackHigh,
  BackLow
};

int getTurnSpeedValue(TurnSpeed speed) {
  switch (speed) {
    case ForwardHigh:
      return TURN_HIGH;
    case ForwardLow:
      return TURN_LOW;
    case BackHigh:
      return TURN_HIGH;
    case BackLow:
      return TURN_LOW;
  }
}
#pragma endregion

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
    noBlockDetected();
  } else {

  }
}
void noBlockDetected() {
  // Placeholder for things like going back and turning around 
  // after a certain amount of time has passed
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
  BoundingBox box = getPixyCamBoundingBox();
  float centre = getCentre(box);

  if (-DEADZONE <= centre && centre <= DEADZONE) {
    centre = 0;
    moveRobot(ForwardLow, ForwardLow);
  } else if (centre < 0) {
    moveRobot(ForwardHigh, BackLow);
  } else if (centre > 0) {
    moveRobot(BackLow, ForwardHigh);
  }
}

BoundingBox getPixyCamBoundingBox() {
  Block block = pixy.ccc.blocks[0];
  return BoundingBox{
    block.m_x,
    block.m_y,
    block.m_width,
    block.m_height
  };
}

float getCentre(BoundingBox box) {
  // Get the centre of the object to keep that in the middle
  float midpointX = (box.x + (box.width / 2));

  return map(midpointX, 0, SCREEN_W, -1, 1);
}

void moveRobot(TurnSpeed a, TurnSpeed b) {
  switch (a) {
    case ForwardHigh:
      Serial.println("Set A to ForwardHigh");
      motors.forwardA();
      break;
    case ForwardLow:
      Serial.println("Set A to ForwardLow");
      motors.forwardA();
      break;
    case BackHigh:
      Serial.println("Set A to BackHigh");
      motors.backwardA();
      break;
    case BackLow:
      Serial.println("Set A to BackLow");
      motors.backwardA();
      break;
  };

  switch (b) {
    case ForwardHigh:
      Serial.println("Set B to ForwardHigh");
      motors.forwardB();
      break;
    case ForwardLow:
      Serial.println("Set B to ForwardLow");
      motors.forwardB();
      break;
    case BackHigh:
      Serial.println("Set B to BackHigh");
      motors.backwardB();
      break;
    case BackLow:
      Serial.println("Set B to BackLow");
      motors.backwardB();
      break;
  }
  int a_val = getTurnSpeedValue(a);
  int b_val = getTurnSpeedValue(b);
  Serial.println("TurnSpeed A: " + a_val);
  Serial.println("TurnSpeed B: " + b_val);
  motors.setSpeedA(a_val);
  motors.setSpeedB(b_val);
}
