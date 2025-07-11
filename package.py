import os
import json

from dependency.downloader import HTTPDownloader
from dependency.extractor import TarGzExtractor
from dependency.manager import DependencyManager
from dependency.utils import ensure_folder_exists, load_packages, remove_file

def main():
  root_dir = os.path.abspath(os.path.dirname(__file__))
  lib_dir = "lib"
  ensure_folder_exists(lib_dir)

  packages_file = os.path.join(root_dir, "packages.json")
  packages = load_packages(packages_file)

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
      remove_file(archive_path)

    except Exception as e:
      print(f"Error processing {package_name}: {e}")
      continue

if __name__ == "__main__":
  main()
  