import time
from adafruit_motorkit import MotorKit
from adafruit_motor import stepper
import _thread

kit = MotorKit()

recipSpeed = 0.9
lsSpeedMmps = 1 # lead screw speed in mm per second

MM_PER_ROT = 1
DEG_PER_STEP = 0.9

def runLeadScrew(s):
  d = DEG_PER_STEP / (s*360/MM_PER_ROT)
  for i in range(40000):
    kit.stepper2.onestep(direction=stepper.FORWARD)
    time.sleep(d)

_thread.start_new_thread(runLeadScrew, (lsSpeedMmps,) )


def runRecipMotors(speed, direction, runTime, pauseTime):
    kit.motor1.throttle = speed * direction
    kit.motor2.throttle = -speed * direction
    time.sleep(runTime)
    kit.motor1.throttle = 0
    kit.motor2.throttle = 0
    time.sleep(pauseTime)

runRecipMotors(recipSpeed, -1, 0.5, 1)
dr = 1

for i in range(200):
    runRecipMotors(recipSpeed, dr, 1, 0.5)
    dr = dr*-1

runRecipMotors(recipSpeed, 1, 0.5, 1)

