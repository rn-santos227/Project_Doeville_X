import tarfile
import zipfile
import os
import subprocess

class IExtractor:
  def extract(self, archive: str, destination: str):
    raise NotImplementedError("extract method must be implemented")
  
class TarGzExtractor(IExtractor):
  def extract(self, archive: str, destination: str):
    print(f"Extracting {archive} to {destination}...")
    command = f"tar -xzf {archive} -C {destination}"
    if subprocess.call(command, shell=True) != 0:
      raise RuntimeError(f"Failed to extract {archive}")