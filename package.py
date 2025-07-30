import os

from dependency.downloader import HTTPDownloader
from dependency.extractor import TarGzExtractor
from dependency.manager import DependencyManager
from dependency.utils import ensure_folder_exists, load_packages, remove_file
from package_check import check_installed

def main():
  root_dir = os.path.abspath(os.path.dirname(__file__))
  lib_dir = "lib"
  ensure_folder_exists(lib_dir)

  packages_file = os.path.join(root_dir, "packages.json")
  packages = load_packages(packages_file)

  downloader = HTTPDownloader()
  extractor = TarGzExtractor()
  manager = DependencyManager(downloader, extractor)
  build_env = os.environ.copy()

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
        env=build_env,
        sha256=package_info.get("sha256"),
      )
      remove_file(archive_path)

      if package_name == "SDL2":
        sdl2_config = os.path.join(output_dir, "bin", "sdl2-config")
        if os.path.isfile(sdl2_config):
          build_env["SDL2_CONFIG"] = os.path.abspath(sdl2_config)
          prefix_bin = os.path.dirname(os.path.abspath(sdl2_config))
          build_env["PATH"] = os.pathsep.join([
            prefix_bin,
            build_env.get("PATH", ""),
          ])

        pkg_dirs = [
          os.path.join(output_dir, "lib", "pkgconfig"),
          os.path.join(output_dir, "lib64", "pkgconfig"),
        ]
        for pkg_dir in pkg_dirs:
          if os.path.isdir(pkg_dir):
            build_env["PKG_CONFIG_PATH"] = os.pathsep.join([
              os.path.abspath(pkg_dir),
              build_env.get("PKG_CONFIG_PATH", ""),
            ])
            break

        prefix_abs = os.path.abspath(output_dir)
        build_env["CMAKE_PREFIX_PATH"] = os.pathsep.join([
          prefix_abs,
          build_env.get("CMAKE_PREFIX_PATH", ""),
        ])
        build_env["CFLAGS"] = "-I" + os.path.join(prefix_abs, "include") + " " + build_env.get("CFLAGS", "")
        build_env["LDFLAGS"] = "-L" + os.path.join(prefix_abs, "lib") + " " + build_env.get("LDFLAGS", "")

    except Exception as e:
      print(f"Error processing {package_name}: {e}")
      continue

  results = check_installed(packages)
  for name, ok in results.items():
    status = "OK" if ok else "MISSING"
    print(f"[{status}] {name}")

if __name__ == "__main__":
  main()
  