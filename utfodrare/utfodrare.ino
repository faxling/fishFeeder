#include "Stepper.h"

Stepper s = Stepper(2048, 8, 10, 9, 11);
volatile int doOffset = 0;
volatile int doStep = 0;
volatile unsigned long nIntervalDays = 1;
volatile unsigned long nOffsetHours = 0;
volatile unsigned long nSecCounter = 0;
unsigned long nStartCount = 300;
unsigned long nStartCount2 = 0;
volatile unsigned long nLastValCount = 0;
volatile unsigned long nFlashCount = 1;
volatile unsigned long nSecADay;

void runStep0() {
  int nLRD = digitalRead(2);
  if (nSecCounter > 20)
    doOffset = 0;


  if ((nLRD == LOW) && (nLastValCount > 25)) {
    nLastValCount = 0;

    if (doOffset == 1) {
      nStartCount = 20;
      nSecCounter = 0;
      digitalWrite(6, HIGH);
      ++nOffsetHours;
      Serial.println(nOffsetHours);
      return;
    }


    nStartCount2 = 20;
    digitalWrite(5, HIGH);

    if (nSecCounter > 5 && nIntervalDays >= 1) {
      nSecCounter = 0;
      nOffsetHours = 0;
      nIntervalDays = 1;
    } else {
      nSecCounter = 0;
      ++nIntervalDays;
    }
    Serial.println(nIntervalDays);
  }
}

void runStep1() {
  if (digitalRead(3) == HIGH)
    doStep = 0;
  else
    doStep = 1;
}

ISR(TIMER1_OVF_vect) {
  TCNT1 = 3036;  // Timer Preloading
  ++nSecCounter;

  if (nSecCounter == 5) {
    if (doOffset == 1)
      nFlashCount = nOffsetHours;
    else
      nFlashCount = nIntervalDays;
  }
}

// 1 sec 3 036
void setup() {
  // The prescaler is a 3-bit value stored in the three least significant bits of the Timer/Counter register
  Serial.begin(9600);
  nSecADay = (unsigned long)24 * (unsigned long)3600;

  pinMode(3, INPUT_PULLUP);  // tryckknapp framkörning till 'Startläge'
  pinMode(2, INPUT_PULLUP);  // Nollställa timer
  // "if digitalRead(4) == LOW" {någon har jordat pinne 4}
  //  pinMode(8, INPUT_PULLUP);  // Nollställa timer
  pinMode(5, OUTPUT);  // Grön diod
  pinMode(6, OUTPUT);  // Blå diod
  pinMode(7, OUTPUT);  // Röd diod (

  attachInterrupt(digitalPinToInterrupt(2), runStep0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), runStep1, CHANGE);

  //  attachInterrupt(digitalPinToInterrupt(3), runStep0, HIGH);
  TCCR1A = 0;           // Init Timer1
  TCCR1B = 0;           // Init Timer1
  TCCR1B |= B00000100;  // Prescalar = 256
  TCNT1 = 3036;         // Timer Preloading
  TIMSK1 |= B00000001;  // Enable Timer Overflow Interrupt

  s.setSpeed(2);
  digitalWrite(6, HIGH);
}



void loop() {
  // put your main code here, to run repeatedly:

  if (nStartCount == 1)
    digitalWrite(6, LOW);

  if (nStartCount > 0) {
    --nStartCount;
    delay(10);
    return;
  }

  if (nStartCount2 == 1)
    digitalWrite(5, LOW);

  if (nStartCount2 > 0)
    --nStartCount2;

  if (nFlashCount > 0) {
    int nPin = 5;
    if (doOffset == 1)
      nPin = 6;

    int n = digitalRead(nPin);
    digitalWrite(nPin, !n);
    if (n == 1)
      --nFlashCount;
    delay(300);


    if (nFlashCount == 0) {
      if (doOffset == 1) {
        Serial.println("Set Offset");
        Serial.println(nOffsetHours);
      }
      if (doOffset == 0) {
        Serial.println("Set Intervall");
        Serial.println(nIntervalDays);
      }
      if (doOffset == 1) {
        if (nOffsetHours > 0) {
          nSecCounter = (unsigned long)3600 * (unsigned long)(24 - nOffsetHours);
          nOffsetHours = 0;
        }
      }
      if (doOffset == 0)
        doOffset = 1;
      else
        doOffset = 0;
    }

    return;
  }

  if (doStep == 1) {
    digitalWrite(7, HIGH);
    s.step(1);
  } else if (nSecCounter > (nSecADay * nIntervalDays)) {
    nSecCounter = 0;
    digitalWrite(7, HIGH);
    Serial.println("Feed");
    s.step(256);
  } else {
    digitalWrite(7, LOW);
    ++nLastValCount;
    delay(10);
  }
}
