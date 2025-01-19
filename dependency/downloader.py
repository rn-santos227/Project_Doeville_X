import os
import subprocess

class HTTPDownloader:
  def download(self, url: str, destination: str):
    print(f"Downloading {url} to {destination}...")

    os.makedirs(os.path.dirname(destination), exist_ok=True)

    try:
      print(f"Downloaded: {destination}")
      subprocess.run(
        ["curl", "-L", "-o", destination, url],
        check=True
      )

    except subprocess.CalledProcessError as e:
      print(f"Error downloading {url}: {e}")