import subprocess

class IExtractor:
  def extract(self, archive: str, destination: str):
    raise NotImplementedError("extract method must be implemented")