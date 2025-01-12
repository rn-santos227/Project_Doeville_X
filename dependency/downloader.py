import os
import urllib.request

class HTTPDownloader:
  def download(self, url: str, destination: str):
    print(f"Downloading {url} to {destination}...")
    command = f"curl -L {url} -o {destination}"