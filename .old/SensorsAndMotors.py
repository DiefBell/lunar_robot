import os
import time
import adafruit_motorkit
import adafruit_motor
from adafruit_motorkit import MotorKit
from adafruit_motor import stepper
from sense_hat import SenseHat

def measure_temp():
    return os.popen("vcgencmd measure_temp").readline().replace("temp=","")

#GPIO Settings
GPIO.setmode(GPIO.BOARD)

# Set up sense hat
sense = SenseHat()
sense.clear()
sense.set_imu_config(True, True, True)
sense.show_message("Hellow world!")

# Set up motors, start one of them
#motors_a = MotorKit()
#motors_b = MotorKit(address=0x61)
#motors_a.motor1.throttle = 0.2

while True:
    # Environmental Sensors
    print("Core temperature: %s" % measure_temp())
    humidity = sense.get_humidity()
    print("Humidity: %s %%" % humidity)
    temp = sense.get_temperature()
    print("Ambient Temperature: %s C" % sense.temp)
    pressure = sense.get_pressure()
    print("Pressure: %s Millibars" % pressure)

    # Orientation
    o = sense.get_orientation()
    pitch = o["pitch"]
    roll = o["roll"]
    yaw = o["yaw"]
    print("p: {0}, r: {1}, y: {2}".format(pitch, roll, yaw) )
    
    # Gyroscope
    acc = sense.get_gyroscope_raw()
    acc_x = acc["x"]
    acc_y = acc["y"]
    acc_z = acc["z"]
    print("x: {0}, y: {1}, z: {2}".format(acc_x, acc_y, acc_z) )

    # Locomotion
    #motors_a.stepper2.onestep(style=stepper.MICROSTEP)
    
    time.sleep(1)
