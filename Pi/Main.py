#!/usr/bin/python3

from flask import Flask, render_template, request, jsonify
from datetime import datetime
from smbus2 import SMBusWrapper


addrArduino = 0x60

app = Flask(__name__)
#host = socket.gethostbyname(socket.gethostname())
host = '10.14.174.55'  # need to get this automagically
#host = '192.168.0.68'

drill = 0

PrevUserInput =\
{
    "MODE" : 0,
    "FWD" : 0,
    "TURN" : 0,

    "FL" : 0,
    "FR" : 0,
    "BL" : 0,
    "BR" : 0,

    "RPM" : 0
}

@app.route("/")
def index():
    t = datetime.now()
    return render_template('index.html', time=t, host=host)


@app.route("/update", methods=['POST'])
def update():
    global PrevUserInput
    global drill

    ui = request.form.to_dict()
    for k,v in ui.items():
        ui[k] = int(float(v) )
    #print(ui)
    if ui != PrevUserInput: # i.e. something has changed
        if ui["MODE"] == 1 and PrevUserInput["MODE"] == 0: # start button pressed
            if drill == 1: drill = 0
            else: drill = 1
        cmd  = [ 0, 0, 0, 0, ui["FL"], ui["FR"], ui["BL"], ui["BR"] ]
        if ui["FWD"] == 1:
            cmd[0] = ui["RPM"]
            cmd[1] = ui["RPM"]
            cmd[2] = 1
            cmd[3] = 1
        elif ui["FWD"] == -1:
            cmd[0] = ui["RPM"]
            cmd[1] = ui["RPM"]
            cmd[2] = 0
            cmd[3] = 0
        elif ui["TURN"] == 1:
            cmd[0] = ui["RPM"]
            cmd[1] = ui["RPM"]
            cmd[2] = 1
            cmd[3] = 0
        elif ui["TURN"] == -1:
            cmd[0] = ui["RPM"]
            cmd[1] = ui["RPM"]
            cmd[2] = 0
            cmd[3] = 1

        PrevUserInput = ui

        with SMBusWrapper(1) as bus:
            bus.write_i2c_block_data(addrArduino, drill, cmd)

    t = datetime.now()
    return jsonify({ 'result' : 'success', 'time' : t })


#if __name__ == "__main__":
print("Host: %s" % host)
app.run(host=host, port=5000, debug=False)
