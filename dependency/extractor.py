import tarfile
import zipfile
import os
import subprocess
  
class TarGzExtractor:
  def extract(self, archive_path: str, dest_folder: str):
    print(f"Extracting {archive_path} to {dest_folder}...")

    os.makedirs(dest_folder, exist_ok=True)

    if archive_path.endswith((".tar.gz", ".tgz")):
      self.__extract_tar_gz(archive_path, dest_folder)

    elif archive_path.endswith(".zip"):
      self.__extract_zip(archive_path, dest_folder)

    else:
      raise ValueError(f"Unsupported archive format: {archive_path}")
    
    print(f"Extraction complete: {dest_folder}")

  def __extract_tar_gz(self, archive_path: str, dest_folder: str):
    with tarfile.open(archive_path, "r:gz") as tar:
      tar.extractall(dest_folder)

  def __extract_zip(self, archive_path: str, dest_folder: str):
    with zipfile.ZipFile(archive_path, "r") as zip_ref:
      zip_ref.extractall(dest_folder)