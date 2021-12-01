import unittest
import LPi.GPIO as GPIO


class Test(unittest.TestCase):
    def runTest(self):
        GPIO.helloworld()

class TestInputOutput(unittest.TestCase):
    def setUp(self):
        GPIO.setmode(GPIO.BOARD)

    def test_outputread(self):
        """Test that an output() can be input()"""
        GPIO.setup(LED_PIN, GPIO.OUT)
        GPIO.output(LED_PIN, GPIO.HIGH)
        self.assertEqual(GPIO.input(LED_PIN), GPIO.HIGH)
        GPIO.output(LED_PIN, GPIO.LOW)
        self.assertEqual(GPIO.input(LED_PIN), GPIO.LOW)

    def test_output_on_input(self):
        """Test output() can not be done on input"""
        GPIO.setup(SWITCH_PIN, GPIO.IN)
        with self.assertRaises(RuntimeError):
            GPIO.output(SWITCH_PIN, GPIO.LOW)

    def tearDown(self):
        GPIO.cleanup()


if __name__ == '__main__':
    # begin the unittest.main()
    unittest.main()

