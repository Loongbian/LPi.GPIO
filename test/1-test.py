import LPi.GPIO as GPIO

class TestGPIOFunction(unittest.TestCase):
    def runTest(self):
        GPIO.setmode(GPIO.BCM)

    def tearDown(self):
        GPIO.cleanup()


if __name__ == '__main__':
    unittest.main()

