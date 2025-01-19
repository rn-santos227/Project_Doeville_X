import os
import json

from dependency.downloader import HTTPDownloader
from dependency.extractor import TarGzExtractor
from dependency.manager import DependencyManager
from dependency.utils import ensure_folder_exists

def load_packages(package_file):
  
  if not os.path.exists(package_file):
    raise FileNotFoundError(f"Package file not found: {package_file}")
  
  with open(package_file, "r") as file:
    return json.load(file)

def main():
  root_dir = os.path.abspath(os.path.dirname(__file__))
  lib_dir = "lib"
  ensure_folder_exists(lib_dir)

  downloader = HTTPDownloader()
  extractor = TarGzExtractor()