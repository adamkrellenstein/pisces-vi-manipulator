#!/usr/bin/env python3

import random
import sys
from importlib import reload  
import time
import inputs
from numpy import interp
import math

from adafruit_servokit import ServoKit
kit = ServoKit(channels=16)


# PWM Addresses for Each Pair of Muscles
armPitchAddrBlack, armPitchAddrGrey     = 0, 1
armYawAddrBlack, armYawAddrGrey         = 2, 3
wristPitchAddrBlack, wristPitchAddrGrey = 4, 5
wristRollAddrBlack, wristRollAddrGrey   = 6, 7
clawAddrBlack, clawAddrGrey             = 8, 9
shoulderAddrBlack, shoulderAddrGrey     = 10, 11
auxOneAddrBlack, auxOneAddrGrey         = 12, 13
auxTwoAddrBlack, auxTwoAddrGrey         = 14, 15
buttonAddrList = [10, 11, 12, 13, 14, 15]

# Global Variable Initialization
slow_mode = False
x_mode = False
y_mode = False
triggerMin = 0          # Dead zone
triggerMax = 1023
hatMin = 4000           # Dead zone
hatMax = 32767
outSlow = 0.05          # (5%) TODO: Calibrate
minPWM = 0              # TODO: Calibrate
maxPWM = 19988          # TODO: Calibrate


def translate(value, leftMin, leftMax, rightMin, rightMax):
    # Figure out how 'wide' each range is
    leftSpan = leftMax - leftMin
    rightSpan = rightMax - rightMin

    # Convert the left range into a 0-1 range (float)
    valueScaled = float(value - leftMin) / float(leftSpan)

    # Convert the 0-1 range into a value in the right range.
    return rightMin + (valueScaled * rightSpan)


def setPWM(address, value):
    kit.servo[address].angle = value * 180


def scale(val, inMin, inMax):
    # This function determines how hat values scale. Because we want more
    # sensitivity on the low-end, all values are square-rooted---compressing the
    # values asymmetrically, with more compression the larger the value.

    if slow_mode:
        return outSlow
    else:
        return round(translate(abs(val), inMin, inMax, 0, 1), 3)


def moveMusclePairTrigger(label, triggerAddr, antiAddr, triggerVal):
    if triggerVal > triggerMin:
        triggerVal = scale(triggerVal, triggerMin, triggerMax)
        antiVal = 0
    else:
        triggerVal = 0
        antiVal = 0

    print(label+": Trigger: ", triggerVal)
    setPWM(triggerAddr, triggerVal)
    setPWM(antiAddr, antiVal)


def moveMusclePairButton(label, buttonAddr, buttonVal):
    print(label+": Button: ", buttonVal)
    if buttonVal:
        setPWM(buttonAddr, scale(1, 0, 1))
    else:
        setPWM(buttonAddr, 0)

    # When one button is pressed, turn off all other buttons (even across modes).
    for i in [addr for addr in buttonAddrList if addr != buttonAddr]:    # Buttons can stick if you change modes while letting off a button.
        setPWM(i, 0)

def moveMusclePairHat(label, blackAddr, greyAddr, hatVal):
    # Move a muscle pair in one dimension using an analog hat value.

    # Set muscle values.
    if (abs(hatVal) <= hatMin):
        # Hat is in dead zone.
        blackVal = 0
        greyVal = 0

    elif (hatVal > hatMin):
        # Hat is up/right.
        greyVal = 0;
        blackVal = scale(hatVal, hatMin, hatMax)

    else:
        # Hat is down/left.
        blackVal = 0;
        greyVal = scale(hatVal, hatMin, hatMax)

    if blackVal or greyVal:
        print(label, "Black:", blackVal, "Grey:", greyVal)
    setPWM(blackAddr, blackVal)
    setPWM(greyAddr, greyVal)


def handle_event():
    global slow_mode, x_mode, y_mode

    try:
        events = inputs.get_gamepad()
    except (OSError, inputs.UnpluggedError) as err:
        indent = random.randint(0, 12)
        print(indent*' ' + 'No device found!')
        time.sleep(1)
        reload(inputs)
        return

    for event in events:

        # Set mode.
        if event.code == 'BTN_SELECT':
            if event.state:
                slow_mode = True
            else:
                slow_mode = False
        elif event.code == 'BTN_NORTH' or event.code == 'BTN_SOUTH':
            if event.state:
                x_mode = True
            else:
                x_mode = False
        elif event.code == 'BTN_WEST' or event.code == 'BTN_EAST':
            if event.state:
                y_mode = True
            else:
                y_mode = False

        # RightHat -> Arm
        elif event.code == 'ABS_RY':
            moveMusclePairHat("Arm Pitch", armPitchAddrBlack, armPitchAddrGrey, event.state)
        elif event.code == 'ABS_RX':
            moveMusclePairHat("Arm Yaw", armYawAddrBlack, armYawAddrGrey, event.state)

        # LeftHat -> Wrist
        elif event.code == 'ABS_Y':
            moveMusclePairHat("Wrist Pitch", wristPitchAddrBlack, wristPitchAddrGrey, event.state)
        elif event.code == 'ABS_X':
            moveMusclePairHat("(Wrist) Roll", wristRollAddrBlack, wristRollAddrGrey, event.state)

        # Triggers -> Claw
        elif event.code == 'ABS_Z':
            moveMusclePairTrigger("ClawOpen", clawAddrBlack, clawAddrGrey, event.state)
        elif event.code == 'ABS_RZ':
            moveMusclePairTrigger("ClawClose", clawAddrGrey, clawAddrBlack, event.state)

        # Shoulder Buttons -> Shoulder/AuxOne/AuxTwo
        elif event.code == 'BTN_TL':
            if x_mode:
                moveMusclePairButton("AuxOne Up", auxOneAddrGrey, event.state)
            elif y_mode:
                moveMusclePairButton("AuxTwo Up", auxTwoAddrGrey, event.state)
            else:
                moveMusclePairButton("Extend", shoulderAddrGrey, event.state)
        elif event.code == 'BTN_TR':
            if x_mode:
                moveMusclePairButton("AuxOne Down", auxOneAddrBlack, event.state)
            elif y_mode:
                moveMusclePairButton("AuxTwo Down", auxTwoAddrBlack, event.state)
            else:
                moveMusclePairButton("Retract", shoulderAddrBlack, event.state)

        elif event.code == 'BTN_MODE':
            if event.state:
                print('Shutting down...')
            else:
                print('Shutdown aborted.')

        elif event.code == 'SYN_REPORT':
            pass
        else:
            print("Unknown command:", event.code)


        # Instead of looping as fast as possible, delay for 1 ms between
        # cycles.
        time.sleep(.001)


def init():

    # PWM Configuration
    for i in range(0, 16):
        kit.servo[i].set_pulse_width_range(minPWM, maxPWM)

    # Set everything to zero on start.
    for i in range(0, 16):
        setPWM(i, 0)

    print("***All Systems Nominal***")


if __name__ == '__main__':
    init()
    while True:
        handle_event()
