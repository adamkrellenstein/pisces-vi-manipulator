#! /usr/bin/env python2

import time
from inputs import get_gamepad

from numpy import interp

from adafruit_servokit import ServoKit
kit = ServoKit(channels=16)
kit.servo[0].set_pulse_width_range(0, 19988);



print("***All Systems Nominal***")

# kit.servo[0].angle = i

while True:
    events = get_gamepad()
    for event in events:
        if event.code == 'ABS_RY':
            num = event.state
            foo = interp(num,[0,32768],[0,180])
            print(foo)
        # print(event.ev_type, event.code, event.state)
