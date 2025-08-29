import os
from dependency.downloader import HTTPDownloader
from dependency.extractor import TarGzExtractor
from dependency.builder import Builder
from dependency.utils import archive_exists, remove_dir

class DependencyManager:
  def __init__(self, downloader, extractor):
    self.downloader: HTTPDownloader = downloader
    self.extractor: TarGzExtractor = extractor

  def process_dependency(self, url, archive_path, source_dir, output_dir, env=None, sha256=None):
    try:
      if not archive_exists(archive_path):
        self.downloader.download(url, archive_path, expected_sha256=sha256)
        
      if not os.path.exists(source_dir):
        self.extractor.extract(archive_path, source_dir)

      builder = Builder(source_dir)
      builder.build(output_dir, env=env)
      self._verify_installation(output_dir)

    except Exception as e:
      remove_dir(output_dir)
      print(f"Failed to process dependency {url}: {e}")
      raise e

  def _verify_installation(self, output_dir: str):
    if not os.path.isdir(output_dir):
      raise RuntimeError(f"Installation directory missing: {output_dir}")
    
    has_files = any(os.scandir(output_dir))
    if not has_files:
      raise RuntimeError(f"Installation failed, {output_dir} is empty")
