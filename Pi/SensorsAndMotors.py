import os
import time
from adafruit_motorkit import MotorKit

motors_a = MotorKit()
motors_b = MotorKit()

def measure_temp():
    return os.popen("vcgencmd measure_temp").readline()

while True:
    print("1")
    print(measure_temp())
    time.sleep(1)