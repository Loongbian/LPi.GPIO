import LPi.GPIO as GPIO

LED_PIN = 13

GPIO.setmode(GPIO.BOARD)
GPIO.setup(LED_PIN, GPIO.OUT)
GPIO.output(LED_PIN, GPIO.HIGH)
GPIO.cleanup()
