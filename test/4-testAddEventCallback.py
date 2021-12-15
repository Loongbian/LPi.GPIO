#!/usr/bin/env python

import LPi.GPIO as GPIO
import time
from threading import Timer
import sys

LOOP_OUT = 22
LOOP_IN = 16


def cb(channel):
    global callback_count
    callback_count += 1
    print('callback_count',callback_count)

# setUp(self)
#GPIO.setwarnings(False)
GPIO.setmode(GPIO.LS2K)
GPIO.setup(LOOP_IN, GPIO.IN)
GPIO.setup(LOOP_OUT, GPIO.OUT)

# falling test
callback_count = 0
GPIO.output(LOOP_OUT, GPIO.HIGH)
GPIO.add_event_detect(LOOP_IN, GPIO.FALLING)
GPIO.add_event_callback(LOOP_IN, cb)
time.sleep(0.01)
for i in range(2048):
    GPIO.output(LOOP_OUT, GPIO.LOW)
    time.sleep(0.001)
    GPIO.output(LOOP_OUT, GPIO.HIGH)
    time.sleep(0.001)
GPIO.remove_event_detect(LOOP_IN)


