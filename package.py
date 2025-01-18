import os
import json
from dependency.downloader import HTTPDownloader
from dependency.extractor import TarGzExtractor
from dependency.utils import ensure_folder_exists
from dependency.manager import DependencyManager

def load_packages(package_file):
  
  if not os.path.exists(package_file):
    raise FileNotFoundError(f"Package file not found: {package_file}")
  
  with open(package_file, "r") as file:
    return json.load(file)

def main():
  package_file = "packages.json"
  lib_dir = "lib"