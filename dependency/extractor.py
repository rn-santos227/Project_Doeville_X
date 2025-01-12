import tarfile
import zipfile
import os
import subprocess
  
class TarGzExtractor:
  def extract(self, archive: str, destination: str):
    print(f"Extracting {archive} to {destination}...")