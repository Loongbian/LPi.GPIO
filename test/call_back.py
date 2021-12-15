import LPi.GPIO as gpio
import time


gpio.setmode(gpio.BOARD)
gpio.setup(21,gpio.IN)
def on_pushdown(channel):
    print("Button Pushed.")

gpio.add_event_detect(21, gpio.RISING, callback=on_pushdown, bouncetime=200)
while(True):
    try:
        if gpio.event_detected(21):
            print('Button pressed')
        time.sleep(1)
    finally:
        time.sleep(1)
#        gpio.cleanup()
