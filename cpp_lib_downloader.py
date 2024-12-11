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