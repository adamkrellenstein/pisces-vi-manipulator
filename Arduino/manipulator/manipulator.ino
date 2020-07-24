// Gamepad Dependencies and Variable Declarations
// (This was taken from the Xbox One Controller example code.)
#include <XBOXONE.h>
#include <SPI.h>
USB Usb;
XBOXONE Xbox(&Usb);

// PWM Dependencies and Variable Declarations
// (This was taken from the Adafruit PWM example code.)
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();


// PWM Addresses for Each Pair of Muscles
// (This establishes a set of constants used throughout the codebase to abstract away the particular PWM addresses for each valve. Each muscle pair comprises a ‘black’ and a ‘grey’ muscle.)
const int armPitchAddrBlack      = 0, armPitchAddrGrey     = 1;
const int armYawAddrBlack        = 2, armYawAddrGrey       = 3;
const int wristPitchAddrBlack    = 4, wristPitchAddrGrey   = 5;
const int wristRollAddrBlack     = 6, wristRollAddrGrey    = 7;
const int clawAddrBlack          = 8, clawAddrGrey         = 9;

// Global Variable Initialization
int slow = 0;     // By default, slow mode is off.

// Establish the maximum and minimum PWM power values that the valves take. When `outVal` is `75`, the valve opens all the way; when `outVal` is `15`, it just barely opens.
const int outMax = 75;              // May be determined by a PWM frequency.
const int outMin = 15;              // Ditto


void setup() {
  // This is a special function for Arduinos: it runs exactly once every time the Arduino is loaded with a new codebase or the board restarts.

  // Serial Port Initialization
  // (The Serial Port is used for debugging the Arduino. In particular, all logs (produced with `Serial.print[ln]()` are written to the Serial Port, and show up in the IDE's Serial Monitor.)
  Serial.begin(115200);
  while (!Serial);
  Serial.print(F("\n\nSerial Port Initialized.\n"));

  // Gamepad Initialization
  // (This is boilerplate code taken from the Xbox One Controller example code.)
  if (Usb.Init() == -1) {
    Serial.print(F("ERROR: OSC failed to start."));
    while (1); // Halt
  }
  Serial.print(F("Gamepad Initialized.\n"));

  // PWM Initialization
  // (This is boilerplate code taken from the Adafruit PWM example code.)
  Serial.print(F("PWM Initialized.\n"));
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // Maximum PWM frequency

  // MechWarrior Reference
  Serial.print(F("***All Systems Nominal.***\n"));
}


int scale(int val) {
  // This function determines how hat values scale. Because we want more sensitivity on the low-end, all values are square-rooted---compressing the values asymmetrically, with more compression the larger the value.
  return sqrt(abs(val));
}

void moveMusclePairTrigger(String label, int blackAddr, int greyAddr, int triggerValBlack, int triggerValGrey, int slow) {
  int blackVal;
  int greyVal;

  int triggerMin = 100; // TODO: Dead zone
  int triggerMax = 1023;

  if (triggerValBlack > 0 && triggerValGrey > 0) {
    blackVal = 0;
    greyVal = 0;
  }
  else if (triggerValBlack > triggerMin) {
    blackVal = outMax; // TODO
    greyVal = outMin;
  }
  else if (triggerValGrey > triggerMin) {
    blackVal = outMin;
    greyVal = outMax; // TODO
  }
  else {
    blackVal = 0;
    greyVal = 0;
  }

  Serial.print((String)label+": Black: "+blackVal+", Grey: "+greyVal+"  |  ");

}

void moveMusclePairHat(String label, int blackAddr, int greyAddr, int hatVal, int slow) {
  // Move a muscle pair in one dimension using an analog hat value.
  // (The `label` is a non-semantic string used only for logging purposes. The two addresses are the PWM addresses of the two muscles in the pair (for a single dimension of motion). The hat value is the value the gamepad uses to represent the deviation of an analog stick from center, for the dimension in question. When `slow` is non-zero (i.e. true), the analog sticks will only ever make muscles move at their absolute slowest rate (specified by the `outMin` value).)

  // Basic Variable Declaration
  int blackVal;
  int greyVal;

  // The Xbox One controller represents the position of an analog stick as a positive or negative 7-bit integer, and so has a range from `-32767` to `32767`.
  int hatMax = 32767;

  // In ‘slow’ mode, the dead zone has to be larger: you should have to move the stick all the way over for even the minimum power to be applied.
  int hatMin;
  if (slow) {
    hatMin = 31000;                   // Dead zone (slow mode)
  }
  else {
    hatMin = 7500;                    // Dead zone (normal)
  }

  if (hatVal == -32768) { hatVal++; } // There's some weirdness here that I don't understand. Without this, you get an overflow problem later.

  // Set muscle values.
  // (This is where the magic happens. Map the input value (taken from the analog stick) to the output value (for the PWM).)
  if (abs(hatVal) <= hatMin) {
    // Hat is in dead zone.
    // (When the absolute value of `hatVal` is less than or equal to `hatMin`, the analog stick is deemed to be in the dead zone. In this case, the output values for the black and grey muscles are just set to zero, so the valve definitely won't move at all..)
    blackVal = 0;
    greyVal = 0;
  } 
  else if (hatVal > hatMin) {
    // Hat is up/right.
    // (If `slow` is non-zero (i.e. true) when the analog stick is out of the dead zone with a positive value---up or right---then the black muscle is set to the minimum output value. If slow is zero (false), then all of the input values are scaled with the `scale()` function and then mapped to the output range. That is, ignoring the scaling function, `hatMax` -> `outMax`, `hatMin` -> `outMin` and `hatVal` goes to somewhere between `outMin` and `outMax`.)
    if (slow) {
      blackVal = outMin;
    }
    else {
      blackVal = map(scale(hatVal), scale(hatMin), scale(hatMax), outMin, outMax);
    }
    greyVal = 0;
  }
  else {
    // Hat is down/left.
    // (The same as above, but for the grey value, when the analog stick is down or to the left and `hatVal` is negative.)
    blackVal = 0;
    if (slow) {
      greyVal = outMin;
    }
    else {
      greyVal = abs(map(scale(hatVal), scale(hatMin), scale(hatMax), outMin, outMax));
    }
  }

  // Log values.
  // (Print the label for the muscle pair, and the ouput values for both the grey and the black muscles. This will be done once per muscle pair, for each program loop.)
  Serial.print((String)label+": Black: "+blackVal+", Grey: "+greyVal+"  |  ");
  // if (label == "Arm Pitch") { Serial.print((String)scale(hatVal)+" "+scale(hatMin)+" "+scale(hatMax)+" "+blackVal); } // DEBUG

  // Set PWM values.
  // (Actually do the thing. Send the black and grey values---whatever they have been determined to be---to the PWM controller to control the voltage that the solenoid in each proportional valve will be fed.)
  pwm.setPWM(blackAddr, 0, blackVal);
  pwm.setPWM(greyAddr, 0, greyVal);
}


void loop() {
  // Run this on every loop of the program (i.e. every 1 ms). Prepare to get new info over USB, from the gamepad, and if the gamepad isn't connected, then sleep for 1 ms, short-circuit the loop and try again.
  Usb.Task();
  if (!Xbox.XboxOneConnected) {
    delay(1);
    return;
  }

  // Hold the `BACK` button on the Xbox controller to make all muscles move as slowly as possible.
  slow = Xbox.getButtonPress(BACK);


  // (For each loop of the program, for each muscle pair, move that muscle pair based on the corresponding gamepad input value. For instance, use the `Y` dimension of the right analog stick to control arm pitch.)

  // RightHat -> Arm
  moveMusclePairHat("Arm Pitch", armPitchAddrBlack, armPitchAddrGrey, Xbox.getAnalogHat(RightHatY), slow);
  moveMusclePairHat("Arm Yaw", armYawAddrBlack, armYawAddrGrey, Xbox.getAnalogHat(RightHatX), slow);

  // LeftHat -> Wrist
  moveMusclePairHat("Wrist Pitch", wristPitchAddrBlack, wristPitchAddrGrey, Xbox.getAnalogHat(LeftHatY), slow);
  moveMusclePairHat("Wrist Roll", wristRollAddrBlack, wristRollAddrGrey, Xbox.getAnalogHat(LeftHatX), slow);

  if (Xbox.getButtonClick(L1)) {
    // Triggers + L1 -> Aux
  }
  else {
    // Triggers -> Claw
    moveMusclePairTrigger("Claw", clawAddrBlack, clawAddrGrey, Xbox.getButtonPress(R2), Xbox.getButtonPress(L2), slow);
  }


  // Log a newline over the Serial Port, so each loop has its own line of debug output.
  Serial.println();

  // Instead of looping as fast as possible, delay for 1 ms between cycles.
  delay(1);
}
