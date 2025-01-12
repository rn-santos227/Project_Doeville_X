from dependency.downloader import HTTPDownloader
from dependency.extractor import TarGzExtractor
from dependency.utils import ensure_folder_exists, archive_exists

class DependencyManager:
  def __init__(self, downloader, extractor):
    pass