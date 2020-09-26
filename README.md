# Introduction

- The Arduino code is in [Arduino/manipulator/manipulator.ino](Arduino/manipulator/manipulator.ino).
	- It is near-complete, however there are persistent issues with dropped signals that effectively cause buttons to stick.
		- It is thought that these issues might resolve themselves with a larger, faster Arduino.
- The Raspberry Pi code is in [manipulator.py](manipulator.py).
	- Cron starts the `tmux` server on system boot.
	- `tmux` is configured to run this service when the `tmux` server starts up.
