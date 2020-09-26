#! /usr/bin/env python3
from inputs import get_gamepad
print("***All Systems Nominal***")

while True:
    events = get_gamepad()
    for event in events:
        print(event.ev_type, event.code, event.state)
