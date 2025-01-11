import subprocess

class IBuilder:
  def build(self, source_dir: str, output_dir: str):
    raise NotImplementedError("build method must be implemented")