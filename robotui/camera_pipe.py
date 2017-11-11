import time
from base_camera import BaseCamera


class PipeCamera(BaseCamera):
  def __init__(self, pipeName):
    self.pipeName = pipeName    

  @staticmethod
  def frames():
    while True:
      with open(pipeName) as fifo:
        data = fifo.read()
        yield data
