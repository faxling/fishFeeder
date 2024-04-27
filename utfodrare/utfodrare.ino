#include "Stepper.h"

// Stepper s = Stepper(2048, 8, 10, 9, 11);
byte buttonUpPin = 10;
byte buttonDownPin = 11;
byte dirPin = 5;
byte stepPin = 4;
#define motorPin1 8   // IN1 on the ULN2003 driver 1
#define motorPin2 10  // IN2 on the ULN2003 driver 1
#define motorPin3 9   // IN3 on the ULN2003 driver 1
#define motorPin4 11  // IN4 on the ULN2003 driver 1

byte sbt0811_in[4] = { 8, 9, 10, 11 };  // IN1, IN2, IN3, IN4 to arduino pins

int full_steps_count = 2048;  // steps for a full cycle
int step_time = 1;            // 1ms for one step

int direction = 1;  // will be either 1 (clockwise) or -1 (anti-clockwise)
int steps_left = full_steps_count;
int current_step = 0;  // 0 to 7

boolean step_positions[8][4] = {
  { 1, 0, 0, 1 },
  { 1, 0, 0, 0 },
  { 1, 1, 0, 0 },
  { 0, 1, 0, 0 },
  { 0, 1, 1, 0 },
  { 0, 0, 1, 0 },
  { 0, 0, 1, 1 },
  { 0, 0, 0, 1 },
};

void setup() {
  for (byte i = 0; i <= 3; i++) {
    pinMode(sbt0811_in[i], OUTPUT);
  }
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(4, INPUT_PULLUP);  // tryckknapp framkörning till 'Startläge'
}
/*
void loop() {
  while (steps_left > 0) {
    move_stepper(1);
    steps_left -= 1;
  }
  steps_left = full_steps_count;
  direction = (direction > 0 ? -1 : 1);

  delay(500);
}
*/
void move_stepper(int step_count) {
  for (int i = 0; i < step_count; i++) {
    // step one step in given direction
    current_step += direction;
    if (current_step > 7) { current_step = 0; }
    if (current_step < 0) { current_step = 7; }

    // set digital pins for the current_step
    for (byte i = 0; i <= 3; i++) {
      digitalWrite(sbt0811_in[i], step_positions[current_step][i]);
    }

    // delay(step_time);
  }
}



/*
void setup() {

  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(4, INPUT_PULLUP);  // tryckknapp framkörning till 'Startläge'
                             // "if digitalRead(4) == LOW" {någon har jordat pinne 4}

  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);

  pinMode(5, OUTPUT);  // Röd diod ('digitalWrite(5,HIGH)' tänder röd diod
  pinMode(6, OUTPUT);  // Grön diod
  pinMode(7, OUTPUT);  // Blå diod

  digitalWrite(motorPin1, HIGH);



  // s.setSpeed(2);
}

*/
long a = 0;
long blink = 0;
long lastBtn = 0;
long blinkCount = 0;
long blinkTime = 30;
long steps = 256;
long stepsCount = 0;
int diodId = 5;
void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
  if (digitalRead(4) == LOW && lastBtn == 0) {
    if (blink == 0) {
      blink = 1;
      direction = (direction > 0 ? -1 : 1);
      stepsCount = steps;
    } else
      blink = 0;
    lastBtn = 1;
  }

  if (digitalRead(4) == HIGH && lastBtn == 1) {
    lastBtn = 0;
  }



  if (blink == 1) {


    //if (stepsCount > 0) {
      move_stepper(1);

      --stepsCount;
      // s.step(1);
    // }

    ++blinkCount;
    blinkCount %= (blinkTime * 2);
    diodId = (direction > 0 ? 5 : 6);
    if (blinkCount > blinkTime)
      digitalWrite(diodId, HIGH);
    else
      digitalWrite(diodId, LOW);

  } else {
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
  }

  
/*
  digitalWrite(13, HIGH);
  digitalWrite(5, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  digitalWrite(5, LOW);
*/


  // s.step(2048);
}
