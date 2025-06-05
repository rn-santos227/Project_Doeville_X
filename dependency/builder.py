import os
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
        subprocess.run(["chmod", "+x", configure_script], check=True)
        subprocess.run([configure_script, f"--prefix={install_prefix}"], cwd=self.source_dir, check=True)
        subprocess.run(["make", "-j"], cwd=self.source_dir, check=True)
        subprocess.run(["make", "install"], cwd=self.source_dir, check=True)

      elif os.path.isfile(cmakelists):
        build_dir = os.path.join(self.source_dir, "build")
        os.makedirs(build_dir, exist_ok=True)
        subprocess.run([
          "cmake",
          "..",
          f"-DCMAKE_INSTALL_PREFIX={install_prefix}"
        ], cwd=build_dir, check=True)
        subprocess.run(["cmake", "--build", ".", "--target", "install"], cwd=build_dir, check=True)

      else:
        raise RuntimeError(f"No build system found in {self.source_dir}")
      
      print(f"Build finished: {self.source_dir}")

    except subprocess.CalledProcessError as e:
      print(f"Build failed for {self.source_dir}: {e}")
      raise