// Gamepad Libraries
#include <XBOXONE.h>
#include <SPI.h>
USB Usb;
XBOXONE Xbox(&Usb);

// PWM Libraries
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();


// PWM addresses for each pair of muscles
const int armPitchAddrBlack      = 0, armPitchAddrGrey     = 1;
const int armYawAddrBlack        = 2, armYawAddrGrey       = 3;
const int wristPitchAddrBlack    = 4, wristPitchAddrGrey   = 5;
const int wristRollAddrBlack     = 6, wristRollAddrGrey    = 7;


void setup() {
  // Initialize Serial Port, for debugging.
  Serial.begin(115200);
  while (!Serial);
  Serial.print(F("\n\nSerial Port Initialized.\n"));

  // Initialize Gamepad.
  if (Usb.Init() == -1) {
    Serial.print(F("ERROR: OSC failed to start."));
    while (1); // Halt
  }
  Serial.print(F("Gamepad Initialized.\n"));

  // Initialize PWM.
  Serial.print(F("PWM Initialized.\n"));
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // Maximum PWM frequency
  // Wire.setClock(100000);

  Serial.print(F("***All Systems Nominal.***\n"));
}


int scale(int val) {
  return sqrt(abs(val));
}


void moveMusclePair(String label, int blackAddr, int greyAddr, int hatVal) {
  // Move a muscle pair in one dimension using an analog hat value.
  // TODO: Holding down a button, make all values minimum.

  int blackVal;
  int greyVal;
  int hatMax = 32767;
  int hatMin = 7500;                  // Dead zone
  if (hatVal == -32768) { hatVal++; } // Otherwise, triggers an overflow.
  const int outMax = 75;              // May be determined by a PWM frequency.
  const int outMin = 15;              // Ditto

  // Set muscle values.
  if (abs(hatVal) <= hatMin) {
    // Hat is in dead zone.
    blackVal = 0;
    greyVal = 0;
  } 
  else if (hatVal > hatMin) {
    // Hat is up/right.
    blackVal = map(scale(hatVal), scale(hatMin), scale(hatMax), outMin, outMax);
    greyVal = 0;
  }
  else {
    // Hat is down/left.
    blackVal = 0;
    greyVal = abs(map(scale(hatVal), scale(hatMin), scale(hatMax), outMin, outMax));
  }

  // Log values.
  Serial.print((String)label+": Black: "+blackVal+", Grey: "+greyVal+"  |  ");
  // if (label == "Arm Pitch") { Serial.print((String)scale(hatVal)+" "+scale(hatMin)+" "+scale(hatMax)+" "+blackVal); } // DEBUG

  // Set PWM.
  pwm.setPWM(blackAddr, 0, blackVal);
  pwm.setPWM(greyAddr, 0, greyVal);
}


void loop() {
  // Init.
  Usb.Task();
  if (!Xbox.XboxOneConnected) {
    delay(1);
    return;
  }

  // RightHat -> Arm
  moveMusclePair("Arm Pitch", armPitchAddrBlack, armPitchAddrGrey, Xbox.getAnalogHat(RightHatY));
  moveMusclePair("Arm Yaw", armYawAddrBlack, armYawAddrGrey, Xbox.getAnalogHat(RightHatX));

  // LeftHat -> Wrist
  moveMusclePair("Wrist Pitch", wristPitchAddrBlack, wristPitchAddrGrey, Xbox.getAnalogHat(LeftHatY));
  moveMusclePair("Wrist Roll", wristRollAddrBlack, wristRollAddrGrey, Xbox.getAnalogHat(LeftHatX));

  Serial.println();
  delay(1);
}
