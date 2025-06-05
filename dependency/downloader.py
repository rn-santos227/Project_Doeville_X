import os
import subprocess

class HTTPDownloader:
  def download(self, url: str, destination: str):
    print(f"Downloading {url} to {destination}...")
    