import json
import os

BASE_DIR = os.path.join(os.path.dirname(__file__), "lib")
PACKAGES_FILE = os.path.join(os.path.dirname(__file__), "packages.json")

def load_packages(path):
  with open(path, "r") as f:
    return json.load(f)


