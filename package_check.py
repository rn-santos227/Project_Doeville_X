import json
import os

BASE_DIR = os.path.join(os.path.dirname(__file__), "lib")
PACKAGES_FILE = os.path.join(os.path.dirname(__file__), "packages.json")

def load_packages(path):
  with open(path, "r") as f:
    return json.load(f)

def check_installed(packages):
  results = {}
  for name, info in packages.items():
    out_dir = info.get("output_dir") or info.get("source_dir")
    path = os.path.join(BASE_DIR, out_dir)
    results[name] = os.path.exists(path)
  return results

def main():
  packages = load_packages(PACKAGES_FILE)
  results = check_installed(packages)
  for name, ok in results.items():
    status = "OK" if ok else "MISSING"
    print(f"[{status}] {name}")
