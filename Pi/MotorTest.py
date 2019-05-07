import time
from adafruit_motorkit import MotorKit
import _thread

kit = MotorKit()

recipSpeed = 0.5
lsSpeedMmps = 0.25 # lead screw speed in mm per second

MM_PER_ROT = 1
DEG_PER_STEP = 0.9

def runLeadScrew(s):
  d = DEG_PER_STEP / (s*360/MM_PER_ROT)
  while 1:
    kit.stepper2.onestep()
    time.sleep(d)

_thread.start_new_thread(lsSpeedMmps)
  
while 1:
  kit.motor1.throttle = motorSpeed
  kit.motor2.throttle = -motorSpeed
  time.sleep(1)
  
  kit.motor1.throttle = 0
  kit.motor2.throttle = 0
  time.sleep(0.2)
  
  kit.motor1.throttle = -motorSpeed
  kit.motor2.throttle = motorSpeed
  time.sleep(1)
  
  kit.motor1.throttle = 0
  kit.motor2.throttle = 0
  time.sleep(0.2)
