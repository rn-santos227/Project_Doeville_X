import os
import hashlib
import shutil
import subprocess

CERT_BUNDLE = os.path.join(os.path.dirname(__file__), "cacert.pem")

class HTTPDownloader:
  def download(self, url: str, destination: str, expected_sha256: str = None):
    print(f"Downloading {url} to {destination}...")
    os.makedirs(os.path.dirname(destination), exist_ok=True)

    try:
      if not url.startswith("https://"):
        raise ValueError("Insecure URL: only HTTPS is supported")
      
      curl = shutil.which("curl")
      if not curl:
        raise RuntimeError("curl not found")

      cmd = [
        curl,
        "--fail",
        "--location",
        "--proto", "=https",
        "--tlsv1.2",
        "--output", destination,
        url,
      ]

      print(f"Running: {' '.join(cmd)}")
      subprocess.run(cmd, check=True)
      print(f"Downloaded: {destination}")

      if expected_sha256:
        actual = self._compute_sha256(destination)
        if actual.lower() != expected_sha256.lower():
          os.remove(destination)
          raise RuntimeError(
            f"SHA-256 mismatch for {destination}: expected {expected_sha256} got {actual}"
          )
        print(f"Verified SHA-256: {actual}")

    except Exception as e:
      print(f"Error downloading {url}: {e}")
      if os.path.exists(destination):
        os.remove(destination)
      raise e

  def _compute_sha256(self, path: str) -> str:
    hash_obj = hashlib.sha256()
    with open(path, "rb") as f:
      for chunk in iter(lambda: f.read(8192), b""):
        hash_obj.update(chunk)
    return hash_obj.hexdigest()    
