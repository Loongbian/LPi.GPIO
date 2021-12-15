import LPi.GPIO as GPIO
import unittest

LED_PIN_LS2K = 18


LED_PIN = 13

# Test starts with 'AAA' so that it is run first
class TestAAASetup(unittest.TestCase):
    def runTest(self):
        # Test mode not set (BOARD or LS2K) exception
        with self.assertRaises(RuntimeError) as e:
            GPIO.setup(LED_PIN, GPIO.OUT)
        self.assertEqual(str(e.exception), 'Please set pin numbering mode using GPIO.setmode(GPIO.BOARD) or GPIO.setmode(GPIO.LS2K)')
        
        # Test trying to change mode after it has been set
        GPIO.setmode(GPIO.LS2K)
        with self.assertRaises(ValueError) as e:
            GPIO.setmode(GPIO.BOARD)
        GPIO.setup(LED_PIN_LS2K, GPIO.IN)
        GPIO.cleanup()



if __name__ == '__main__':
    # begin the unittest.main()
    unittest.main()

