import os
import shutil
import subprocess

CERT_BUNDLE = os.path.join(os.path.dirname(__file__), "cacert.pem")

try:
  import certifi
except ImportError:
  certifi = None

class HTTPDownloader:
  def download(self, url: str, destination: str):
    print(f"Downloading {url} to {destination}...")
    os.makedirs(os.path.dirname(destination), exist_ok=True)

    try:
      if not url.startswith("https://"):
        raise ValueError("Insecure URL: only HTTPS is supported")
      
      curl = shutil.which("curl")
      if not curl:
        raise RuntimeError("curl not found")

      cafile = None
      if certifi:
        cafile = certifi.where()
      elif os.path.exists(CERT_BUNDLE):
        cafile = CERT_BUNDLE

      cmd = [
        curl,
        "--fail",
        "--location",
        "--proto", "=https",
        "--tlsv1.2",
        "--output", destination,
        url,
      ]

      if cafile:
        cmd.extend(["--cacert", cafile])

      print(f"Running: {' '.join(cmd)}")
      subprocess.run(cmd, check=True)
      print(f"Downloaded: {destination}")

    except Exception as e:
      print(f"Error downloading {url}: {e}")
      if os.path.exists(destination):
        os.remove(destination)
      raise e
