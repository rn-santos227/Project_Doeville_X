import tarfile
import zipfile
import os
import subprocess
  
class TarGzExtractor:
  def extract(self, archive_path: str, dest_folder: str):
    print(f"Extracting {archive_path} to {dest_folder}...")

    if archive_path.endswith(".tar.gz") or archive_path.endswith(".tgz"):
      with tarfile.open(archive_path, "r:gz") as tar:
        tar.extractall(dest_folder)

    elif archive_path.endswith(".zip"):
      with zipfile.ZipFile(archive_path, "r") as zip_ref:
        zip_ref.extractall(dest_folder)