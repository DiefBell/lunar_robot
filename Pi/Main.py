from flask import Flask, render_template
from datetime import datetime

app = Flask(__name__)


@app.route("/")
def index():
    t = datetime.now()
    return render_template('index.html', time=t)


if __name__ == "__main__":
    app.run()
