// Gamepad
#include <XBOXONE.h>
#include <SPI.h>
USB Usb;
XBOXONE Xbox(&Usb);

// PWM
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();


void setup() {
  // Initialize Serial Port, for debugging.
  Serial.begin(115200);
  while (!Serial);

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


void moveMusclePair(int blackAddr, int greyAddr, int hatVal, String label) {
  // Move a muscle pair in one dimension using an analog hat value.

  int blackVal;
  int greyVal;
  const int hatMax = 32767;
  const int hatMin = 10000; // Dead Zone
  const int outMax = 75;
  const int outMin = 0;

  // Set muscle values.
  if (abs(hatVal) <= hatMin) {
    // Hat is in dead zone.
    blackVal = 0;
    greyVal = 0;
  } 
  else {
    if (hatVal > hatMin) {
      // Hat is up/right.
      blackVal = map(hatVal, 0, hatMax, 0, outMax);
      greyVal = 0;
    }
    else {
      // Hat is down/left.
      blackVal = 0;
      greyVal = abs(map(hatVal, 0, hatMax, 0, outMax));
    }

    // Log values (if non-zero).
    Serial.print((String)label+": Black: "+blackVal+", Grey: "+greyVal+"\n");
  }

  // Set PWM.
  pwm.setPWM(blackAddr, 0, blackVal);
  pwm.setPWM(greyAddr, 0, greyVal);
}


void loop() {
  Usb.Task();
  if (!Xbox.XboxOneConnected) {
    delay(1);
    return;
  }

  // PWM addresses for each pair of muscles
  const int armPitchAddrBlack      = 0, armPitchAddrGrey     = 1;
  const int armYawAddrBlack        = 2, armYawAddrGrey       = 3;
  const int wristPitchAddrBlack    = 4, wristPitchAddrGrey   = 5;
  const int wristRollAddrBlack     = 6, wristRollAddrGrey    = 7;

  // RightHat -> Arm
  moveMusclePair(armPitchAddrBlack, armPitchAddrGrey, Xbox.getAnalogHat(RightHatY), "Arm Pitch");
  moveMusclePair(armYawAddrBlack, armYawAddrGrey, Xbox.getAnalogHat(RightHatX), "Arm Yaw");

  // RightHat -> Wrist
  moveMusclePair(wristPitchAddrBlack, wristPitchAddrGrey, Xbox.getAnalogHat(LeftHatY), "Wrist Pitch");
  moveMusclePair(wristRollAddrBlack, wristRollAddrGrey, Xbox.getAnalogHat(LeftHatX), "Wrist Roll");
















  // // Triggers
  // if (Xbox.getButtonPress(L2) > 0 || Xbox.getButtonPress(R2) > 0) {
  //   if (Xbox.getButtonPress(L2) > 0) {
  //     Serial.print(F("L2: "));
  //     Serial.print(Xbox.getButtonPress(L2));
  //     Serial.print("\t");
  //   }
  //   if (Xbox.getButtonPress(R2) > 0) {
  //     Serial.print(F("R2: "));
  //     Serial.print(Xbox.getButtonPress(R2));
  //     Serial.print("\t");
  //   }
  //   Serial.println();

  //   // Triggers as Buttons
  //   // if (Xbox.getButtonClick(L2))
  //   //  Serial.println(F("L2"));
  //   // if (Xbox.getButtonClick(R2))
  //   //  Serial.println(F("R2"));
  // }


  // // D-Pad
  // if (Xbox.getButtonClick(UP))
  //   Serial.println(F("Up"));
  // if (Xbox.getButtonClick(DOWN))
  //   Serial.println(F("Down"));
  // if (Xbox.getButtonClick(LEFT))
  //   Serial.println(F("Left"));
  // if (Xbox.getButtonClick(RIGHT))
  //   Serial.println(F("Right"));


  // // Specials
  // // if (Xbox.getButtonClick(UP))
  // // if (Xbox.getButtonClick(START))
  // //   Serial.println(F("Start"));
  // // if (Xbox.getButtonClick(BACK))
  // //   Serial.println(F("Back"));
  // // if (Xbox.getButtonClick(XBOX))
  // //   Serial.println(F("Xbox"));
  // // if (Xbox.getButtonClick(SYNC))
  // //   Serial.println(F("Sync"));


  // // Bumpers
  // if (Xbox.getButtonClick(L1))
  //   Serial.println(F("L1"));
  // if (Xbox.getButtonClick(R1))
  //   Serial.println(F("R1"));
  // if (Xbox.getButtonClick(L3))
  //   Serial.println(F("L3"));
  // if (Xbox.getButtonClick(R3))
  //   Serial.println(F("R3"));


  // // Letters
  // if (Xbox.getButtonClick(A))
  //   Serial.println(F("A"));
  // if (Xbox.getButtonClick(B))
  //   Serial.println(F("B"));
  // if (Xbox.getButtonClick(X))
  //   Serial.println(F("X"));
  // if (Xbox.getButtonClick(Y))
  //   Serial.println(F("Y"));


  delay(1);
}
