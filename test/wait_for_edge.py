#!/usr/bin/env python

import LPi.GPIO as GPIO
import time

def my_callback(channel):
    print('callback')

touchPin=21

GPIO.setmode(GPIO.LS2K)
GPIO.setup(touchPin,GPIO.IN)

GPIO.add_event_detect(touchPin, GPIO.RISING)
#GPIO.add_event_detect(touchPin, GPIO.RISING, callback=my_callback)
print ('start')

GPIO.wait_for_edge(touchPin,GPIO.RISING)

print ('found RISING')

GPIO.wait_for_edge(touchPin, GPIO.FALLING)

print ('found FALLING')

if GPIO.event_detected(touchPin):
    print('Button pressed')

GPIO.remove_event_detect(touchPin)
