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

      except Exception as e:
        pass