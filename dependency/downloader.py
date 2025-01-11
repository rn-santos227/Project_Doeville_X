import os
import subprocess

class IDownloader:
  def download(self, url: str, destination: str):
    raise NotImplementedError("download method must be implemented")
  
class HTTPDownloader(IDownloader):
  def download(self, url: str, destination: str):
    print(f"Downloading {url} to {destination}...")
    command = f"curl -L {url} -o {destination}"