import os
import urllib.request
import tarfile
import zipfile

dependencies = {
  "SDL2": "https://www.libsdl.org/release/SDL2-2.26.5.tar.gz",
  "SDL2_ttf": "https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.20.1.tar.gz",
  "Lua": "https://www.lua.org/ftp/lua-5.4.6.tar.gz"
}

LIB_DIR = "lib"

def download_file(name, url, dest):
  print(f"Downloading {name} from {url}...")
  file_path = os.path.join(dest, os.path.basename(url))
  
  with urllib.request.urlopen(url) as response, open(file_path, 'wb') as out_file:
    out_file.write(response.read())
  
  print(f"{name} downloaded to {file_path}")
  return file_path

def extract_file(file_path, dest):
  print(f"Extracting {file_path} to {dest}...")
  if file_path.endswith(".tar.gz") or file_path.endswith(".tgz"):
    with tarfile.open(file_path, "r:gz") as tar:
      tar.extractall(dest)
  
  elif file_path.endswith(".zip"):
    with zipfile.ZipFile(file_path, "r") as zip_ref:
      zip_ref.extractall(dest)
  
  print(f"Extraction complete: {dest}")
  os.remove(file_path)
  print(f"Removed compressed file: {file_path}")

def setup_dependencies():
  if not os.path.exists(LIB_DIR):
    os.makedirs(LIB_DIR)

  for name, url in dependencies.items():
    dest = os.path.join(LIB_DIR, name)
    if not os.path.exists(dest):
      os.makedirs(dest)
    file_path = download_file(name, url, dest)
    extract_file(file_path, dest)

if __name__ == "__main__":
  setup_dependencies()