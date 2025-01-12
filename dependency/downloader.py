import os
import subprocess
import urllib.request

class IDownloader:
  def download(self, url: str, destination: str):
    raise NotImplementedError("download method must be implemented")
  
class HTTPDownloader(IDownloader):
  def download(self, url: str, destination: str):
    print(f"Downloading {url} to {destination}...")
    command = f"curl -L {url} -o {destination}"
    if subprocess.call(command, shell=True) != 0:
      raise RuntimeError(f"Failed to download {url}")