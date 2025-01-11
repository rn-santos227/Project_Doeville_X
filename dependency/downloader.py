import os
import subprocess

class IDownloader:
  def download(self, url: str, destination: str):
    raise NotImplementedError("download method must be implemented")