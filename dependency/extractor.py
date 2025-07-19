import os
import tarfile
import zipfile
import shutil
  
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
    
    self._maybe_strip_top_dir(dest_folder)
    print(f"Extraction complete: {dest_folder}")

  def __extract_tar_gz(self, archive_path: str, dest_folder: str):
    with tarfile.open(archive_path, "r:gz") as tar:
      self._safe_extract_tar(tar, dest_folder)

  def __extract_zip(self, archive_path: str, dest_folder: str):
    with zipfile.ZipFile(archive_path, "r") as zip_ref:
      self._safe_extract_zip(zip_ref, dest_folder)

  def _safe_extract_tar(self, tar: tarfile.TarFile, dest_folder: str):
    for member in tar.getmembers():
      member_path = os.path.join(dest_folder, member.name)
      if not self._is_within_directory(dest_folder, member_path):
        raise Exception("Attempted Path Traversal in Tar File")
    tar.extractall(dest_folder)

  def _safe_extract_zip(self, zip_ref: zipfile.ZipFile, dest_folder: str):
    for member in zip_ref.namelist():
      member_path = os.path.join(dest_folder, member)
      if not self._is_within_directory(dest_folder, member_path):
        raise Exception("Attempted Path Traversal in Zip File")
    zip_ref.extractall(dest_folder)

  def _is_within_directory(self, directory: str, target: str) -> bool:
    abs_directory = os.path.abspath(directory)
    abs_target = os.path.abspath(target)
    return os.path.commonpath([abs_directory]) == os.path.commonpath([abs_directory, abs_target])

  def _maybe_strip_top_dir(self, dest_folder: str):
    entries = os.listdir(dest_folder)
    if len(entries) != 1:
      return
    only_entry = os.path.join(dest_folder, entries[0])
    if not os.path.isdir(only_entry):
      return
    for name in os.listdir(only_entry):
      shutil.move(os.path.join(only_entry, name), dest_folder)
    os.rmdir(only_entry)
