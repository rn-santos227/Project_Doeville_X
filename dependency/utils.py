import os
import json

def ensure_folder_exists(folder_path):
  if not os.path.exists(folder_path):
    os.makedirs(folder_path)

def archive_exists(file_path):
  return os.path.exists(file_path)

def load_packages(package_file):
  if not os.path.exists(package_file):
    raise FileNotFoundError(f"Package file not found: {package_file}")
  
  with open(package_file, "r") as file:
    return json.load(file)