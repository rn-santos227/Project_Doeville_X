import os
import subprocess
import shutil
import sys

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

      if os.path.isfile(configure_script) and not (os.name == "nt" and os.path.isfile(cmakelists)):
        os.chmod(configure_script, 0o755)
        configure_args = [configure_script, f"--prefix={install_prefix}"]
        
        if os.name == "nt":
          shell = self._find_shell(env)
          if not shell:
            raise RuntimeError("shell for running configure not found")
          host = env.get("CHOST") or env.get("HOST") or "x86_64-w64-mingw32"
          configure_args.append(f"--host={host}")
          subprocess.run([shell] + configure_args, cwd=self.source_dir, check=True, env=env)
        else:
          subprocess.run(configure_args, cwd=self.source_dir, check=True, env=env)

        make_cmd = shutil.which("make", path=env.get("PATH"))
        
        if self._is_windows():
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

        generator = None
        if self._is_windows():
          if shutil.which("mingw32-make", path=env.get("PATH")):
            generator = "MinGW Makefiles"
          elif shutil.which("nmake", path=env.get("PATH")):
            generator = "NMake Makefiles"

        configure = [cmake_cmd, "..", f"-DCMAKE_INSTALL_PREFIX={install_prefix}"]
        if generator:
          configure += ["-G", generator]

        subprocess.run(configure, cwd=build_dir, check=True, env=env)

        build_cmd = [cmake_cmd, "--build", ".", "--target", "install"]
        if self._is_windows():
          build_cmd += ["--config", "Release"]
        
        subprocess.run(build_cmd, cwd=build_dir, check=True, env=env)

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

      elif os.path.isfile(os.path.join(self.source_dir, "src", "glad.c")):
        include_dir = os.path.join(self.source_dir, "include")
        src_file = os.path.join(self.source_dir, "src", "glad.c")
        dst_include = os.path.join(install_prefix, "include")
        dst_lib = os.path.join(install_prefix, "lib")
        os.makedirs(dst_include, exist_ok=True)
        os.makedirs(dst_lib, exist_ok=True)
        shutil.copytree(include_dir, dst_include, dirs_exist_ok=True)

        cc = shutil.which("cc", path=env.get("PATH")) or shutil.which("gcc", path=env.get("PATH"))
        if not cc:
          raise RuntimeError("'cc' not found in PATH")
        obj_file = os.path.join(self.source_dir, "glad.o")
        subprocess.run([cc, "-c", src_file, "-o", obj_file], cwd=self.source_dir, check=True, env=env)

        ar = shutil.which("ar", path=env.get("PATH")) or "ar"
        if not shutil.which(ar, path=env.get("PATH")):
          raise RuntimeError("'ar' not found in PATH")
        lib_file = os.path.join(dst_lib, "libglad.a")
        subprocess.run([ar, "rcs", lib_file, obj_file], cwd=self.source_dir, check=True, env=env)
        os.remove(obj_file)

      elif os.path.isfile(makefile):
        make_cmd = shutil.which("make", path=env.get("PATH"))
        if not make_cmd:
          raise RuntimeError("'make' not found in PATH")

        is_lua = os.path.exists(os.path.join(self.source_dir, "src", "luaconf.h"))

        if is_lua:
          target = "mingw" if self._is_windows() else "linux"
          subprocess.run([make_cmd, target], cwd=self.source_dir, check=True, env=env)
          install_path = install_prefix
          if self._is_msys_make(make_cmd):
            install_path = self._to_msys_path(install_path)
          subprocess.run([
            make_cmd,
            "install",
            f"INSTALL_TOP={install_prefix}",
            f"INSTALL_TOP={install_path}",
          ], cwd=self.source_dir, check=True, env=env)
        
        else:
          subprocess.run([make_cmd], cwd=self.source_dir, check=True, env=env)
          prefix_path = install_prefix
          if self._is_msys_make(make_cmd):
            prefix_path = self._to_msys_path(prefix_path)
          subprocess.run([
            make_cmd,
            "install",
            f"PREFIX={install_prefix}",
            f"PREFIX={prefix_path}",
          ], cwd=self.source_dir, check=True, env=env)

      else:
        raise RuntimeError(f"No build system found in {self.source_dir}")
      
      print(f"Build finished: {self.source_dir}")

    except subprocess.CalledProcessError as e:
      print(f"Build failed for {self.source_dir}: {e}")
      raise e

  def _is_windows(self) -> bool:
    platform_val = sys.platform.lower()
    return os.name == "nt" or platform_val.startswith("win") or \
           platform_val.startswith("msys") or platform_val.startswith("cygwin")

  def _find_shell(self, env):
    path_entries = env.get("PATH", "").split(os.pathsep)
    for entry in path_entries:
      for name in ("bash.exe", "bash", "sh.exe", "sh"):
        candidate = os.path.join(entry, name)
        if os.path.isfile(candidate):
          norm = os.path.normcase(candidate)
          if os.path.basename(candidate).startswith("bash") and "system32" in norm:
            continue
          return candidate
    return None

  def _is_msys_make(self, make_cmd: str) -> bool:
    if not make_cmd or not self._is_windows():
      return False
    cmd_lower = os.path.normcase(make_cmd)
    return "msys" in cmd_lower or "usr\\bin\\make" in cmd_lower

  def _to_msys_path(self, path: str) -> str:
    path = os.path.abspath(path)
    drive, rest = os.path.splitdrive(path)
    rest = rest.replace("\\", "/")
    if drive:
      return f"/{drive[0].lower()}{rest}"
    return rest
 
