#include "GeometryUtils.h"

#include <algorithm>
#include <cmath>

#include "libraries/constants/AngleConstants.h"
#include "libraries/constants/FloatConstants.h"
#include "libraries/constants/IndexConstants.h"
#include "libraries/constants/NumericConstants.h"

namespace Project::Utilities {
  namespace Constants = Project::Libraries::Constants;
  Circle GeometryUtils::makeCircle(float x, float y, float r) {
    return {x, y, r};
  }

  SDL_FRect GeometryUtils::makeRect(float x, float y, float w, float h) {
    return SDL_FRect{x, y, w, h};
  }

  SDL_FRect GeometryUtils::capsuleBounds(const Capsule& cap) {
    float minX = std::min(cap.start.x, cap.end.x) - cap.r;
    float minY = std::min(cap.start.y, cap.end.y) - cap.r;
    float maxX = std::max(cap.start.x, cap.end.x) + cap.r;
    float maxY = std::max(cap.start.y, cap.end.y) + cap.r;
    return SDL_FRect{minX, minY, maxX - minX, maxY - minY};
  }

  SDL_FRect GeometryUtils::polygonBounds(const Polygon& poly) {
    if (poly.vertices.empty()) return SDL_FRect{0.f, 0.f, 0.f, 0.f};
    float minX = poly.vertices[0].x;
    float minY = poly.vertices[0].y;
    float maxX = poly.vertices[0].x;
    float maxY = poly.vertices[0].y;
    for (const auto& v : poly.vertices) {
      if (v.x < minX) minX = v.x;
      if (v.y < minY) minY = v.y;
      if (v.x > maxX) maxX = v.x;
      if (v.y > maxY) maxY = v.y;
    }
    return SDL_FRect{minX, minY, maxX - minX, maxY - minY};
  }

  float GeometryUtils::distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
  }

  float GeometryUtils::pointSegmentDistance(float px, float py, float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    if (dx == 0 && dy == 0) {
      dx = px - x1;
      dy = py - y1;
      return std::sqrt(dx*dx + dy*dy);
    }
    float t = ((px - x1) * dx + (py - y1) * dy) / (dx*dx + dy*dy);
    t = std::clamp(t, 0.0f, Constants::DEFAULT_WHOLE);
    float projX = x1 + t * dx;
    float projY = y1 + t * dy;
    dx = px - projX;
    dy = py - projY;
    return std::sqrt(dx*dx + dy*dy);
  }

  bool GeometryUtils::circleIntersect(const Circle& a, const Circle& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float distSq = dx * dx + dy * dy;
    float radiusSum = a.r + b.r;
    return distSq <= radiusSum * radiusSum;
  }

  bool GeometryUtils::rectIntersect(const SDL_FRect& a, const SDL_FRect& b) {
    SDL_FRect inter;
    return SDL_IntersectFRect(&a, &b, &inter);
  }

  bool GeometryUtils::rectCircleIntersect(const SDL_FRect& rect, const Circle& c) {
    float closestX = std::clamp(c.x, rect.x, rect.x + rect.w);
    float closestY = std::clamp(c.y, rect.y, rect.y + rect.h);
    float dx = c.x - closestX;
    float dy = c.y - closestY;
    return (dx * dx + dy * dy) <= (c.r * c.r);
  }

  bool GeometryUtils::polygonIntersect(const Polygon& a, const Polygon& b) {
    SDL_FRect ba = polygonBounds(a);
    SDL_FRect bb = polygonBounds(b);
    return rectIntersect(ba, bb);
  }

  bool GeometryUtils::polygonCircleIntersect(const Polygon& poly, const Circle& c) {
    SDL_FRect bounds = polygonBounds(poly);
    if (!rectCircleIntersect(bounds, c)) return false;
    return true;
  }

  bool GeometryUtils::capsuleCircleIntersect(const Capsule& cap, const Circle& c) {
    float dist = pointSegmentDistance(c.x, c.y, cap.start.x, cap.start.y, cap.end.x, cap.end.y);
    return dist <= cap.r + c.r;
  }

  void GeometryUtils::renderCircle(SDL_Renderer* renderer, int cx, int cy, int r) {
    int x = r - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (r << 1);

    while (x >= y) {
      SDL_RenderDrawPoint(renderer, cx + x, cy + y);
      SDL_RenderDrawPoint(renderer, cx + y, cy + x);
      SDL_RenderDrawPoint(renderer, cx - y, cy + x);
      SDL_RenderDrawPoint(renderer, cx - x, cy + y);
      SDL_RenderDrawPoint(renderer, cx - x, cy - y);
      SDL_RenderDrawPoint(renderer, cx - y, cy - x);
      SDL_RenderDrawPoint(renderer, cx + y, cy - x);
      SDL_RenderDrawPoint(renderer, cx + x, cy - y);

      if (err <= 0) {
        y++;
        err += dy;
        dy += Constants::INDEX_TWO;
      }
      
      if (err > 0) {
        x--;
        dx += Constants::INDEX_TWO;
        err += dx - (r << 1);
      }
    }
  }

  void GeometryUtils::renderFilledCircle(SDL_Renderer* renderer, int cx, int cy, int r) {
    int x = r;
    int y = 0;
    int err = 1 - x;
    while (x >= y) {
      SDL_RenderDrawLine(renderer, cx - x, cy + y, cx + x, cy + y);
      SDL_RenderDrawLine(renderer, cx - x, cy - y, cx + x, cy - y);
      SDL_RenderDrawLine(renderer, cx - y, cy + x, cx + y, cy + x);
      SDL_RenderDrawLine(renderer, cx - y, cy - x, cx + y, cy - x);
      ++y;
      if (err < 0) {
        err += Constants::INDEX_TWO * y + 1;
      } else {
        --x;
        err += Constants::INDEX_TWO * (y - x + 1);
      }
    }
  }

  void GeometryUtils::renderRoundedRect(SDL_Renderer* renderer, const SDL_FRect& rect, int r) {
    if (r <= 0) {
      SDL_RenderDrawRectF(renderer, &rect);
      return;
    }

    float radius = std::min(static_cast<float>(r), std::min(rect.w, rect.h) / 2.0f);
    float x = rect.x;
    float y = rect.y;
    float w = rect.w;
    float h = rect.h;

    SDL_RenderDrawLineF(renderer, x + radius, y, x + w - radius, y);
    SDL_RenderDrawLineF(renderer, x + radius, y + h, x + w - radius, y + h);
    SDL_RenderDrawLineF(renderer, x, y + radius, x, y + h - radius);
    SDL_RenderDrawLineF(renderer, x + w, y + radius, x + w, y + h - radius);

    auto drawQuarterCircle = [&](float cx, float cy, float r, int startAngleDeg, int endAngleDeg) {
      const int steps = Constants::BIT_32;
      float step = (endAngleDeg - startAngleDeg) * (M_PI / Constants::ANGLE_180_DEG) / steps;
      float angle = startAngleDeg * (M_PI / 180.0f);

      for (int i = 0; i < steps; i++) {
        float x1 = cx + cosf(angle) * r;
        float y1 = cy + sinf(angle) * r;
        float x2 = cx + cosf(angle + step) * r;
        float y2 = cy + sinf(angle + step) * r;
        SDL_RenderDrawLineF(renderer, x1, y1, x2, y2);
        angle += step;
      }
    };

    drawQuarterCircle(x + radius, y + radius, radius, Constants::ANGLE_180_DEG, Constants::ANGLE_270_DEG);
    drawQuarterCircle(x + w - radius, y + radius, radius, Constants::ANGLE_270_DEG, Constants::ANGLE_360_DEG);
    drawQuarterCircle(x + w - radius, y + h - radius, radius, Constants::ANGLE_0_DEG, Constants::ANGLE_90_DEG);
    drawQuarterCircle(x + radius, y + h - radius, radius, Constants::ANGLE_90_DEG, Constants::ANGLE_180_DEG);
  }


  void GeometryUtils::renderFilledRoundedRect(SDL_Renderer* renderer, const SDL_FRect& rect, int r) {
    if (r <= 0) {
      SDL_RenderFillRectF(renderer, &rect);
      return;
    }

    float radius = std::min(static_cast<float>(r), std::min(rect.w, rect.h) / 2.0f);
    float x = rect.x;
    float y = rect.y;
    float w = rect.w;
    float h = rect.h;

    SDL_FRect core = { x + radius, y, w - 2 * radius, h };
    SDL_RenderFillRectF(renderer, &core);

    SDL_FRect left = { x, y + radius, radius, h - 2 * radius };
    SDL_RenderFillRectF(renderer, &left);

    SDL_FRect right = { x + w - radius, y + radius, radius, h - 2 * radius };
    SDL_RenderFillRectF(renderer, &right);

    auto fillQuarterCircle = [&](float cx, float cy, float r, int startAngleDeg, int endAngleDeg) {
      const int steps = Constants::BIT_32;
        float step = (endAngleDeg - startAngleDeg) * (M_PI / Constants::ANGLE_180_DEG) / steps;

        for (int i = 0; i < steps; i++) {
            float angle = (startAngleDeg + i * (Constants::ANGLE_180_DEG / steps)) * (M_PI / Constants::ANGLE_180_DEG);
            float nextAngle = (startAngleDeg + (i + 1) * (Constants::ANGLE_180_DEG / steps)) * (M_PI / Constants::ANGLE_180_DEG);

            float x1 = cx + cosf(angle) * r;
            float y1 = cy + sinf(angle) * r;
            float x2 = cx + cosf(nextAngle) * r;
            float y2 = cy + sinf(nextAngle) * r;

            SDL_RenderDrawLineF(renderer, cx, cy, x1, y1);
            SDL_RenderDrawLineF(renderer, cx, cy, x2, y2);
            SDL_RenderDrawLineF(renderer, x1, y1, x2, y2);
        }
    };

    fillQuarterCircle(x + radius, y + radius, radius, Constants::ANGLE_180_DEG, Constants::ANGLE_270_DEG);
    fillQuarterCircle(x + w - radius, y + radius, radius, Constants::ANGLE_270_DEG, Constants::ANGLE_360_DEG);
    fillQuarterCircle(x + w - radius, y + h - radius, radius, Constants::ANGLE_0_DEG, Constants::ANGLE_90_DEG);
    fillQuarterCircle(x + radius, y + h - radius, radius, Constants::ANGLE_90_DEG, Constants::ANGLE_180_DEG);
  }
}
