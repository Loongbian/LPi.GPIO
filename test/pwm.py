import LPi.GPIO as GPIO
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(18, GPIO.OUT)

p = GPIO.PWM(18, 1000)  # 18端口设置成1000Hz
p.start(10)  #开启pwm

input("continue:")  #随意按下一个键继续执行
p.ChangeFrequency(2000)
p.ChangeDutyCycle(70)

input("exit:") #随意按下就退出
p.stop()  #停止pwm
GPIO.cleanup()
