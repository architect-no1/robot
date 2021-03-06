#!/usr/bin/env python

from importlib import import_module
import os, sys
from flask import Flask, render_template, Response, redirect, url_for
from camera_pipe import PipeCamera, pipeName

app = Flask(__name__, static_url_path="/static",
                      static_folder="static",
                      template_folder="static")
pipeCamera = PipeCamera()

@app.route('/')
def index():
    # Main Page
    return redirect(url_for('static', filename='index.html'))
    #return app.send_static_file('index.html')

def gen(camera):
    # Camera streaming generator function
    while True:
        frame = camera.get_frame()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/camera')
def video_feed():
    # Camera streaming url. This URL is used in webpage's image tag
    return Response(gen(pipeCamera),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
  app.run(host='0.0.0.0', port=80, threaded=True)
