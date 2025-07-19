import os
import ssl
import shutil
import urllib.request

class HTTPDownloader:
  def download(self, url: str, destination: str):
    print(f"Downloading {url} to {destination}...")
    os.makedirs(os.path.dirname(destination), exist_ok=True)

    try:
      if not url.startswith("https://"):
        raise ValueError("Insecure URL: only HTTPS is supported")
  
      context = ssl.create_default_context()
      with urllib.request.urlopen(url, context=context) as response:
        with open(destination, "wb") as out_file:
          shutil.copyfileobj(response, out_file)
      print(f"Downloaded: {destination}")

    except Exception as e:
      print(f"Error downloading {url}: {e}")
      if os.path.exists(destination):
        os.remove(destination)
      raise e
