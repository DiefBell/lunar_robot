from flask import Flask, render_template, request, jsonify
from datetime import datetime
#import socket
from smbus2 import SMBusWrapper


addrArduino = 0x60

app = Flask(__name__)
#host = socket.gethostbyname(socket.gethostname())
host = '192.168.0.68'  # need to get this automagically

sensors = []
drill = 0

SPEED = 10

UserInput =\
{
    "FACE_1" : 0,
    "FACE_2" : 0,
    "FACE_3" : 0,
    "FACE_4" : 0,
    "LEFT_TOP_SHOULDER" : 0,
    "RIGHT_TOP_SHOULDER" : 0,
    "LEFT_BOTTOM_SHOULDER" : 0,
    "RIGHT_BOTTOM_SHOULDER" : 0,
    "SELECT_BACK" : 0,
    "START_FORWARD" : 0,
    "LEFT_STICK" : 0,
    "RIGHT_STICK" : 0,
    "DPAD_UP" : 0,
    "DPAD_DOWN" : 0,
    "DPAD_LEFT" : 0,
    "DPAD_RIGHT" : 0,
    "HOME" : 0,
    "LEFT_STICK_X" : 0,
    "LEFT_STICK_Y" : 0,
    "RIGHT_STICK_X" : 0,
    "RIGHT_STICK_Y" : 0
}

@app.route("/")
def index():
    t = datetime.now()
    return render_template('index.html', time=t, host=host)


@app.route("/update", methods=['POST'])
def update():
    global UserInput
    global sensors
    global drill
    global SPEED

    gamepad = {}
    for k,v in request.form.to_dict().items():
        if float(v) > 0: gamepad[k] = 1
        elif float(v) < 0: gamepad[k] = -1
        else: gamepad[k] = 0
    if gamepad != UserInput:  # i.e. something has changed
        if gamepad["START_FORWARD"] == 1 and UserInput["START_FORWARD"] == 0:  # when the start button is pressed, but ignores holds
            if drill == 0: drill = 1
            else: drill = 0
        
        cmd = [0, 0, 0, 0]
        if gamepad["DPAD_UP"] == 1 or gamepad["LEFT_STICK_Y"] == -1: cmd = [SPEED, SPEED, 0, 0]
        elif gamepad["DPAD_DOWN"] == 1 or gamepad["LEFT_STICK_Y"] == 1: cmd = [-SPEED, -SPEED, 0, 0]
        elif gamepad["DPAD_RIGHT"] == 1 or gamepad["LEFT_STICK_X"] == 1: cmd = [SPEED, -SPEED, 0, 0]
        elif gamepad["DPAD_LEFT"] == 1 or gamepad["LEFT_STICK_X"] == -1: cmd = [-SPEED, SPEED, 0, 0]
    
        with SMBusWrapper(1) as bus:
            bus.write_i2c_block_data(addrArduino, drill, cmd)

        UserInput = gamepad

    t = datetime.now()
    return jsonify({ 'result' : 'success', 'time' : t })


#if __name__ == "__main__":
print("Host: %s" % host)
app.run(host=host, port=5000, debug=False)
