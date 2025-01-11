import subprocess

class IBuilder:
  def build(self, source_dir: str, output_dir: str):
    raise NotImplementedError("build method must be implemented")
  
class CMakeBuilder(IBuilder):
  def build(self, source_dir: str, output_dir: str):
    print(f"Building {source_dir} into {output_dir}...")