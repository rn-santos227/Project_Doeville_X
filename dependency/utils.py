import os

def ensure_folder_exists(folder_path):
  if not os.path.exists(folder_path):
    os.makedirs(folder_path)

def archive_exists(file_path):
  return os.path.exists(file_path)