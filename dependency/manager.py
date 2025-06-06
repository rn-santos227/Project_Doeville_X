import os
from dependency.downloader import HTTPDownloader
from dependency.extractor import TarGzExtractor
from dependency.builder import Builder
from dependency.utils import ensure_folder_exists, archive_exists

class DependencyManager:
  def __init__(self, downloader, extractor):
    self.downloader: HTTPDownloader = downloader
    self.extractor: TarGzExtractor = extractor

  def process_dependency(self, url, archive_path, source_dir, output_dir):
    if not archive_exists(archive_path):
      self.downloader.download(url, archive_path)
      self.extractor.extract(archive_path, source_dir)

      try:
        if not archive_exists(archive_path):
          self.downloader.download(url, archive_path)

        if not os.path.exists(source_dir):
          self.extractor.extract(archive_path, source_dir)
        
        builder = Builder(source_dir)
        builder.build(output_dir)

      except Exception as e:
        print(f"Failed to process dependency {url}: {e}")
        raise