#!/usr/bin/env python3

import os
import contextlib   # Hack to silence PyGame support message.
with contextlib.redirect_stdout(None):
    import pygame

# Initialize PyGame
pygame.init()
# os.environ["DISPLAY"] = ":0.0"

# Initialize Joysticks
pygame.joystick.init()
joysticks = [pygame.joystick.Joystick(i) for i in range(pygame.joystick.get_count())]
if len(joysticks) == 0:
    print("No joystick found.")
    exit(1)
elif len(joysticks) > 1:
    print("Multiple joysticks found:", joysticks)
    exit(1)
else:
    joystick = joysticks[0]
    joystick.init()
    print("Joystick initialized:", joystick.get_name())


print("***All Systems Nominal***")
while True:
    for event in pygame.event.get():

        axes = joystick.get_numaxes()
        print("Number of axes: {}".format(axes))

        for i in range(axes):
            axis = joystick.get_axis(i)
            print("Axis {} value: {:>6.3f}".format(i, axis))

        buttons = joystick.get_numbuttons()
        print("Number of buttons: {}".format(buttons))

        for i in range(buttons):
            button = joystick.get_button(i)
            print("Button {:>2} value: {}".format(i, button))

        hats = joystick.get_numhats()
        print("Number of hats: {}".format(hats))

        # D-Pad
        for i in range(hats):
            hat = joystick.get_hat(i)
            print("Hat {} value: {}".format(i, str(hat)))

pygame.quit()
