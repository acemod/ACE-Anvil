# flask --app test run

from flask import Flask, request
import math

app = Flask(__name__)

@app.route("/")
def hello_world():
    return "use api path"

@app.route("/test")
def test():
    x = request.args.get('x', type=float)
    y = request.args.get('y', type=float)
    q = request.args.get('q', type=float)

    distEast = x - 160
    distNorth = y - 100
    dist2d = math.sqrt(math.pow(distEast,2) + math.pow(distNorth,2))
    response = f"{distEast:.4f},{distNorth:.4f},{dist2d:.4f}"

    print(f"                                                                                                                 input:    {x}, {y}, {q}")
    print(f"                                                                                                                 output:   {response}")
    return response
