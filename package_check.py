import os
import argparse
import sys

from dependency.utils import load_packages

BASE_DIR = os.path.join(os.path.dirname(__file__), "lib")
PACKAGES_FILE = os.path.join(os.path.dirname(__file__), "packages.json")

def check_installed(packages):
  results = {}
  for name, info in packages.items():
    out_dir = info.get("output_dir")
    source_dir = info.get("source_dir")

    build_path = os.path.join(BASE_DIR, out_dir) if out_dir else None
    source_path = os.path.join(BASE_DIR, source_dir)

    build_exists = build_path and os.path.isdir(build_path) and any(os.scandir(build_path))
   
    results[name] = build_exists
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
