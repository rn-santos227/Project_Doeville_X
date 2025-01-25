import os
import tarfile
import zipfile
import shutil
  
class TarGzExtractor:
  def extract(self, archive_path: str, dest_folder: str):
    print(f"Extracting {archive_path} to {dest_folder}...")

    temp_folder = dest_folder + "_temp"
    os.makedirs(dest_folder, exist_ok=True)

    if archive_path.endswith((".tar.gz", ".tgz")):
      self.__extract_tar_gz(archive_path, dest_folder)

    elif archive_path.endswith(".zip"):
      self.__extract_zip(archive_path, dest_folder)

    else:
      raise ValueError(f"Unsupported archive format: {archive_path}")
    
    self.__move_to_original_name(temp_folder, dest_folder)
    print(f"Extraction complete: {dest_folder}")

  def __extract_tar_gz(self, archive_path: str, dest_folder: str):
    with tarfile.open(archive_path, "r:gz") as tar:
      tar.extractall(dest_folder)

  def __extract_zip(self, archive_path: str, dest_folder: str):
    with zipfile.ZipFile(archive_path, "r") as zip_ref:
      zip_ref.extractall(dest_folder)

  def __move_to_original_name(self, temp_folder: str, dest_folder: str):
    extracted_items = os.listdir(temp_folder)

    if len(extracted_items) == 1 and os.path.isdir(os.path.join(temp_folder, extracted_items[0])):
      extracted_folder = os.path.join(temp_folder, extracted_items[0])
      shutil.move(extracted_folder, dest_folder)

    else:
      os.makedirs(dest_folder, exist_ok=True)
      for item in extracted_items:
        shutil.move(os.path.join(temp_folder, item), dest_folder)

    shutil.rmtree(temp_folder)