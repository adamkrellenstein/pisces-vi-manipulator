import time
from inputs import get_gamepad
from numpy import interp
import math

from adafruit_servokit import ServoKit
kit = ServoKit(channels=16)
kit.servo[0].set_pulse_width_range(0, 19988)   # TODO
kit.servo[1].set_pulse_width_range(0, 19988)   # TODO


# PWM Addresses for Each Pair of Muscles
armPitchAddrBlack, armPitchAddrGrey     = 0, 1
armYawAddrBlack, armYawAddrGrey         = 2, 3
wristPitchAddrBlack, wristPitchAddrGrey = 4, 5
wristRollAddrBlack, wristRollAddrGrey   = 6, 7
clawAddrBlack, clawAddrGrey             = 8, 9
# auxOneAddrBlack        = 10, auxOneAddrGrey      = 11
# auxTwoAddrBlack        = 12, auxTwoAddrGrey      = 13
# shoulderAddrBlack      = 14, shoulderAddrGrey    = 15


# Global Variable Initialization
slow = 0        # By default, slow mode is off.
outSlow = 0.1   # TODO: minimum power that still will move things


def translate(value, leftMin, leftMax, rightMin, rightMax):
    # Figure out how 'wide' each range is
    leftSpan = leftMax - leftMin
    rightSpan = rightMax - rightMin

    # Convert the left range into a 0-1 range (float)
    valueScaled = float(value - leftMin) / float(leftSpan)

    # Convert the 0-1 range into a value in the right range.
    return rightMin + (valueScaled * rightSpan)


def setPWM(address, value):
    # Actually do the thing. Send the black and grey values---whatever they
    # have been determined to be---to the PWM controller to control the voltage
    # that the solenoid in each proportional valve will be fed.
    kit.servo[address].angle = value * 180


def scale(val, inMin, inMax):
    # This function determines how hat values scale. Because we want more
    # sensitivity on the low-end, all values are square-rooted---compressing the
    # values asymmetrically, with more compression the larger the value.

    return round(translate(abs(val), inMin, inMax, 0, 1), 3)
    # return round(translate(math.sqrt(abs(val)), math.sqrt(inMin), math.sqrt(inMax), 0, 1), 3)


def moveMusclePairTrigger(label, blackAddr, greyAddr, triggerValBlack, triggerValGrey, slow):
    triggerMin = 300      # Dead zone
    triggerMax = 1023

    if (triggerValBlack > 0 and triggerValGrey > 0):
        # If both triggers are pulled, stop moving the claw.
        blackVal = 0
        greyVal = 0
    elif (triggerValBlack > triggerMin):
        blackVal = scale(triggerValBlack, triggerMin, triggerMax)
        greyVal = 0
    elif (triggerValGrey > triggerMin):
        blackVal = 0
        greyVal = scale(triggerValGrey, triggerMin, triggerMax)
    else:
        blackVal = 0
        greyVal = 0

    # print(label+": Black: "+blackVal+", Grey: "+greyVal+"  |  ")
    setPWM(blackAddr, blackVal)
    setPWM(greyAddr, greyVal)


def moveMusclePairButton(label, blackAddr, greyAddr, shoulderValBlack, shoulderValGrey, slow):
  
    if (shoulderValBlack and shoulderValGrey):
        # If both buttons are pressed, stop moving the arm.
        blackVal = 0
        greyVal = 0
    elif (shoulderValBlack):
        blackVal = 1
        greyVal = 0
    elif (shoulderValGrey):
        blackVal = 0
        greyVal = 1
    else:
        blackVal = 0
        greyVal = 0

    # print(label+": Black: "+blackVal+", Grey: "+greyVal+"  |  ")
    setPWM(blackAddr, blackVal)
    setPWM(greyAddr, greyVal)


def moveMusclePairHat(label, blackAddr, greyAddr, hatVal, slow):
    # Move a muscle pair in one dimension using an analog hat value.
    hatMax = 32767

    if (slow):
        # In 'slow' mode, the dead zone has to be larger: you should have to move
        # the stick all the way over for even the minimum power to be applied.
        hatMin = 31000  # Dead zone (slow mode)
    else:
        hatMin = 7500   # Dead zone (normal)

    # Set muscle values.
    if (abs(hatVal) <= hatMin):
        # Hat is in dead zone.
        blackVal = 0
        greyVal = 0

    elif (hatVal > hatMin):
        # Hat is up/right.
        greyVal = 0;
        if (slow):
            blackVal = outSlow
        else:
            blackVal = scale(hatVal, hatMin, hatMax)

    else:
        # Hat is down/left.
        blackVal = 0;
        if (slow):
            greyVal = outSlow
        else:
            greyVal = scale(hatVal, hatMin, hatMax)

    # print(label, "Black:", blackVal, "Grey:", greyVal, "|");
    # print(label+": Black: "+blackVal+", Grey: "+greyVal+"  |  ");
    print(label, hatVal, "Black:", blackVal, "Grey:", greyVal, "|");
    setPWM(blackAddr, blackVal)
    setPWM(greyAddr, greyVal)


def main_loop():
    events = get_gamepad()
    for event in events:

        # Hold the `BACK` button on the Xbox controller to make all muscles
        # move as slowly as possible.
        # slow = Xbox.getButtonPress(BACK);

        # RightHat -> Arm
        if event.code == 'ABS_RY':
            value = event.state
            moveMusclePairHat("Arm Pitch", armPitchAddrBlack, armPitchAddrGrey, value, slow);

        # moveMusclePairHat("Arm Yaw", armYawAddrBlack, armYawAddrGrey, Xbox.getAnalogHat(RightHatX), slow);

        # LeftHat -> Wrist
        # moveMusclePairHat("Wrist Pitch", wristPitchAddrBlack, wristPitchAddrGrey, Xbox.getAnalogHat(LeftHatY), slow);
        # moveMusclePairHat("(Wrist) Roll", wristRollAddrBlack, wristRollAddrGrey, Xbox.getAnalogHat(LeftHatX), slow);

        # print(Xbox.getButtonClick(R1)+";"+Xbox.getButtonPress(R1)+";"+Xbox.getButtonClick(L1));

        # if (Xbox.getButtonPress(Y)) {
        #   // Triggers + Y -> AuxOne
        #   moveMusclePairTrigger("AuxOne", auxOneAddrBlack, auxOneAddrGrey, Xbox.getButtonPress(R2), Xbox.getButtonPress(L2), slow);
        # elif (Xbox.getButtonPress(X)) {
        #   // Triggers + X -> AuxTwo
        #   moveMusclePairTrigger("AuxTwo", auxTwoAddrBlack, auxTwoAddrGrey, Xbox.getButtonPress(R2), Xbox.getButtonPress(L2), slow);
        # else {
        #   // Triggers -> Claw
        #   moveMusclePairTrigger("Claw", clawAddrBlack, clawAddrGrey, Xbox.getButtonPress(R2), Xbox.getButtonPress(L2), slow);
        # Shoulder Buttons -> Shoulder (shoulderAddrGrey is retracting)
        # moveMusclePairButton("Extend", shoulderAddrBlack, shoulderAddrGrey, Xbox.getButtonPress(R1), Xbox.getButtonPress(L1), slow);

        # Log a newline, so each loop has its own line of debug output.
        # print()

        # Instead of looping as fast as possible, delay for 1 ms between
        # cycles.
        time.sleep(.001)


if __name__ == '__main__':
    print("***All Systems Nominal***")
    while True:
        main_loop()
