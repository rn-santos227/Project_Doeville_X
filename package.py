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

  packages_file = os.path.join(root_dir, "packages.json")

  if not os.path.exists(packages_file):
    print("Error: packages.json not found.")
    return

  with open(packages_file, "r") as file:
    packages = json.load(file)

  downloader = HTTPDownloader()
  extractor = TarGzExtractor()
  manager = DependencyManager(downloader, extractor)

  for package_name, package_info in packages.items():
    print(f"Processing package: {package_name}...")

    archive_path = os.path.join(lib_dir, package_info["archive"])
    source_dir = os.path.join(lib_dir, package_info["source_dir"])
    output_dir = os.path.join(lib_dir, package_info["output_dir"])

    try:
      manager.process_dependency(
        url=package_info["url"],
        archive_path=archive_path,
        source_dir=source_dir,
        output_dir=output_dir,
      )

    except Exception as e:
      print(f"Error processing {package_name}: {e}")
      continue

  if __name__ == "__main__":
    pass