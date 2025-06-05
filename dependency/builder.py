import os
import subprocess

class Builder:
  def __init__(self, source_dir: str):
    self.source_dir = source_dir

  def build(self, install_prefix: str):
    print(f"Building {self.source_dir}...")
    os.makedirs(install_prefix, exist_ok=True)

    try:
      configure_script = os.path.join(self.source_dir, "configure")

    except subprocess.CalledProcessError as e:
      print(f"Build failed for {self.source_dir}: {e}")
      raise