import time
from base_camera import BaseCamera

pipeName = "/home/pi/robot/bin/pipe"

class PipeCamera(BaseCamera):
  @staticmethod
  def frames():
    while True:
      with open(pipeName) as fifo:
        data = fifo.read()
        yield data
