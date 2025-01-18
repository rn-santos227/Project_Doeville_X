import os
import json
from dependency.utils import ensure_folder_exists
from dependency.manager import DependencyManager

def load_packages(package_file):
  
  if not os.path.exists(package_file):
    raise FileNotFoundError(f"Package file not found: {package_file}")
