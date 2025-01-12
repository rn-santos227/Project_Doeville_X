import os
import urllib.request

class HTTPDownloader:
  def download(self, url: str, destination: str):
    print(f"Downloading {url} to {destination}...")
    with urllib.request.urlopen(url) as response, open(destination, "wb") as out_file:
      out_file.write(response.read())