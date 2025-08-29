#include "SweepAndPrune.h"

#include <algorithm>

namespace Project::Utilities {
  std::vector<SweepAndPrune::Pair> SweepAndPrune::findPairs(const std::vector<std::pair<SDL_FRect, Collider>>& objects) {
    std::vector<std::pair<SDL_FRect, Collider>> sorted = objects;
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
      return a.first.x < b.first.x;
    });

    std::vector<Pair> pairs;
    for (size_t i = 0; i < sorted.size(); ++i) {
      const auto& A = sorted[i];
      for (size_t j = i + 1; j < sorted.size(); ++j) {
        const auto& B = sorted[j];
        if (B.first.x > A.first.x + A.first.w) break;
        if (SDL_HasIntersectionF(&A.first, &B.first)) {
          pairs.emplace_back(A.second, B.second);
        }
      }
    }
    return pairs;
  }
}
