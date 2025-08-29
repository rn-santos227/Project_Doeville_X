import os
import json
import shutil

def ensure_folder_exists(folder_path):
  if not os.path.exists(folder_path):
    os.makedirs(folder_path)

def archive_exists(file_path):
  return os.path.exists(file_path)

def load_packages(package_file):
  if not os.path.exists(package_file):
    raise FileNotFoundError(f"Package file not found: {package_file}")
  
  with open(package_file, "r") as file:
    try:
      return json.load(file)
    except json.JSONDecodeError as e:
      raise ValueError(f"Invalid package file {package_file}: {e}")
  
def remove_file(filepath):
  try:
    if os.path.exists(filepath):
      os.remove(filepath)
      print(f"Removed file: {filepath}")

  except OSError as e:
    print(f"Error removing file {filepath}: {e}")

def remove_dir(dirpath):
  """Remove a directory tree if it exists."""
  try:
    if os.path.isdir(dirpath):
      shutil.rmtree(dirpath)
      print(f"Removed directory: {dirpath}")
  except OSError as e:
    print(f"Error removing directory {dirpath}: {e}")
