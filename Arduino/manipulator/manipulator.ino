// GAMEPAD
#include <XBOXONE.h>
#include <SPI.h>
USB Usb;
XBOXONE Xbox(&Usb);


// PWM
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();


// SETUP
void setup() {
  // Initialize Serial Port, for debugging.
  Serial.begin(115200);
  while (!Serial); // Wait for serial port to connect

  // Initialize Gamepad.
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nXBOX USB Library Started\n"));

  // Initialize PWM.
  Serial.println("\n16 channel PWM test!\n");
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // Maximum PWM frequency
  Wire.setClock(400000);
}

int openValve(int addr, int hatVal) {

  int inVal = hatVal;
  const int inMax = 32767;
  const int inMin = 10000; // Dead Zone
  const int outMax = 75;
  const int outMin = 0;

  int blackVal;
  int greyVal;
  if (inVal < 7500) {
    greyVal = 0;
    blackVal = map(inVal, 0, inMax, 0, outMax);
  } 
  else {
    greyVal = map(inVal, 0, inMax, 0, outMax);
    blackVal = 0;
  }

  Serial.print(outVal);
  // Serial.print("\n");
  pwm.setPWM(1, 0, 1 ); // TODO
  pwm.setPWM(0, 0, strength); // TODO
  return outVal;
}


// MAIN LOOP
void loop() {
  Usb.Task();
  if (!Xbox.XboxOneConnected) {
    delay(1);
    return;
  }

  // Analog Sticks
  if (Xbox.getAnalogHat(LeftHatX) > 7500 || Xbox.getAnalogHat(LeftHatX) < -7500) {
    Serial.print(F("LeftHatX: "));
    Serial.print(Xbox.getAnalogHat(LeftHatX));
    Serial.print("\t");
  }
  if (Xbox.getAnalogHat(LeftHatY) > 7500 || Xbox.getAnalogHat(LeftHatY) < -7500) {
    Serial.print(F("LeftHatY: "));
    Serial.print(Xbox.getAnalogHat(LeftHatY));
    Serial.print("\t");
  }
  if (Xbox.getAnalogHat(RightHatX) > 7500 || Xbox.getAnalogHat(RightHatX) < -7500) {
    Serial.print(F("RightHatX: "));
    Serial.print(Xbox.getAnalogHat(RightHatX));
    Serial.print("\t");
  }

  int strength = getStrength(Xbox.getAnalogHat(RightHatY));

  Serial.println();


  // Triggers
  if (Xbox.getButtonPress(L2) > 0 || Xbox.getButtonPress(R2) > 0) {
    if (Xbox.getButtonPress(L2) > 0) {
      Serial.print(F("L2: "));
      Serial.print(Xbox.getButtonPress(L2));
      Serial.print("\t");
    }
    if (Xbox.getButtonPress(R2) > 0) {
      Serial.print(F("R2: "));
      Serial.print(Xbox.getButtonPress(R2));
      Serial.print("\t");
    }
    Serial.println();

    // Triggers as Buttons
    // if (Xbox.getButtonClick(L2))
    //  Serial.println(F("L2"));
    // if (Xbox.getButtonClick(R2))
    //  Serial.println(F("R2"));
  }


  // D-Pad
  if (Xbox.getButtonClick(UP))
    Serial.println(F("Up"));
  if (Xbox.getButtonClick(DOWN))
    Serial.println(F("Down"));
  if (Xbox.getButtonClick(LEFT))
    Serial.println(F("Left"));
  if (Xbox.getButtonClick(RIGHT))
    Serial.println(F("Right"));


  // Specials
  // if (Xbox.getButtonClick(UP))
  // if (Xbox.getButtonClick(START))
  //   Serial.println(F("Start"));
  // if (Xbox.getButtonClick(BACK))
  //   Serial.println(F("Back"));
  // if (Xbox.getButtonClick(XBOX))
  //   Serial.println(F("Xbox"));
  // if (Xbox.getButtonClick(SYNC))
  //   Serial.println(F("Sync"));


  // Bumpers
  if (Xbox.getButtonClick(L1))
    Serial.println(F("L1"));
  if (Xbox.getButtonClick(R1))
    Serial.println(F("R1"));
  if (Xbox.getButtonClick(L3))
    Serial.println(F("L3"));
  if (Xbox.getButtonClick(R3))
    Serial.println(F("R3"));


  // Letters
  if (Xbox.getButtonClick(A))
    Serial.println(F("A"));
  if (Xbox.getButtonClick(B))
    Serial.println(F("B"));
  if (Xbox.getButtonClick(X))
    Serial.println(F("X"));
  if (Xbox.getButtonClick(Y))
    Serial.println(F("Y"));


  delay(1);
}
