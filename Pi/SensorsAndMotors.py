import os
import time
from adafruit_motorkit import MotorKit
from adadfruit_motor import stepper

motors_a = MotorKit()
motors_b = MotorKit()

def measure_temp():
    return os.popen("vcgencmd measure_temp").readline()

motors_a.motor1.throttle = 0.2
while True:
    print("1")
    print(measure_temp())
    motors_a.stepper2.onestep(style=stepper.MICROSTEP)
    time.sleep(1)