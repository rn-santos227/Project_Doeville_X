import os
import shutil
import subprocess

class Builder:
  def __init__(self, source_dir: str):
    self.source_dir = source_dir

  def build(self, install_prefix: str, env=None):
    print(f"Building {self.source_dir}...")
    install_prefix = os.path.abspath(install_prefix)
    os.makedirs(install_prefix, exist_ok=True)

    if env is None:
      env = os.environ.copy()

    try:
      configure_script = os.path.join(os.path.abspath(self.source_dir), "configure")
      cmakelists = os.path.join(self.source_dir, "CMakeLists.txt")
      meson_file = os.path.join(self.source_dir, "meson.build")
      makefile = os.path.join(self.source_dir, "Makefile")

      if os.path.isfile(configure_script):
        os.chmod(configure_script, 0o755)
        subprocess.run([configure_script, f"--prefix={install_prefix}"], cwd=self.source_dir, check=True, env=env)

        make_cmd = shutil.which("make", path=env.get("PATH"))
        if os.name == "nt":
          make_cmd = shutil.which("mingw32-make", path=env.get("PATH")) or \
                    shutil.which("nmake", path=env.get("PATH")) or make_cmd
        
        if not make_cmd:
          raise RuntimeError("'make' not found in PATH")

        subprocess.run([make_cmd, "-j"], cwd=self.source_dir, check=True, env=env)
        subprocess.run([make_cmd, "install"], cwd=self.source_dir, check=True, env=env)

      elif os.path.isfile(cmakelists):
        cmake = shutil.which("cmake", path=env.get("PATH"))
        if not cmake:
          raise RuntimeError("cmake executable not found")

        build_dir = os.path.join(self.source_dir, "build")
        os.makedirs(build_dir, exist_ok=True)

        cmake_cmd = shutil.which("cmake", path=env.get("PATH")) or "cmake"
        if not shutil.which(cmake_cmd, path=env.get("PATH")):
          raise RuntimeError("'cmake' not found in PATH")

        subprocess.run([
          cmake_cmd,
          "..",
          f"-DCMAKE_INSTALL_PREFIX={install_prefix}"
        ], cwd=build_dir, check=True, env=env)

        build_cmd = [cmake_cmd, "--build", ".", "--target", "install"]
        if os.name == "nt":
          build_cmd += ["--config", "Release"]
          subprocess.run(build_cmd, cwd=build_dir, check=True)

      elif os.path.isfile(meson_file):
        meson = shutil.which("meson", path=env.get("PATH"))
        if not meson:
          raise RuntimeError("'meson' not found in PATH")

        build_dir = os.path.join(self.source_dir, "build")
        os.makedirs(build_dir, exist_ok=True)

        subprocess.run([
          meson,
          "setup",
          build_dir,
          f"--prefix={install_prefix}",
        ], cwd=self.source_dir, check=True, env=env)

        subprocess.run([meson, "compile", "-C", build_dir], check=True, env=env)
        subprocess.run([meson, "install", "-C", build_dir], check=True, env=env)

      elif os.path.isfile(makefile):
        make_cmd = shutil.which("make", path=env.get("PATH"))
        if not make_cmd:
          raise RuntimeError("'make' not found in PATH")

        is_lua = os.path.exists(os.path.join(self.source_dir, "src", "luaconf.h"))

        if is_lua:
          subprocess.run([make_cmd, "linux"], cwd=self.source_dir, check=True, env=env)
          subprocess.run([
            make_cmd,
            "install",
            f"INSTALL_TOP={install_prefix}",
          ], cwd=self.source_dir, check=True, env=env)
        else:
          subprocess.run([make_cmd], cwd=self.source_dir, check=True, env=env)
          subprocess.run([
            make_cmd,
            "install",
            f"PREFIX={install_prefix}",
          ], cwd=self.source_dir, check=True, env=env)

      else:
        raise RuntimeError(f"No build system found in {self.source_dir}")
      
      print(f"Build finished: {self.source_dir}")

    except subprocess.CalledProcessError as e:
      print(f"Build failed for {self.source_dir}: {e}")
      raise e
