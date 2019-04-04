from flask import Flask, render_template, request, jsonify
from datetime import datetime
import socket

app = Flask(__name__)
host = socket.gethostbyname(socket.gethostname())

InputMap =\
{
    "forward" : "38",
    "back" : "40",
    "left" : "37",
    "right" : "39"
}
UserInput =\
{
    "37" : False,  # left
    "38" : False,  # fwd
    "39" : False,  # right
    "40" : False  # back
}

@app.route("/")
def index():
    t = datetime.now()
    return render_template('index.html', time=t)


@app.route("/update", methods=['POST'])
def update():
    global UserInput
    UserInput = request.form
    print(UserInput)

    t = datetime.now()
    return jsonify({ 'result' : 'success', 'time' : t })


if __name__ == "__main__":
    print("Host: %s" % host)
    app.run(host=host, port=5000, debug=False)
