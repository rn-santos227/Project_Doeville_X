#ifndef MAP_ASSET_DATA_H
#define MAP_ASSET_DATA_H

#include <string>
#include <vector>
#include <unordered_map>

namespace Project::Assets {
  struct MapAssetData {
    std::vector<std::vector<int>> layout;
    std::unordered_map<int, std::string> tiles;
    int rows = 0;
    int cols = 0;
  };
}

#endif
