import os
import shutil
import subprocess

class Builder:
  def __init__(self, source_dir: str):
    self.source_dir = source_dir

  def build(self, install_prefix: str):
    print(f"Building {self.source_dir}...")
    os.makedirs(install_prefix, exist_ok=True)

    try:
      configure_script = os.path.join(self.source_dir, "configure")
      cmakelists = os.path.join(self.source_dir, "CMakeLists.txt")

      if os.path.isfile(configure_script):
        os.chmod(configure_script, 0o755)
        subprocess.run([configure_script, f"--prefix={install_prefix}"], cwd=self.source_dir, check=True)

        make_cmd = shutil.which("make")
        if os.name == "nt":
          make_cmd = shutil.which("mingw32-make") or shutil.which("nmake") or make_cmd
        
        if not make_cmd:
          raise RuntimeError("'make' not found in PATH")

        subprocess.run([make_cmd, "-j"], cwd=self.source_dir, check=True)
        subprocess.run([make_cmd, "install"], cwd=self.source_dir, check=True)

      elif os.path.isfile(cmakelists):
        cmake = shutil.which("cmake")
        if not cmake:
          raise RuntimeError("cmake executable not found")

        build_dir = os.path.join(self.source_dir, "build")
        os.makedirs(build_dir, exist_ok=True)

        cmake_cmd = shutil.which("cmake") or "cmake"
        if not shutil.which(cmake_cmd):
          raise RuntimeError("'cmake' not found in PATH")

        subprocess.run([
          cmake_cmd,
          "..",
          f"-DCMAKE_INSTALL_PREFIX={install_prefix}"
        ], cwd=build_dir, check=True)

        build_cmd = [cmake_cmd, "--build", ".", "--target", "install"]
        if os.name == "nt":
          build_cmd += ["--config", "Release"]
        subprocess.run(build_cmd, cwd=build_dir, check=True)

      else:
        raise RuntimeError(f"No build system found in {self.source_dir}")
      
      print(f"Build finished: {self.source_dir}")

    except subprocess.CalledProcessError as e:
      print(f"Build failed for {self.source_dir}: {e}")
      raise e
