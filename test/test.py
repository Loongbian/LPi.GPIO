import unittest
import LPi.GPIO as GPIO


class Test(unittest.TestCase):
    def runTest(self):
        GPIO.helloworld()


if __name__ == '__main__':
    # begin the unittest.main()
    unittest.main()

