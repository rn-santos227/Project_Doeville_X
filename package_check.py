import json
import os
import argparse
import sys

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
  parser = argparse.ArgumentParser(description="Check installed packages")
  parser.add_argument(
    "--fail-on-missing",
    action="store_true",
    help="Return non-zero exit code if any package is missing",
  )
  args = parser.parse_args()

  packages = load_packages(PACKAGES_FILE)
  results = check_installed(packages)
  missing = False

  for name, ok in results.items():
    status = "OK" if ok else "MISSING"
    if not ok:
      missing = True
    print(f"[{status}] {name}")

  if args.fail_on_missing and missing:
    sys.exit(1)

if __name__ == "__main__":
  main()
