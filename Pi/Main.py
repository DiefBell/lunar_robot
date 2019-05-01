#!/usr/bin/python3

from flask import Flask, render_template, request, jsonify
from datetime import datetime
from smbus2 import SMBusWrapper
import time

millis = lambda: int(round(time.time() * 1000))


addrArduino = 0x60

app = Flask(__name__)
#host = socket.gethostbyname(socket.gethostname())
host = '10.14.173.224'  # need to get this automagically
#host = '192.168.0.68'

t_prevArduinoUpdate = millis

mode = 0
toggleMotors = 1

PrevUserInput =\
{
    "MODE" : 0,
    "TOGGLE" : 0,
    "FWD" : 0,
    "TURN" : 0,

    "FL" : 0,
    "FR" : 0,
    "BL" : 0,
    "BR" : 0,

    "RPM" : 0,

    "DEF_S_CAR" : 90, # default servo positions
    "DEF_S_COL" : 90
}

@app.route("/")
def index():
    t = datetime.now()
    return render_template('index.html', time=t, host=host)


@app.route("/update", methods=['POST'])
def update():
    global PrevUserInput
    global mode
    global toggleMotors
    global t_prevArduinoUpdate

    arduinoUpdateInterval = 400 # maximum of 400ms between updates to the Arduino

    ui = request.form.to_dict()
    for k,v in ui.items():
        if k == "RPM":
            ui[k] = int(10 * float(v))
        else:
            ui[k] = int(v)
    #print(ui)
    if ui != PrevUserInput or t_prevArduinoUpdate - millis > arduinoUpdateInterval: # i.e. something has changed
        if ui["MODE"] == 1 and PrevUserInput["MODE"] == 0: # start button pressed
            if mode == 1: mode = 0
            else: mode = 1
        if ui["TOGGLE"] == 1 and PrevUserInput["TOGGLE"] == 0: # select button pressed
            if toggleMotors == 1: toggleMotors = 0
            else: toggleMotors = 1
        cmd  = [ 0, # motor speed
                 0, 0, # motor directions or servo positions
                 ui["FL"], ui["FR"], ui["BL"], ui["BR"] ] # USRF limits
        if mode == 1:
            if ui["FWD"] == 1:
                cmd[0] = ui["RPM"]
                cmd[1] = toggleMotors
                cmd[2] = toggleMotors
            elif ui["FWD"] == -1:
                cmd[0] = ui["RPM"]
                cmd[1] = 1 - toggleMotors
                cmd[2] = 1 - toggleMotors
            elif ui["TURN"] == 1:
                cmd[0] = ui["RPM"]
                cmd[1] = toggleMotors
                cmd[2] = 1 - toggleMotors
            elif ui["TURN"] == -1:
                cmd[0] = ui["RPM"]
                cmd[1] = 1 - toggleMotors
                cmd[2] = toggleMotors
        else:
            cmd[1] = ui["DEF_S_CAR"]
            cmd[2] = ui["DEF_S_COL"]

        PrevUserInput = ui

        with SMBusWrapper(1) as bus:
            bus.write_i2c_block_data(addrArduino, mode, cmd)

    t = datetime.now()
    return jsonify({ 'result' : 'success', 'time' : t })


#if __name__ == "__main__":
print("Host: %s" % host)
app.run(host=host, port=5000, debug=False)
