import unittest
import LPi as GPIO


LED_PIN = 12

# Test starts with 'AAA' so that it is run first
class TestAAASetup(unittest.TestCase):
    def runTest(self):
        # Test mode not set (BOARD or BCM) exception
        #with self.assertRaises(RuntimeError) as e:
        #    GPIO.setup(LED_PIN, GPIO.OUT)
        #self.assertEqual(str(e.exception), 'Please set pin numbering mode using GPIO.setmode(GPIO.BOARD) or GPIO.setmode(GPIO.BCM)')

        # Test trying to change mode after it has been set
        GPIO.setmode(GPIO.BCM)
        with self.assertRaises(ValueError) as e:
            GPIO.setmode(GPIO.BOARD)
        GPIO.setup(LED_PIN_BCM, GPIO.IN)
        GPIO.cleanup()



class Test(unittest.TestCase):
    def runTest(self):
        GPIO.helloworld()


if __name__ == '__main__':
    # begin the unittest.main()
    unittest.main()

