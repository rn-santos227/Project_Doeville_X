#include "GeometryUtils.h"

#include <algorithm>
#include <cmath>

#include "libraries/constants/IndexConstants.h"
#include "libraries/constants/PhysicsConstants.h"

namespace Project::Utilities {
  namespace Constants = Project::Libraries::Constants;
  Circle GeometryUtils::makeCircle(int x, int y, int r) { 
    return {x, y, r}; 
  }

  SDL_Rect GeometryUtils::makeRect(int x, int y, int w, int h) {
    SDL_Rect rect{x, y, w, h};
    return rect;
  }

  SDL_Rect GeometryUtils::capsuleBounds(const Capsule& cap) {
    float minX = std::min(cap.start.x, cap.end.x) - cap.r;
    float minY = std::min(cap.start.y, cap.end.y) - cap.r;
    float maxX = std::max(cap.start.x, cap.end.x) + cap.r;
    float maxY = std::max(cap.start.y, cap.end.y) + cap.r;
    return SDL_Rect{static_cast<int>(minX), static_cast<int>(minY), static_cast<int>(maxX - minX), static_cast<int>(maxY - minY)};
  }

  SDL_Rect GeometryUtils::polygonBounds(const Polygon& poly) {
    if (poly.vertices.empty()) return SDL_Rect{0,0,0,0};
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
    return SDL_Rect{static_cast<int>(minX), static_cast<int>(minY), static_cast<int>(maxX - minX), static_cast<int>(maxY - minY)};
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
    float dx = static_cast<float>(a.x - b.x);
    float dy = static_cast<float>(a.y - b.y);
    float distSq = dx * dx + dy * dy;
    int radiusSum = a.r + b.r;
    return distSq <= static_cast<float>(radiusSum * radiusSum);
  }

  bool GeometryUtils::rectIntersect(const SDL_Rect& a, const SDL_Rect& b) {
    SDL_Rect inter;
    return SDL_IntersectRect(&a, &b, &inter);
  }

  bool GeometryUtils::rectCircleIntersect(const SDL_Rect& rect, const Circle& c) {
    int closestX = std::clamp(c.x, rect.x, rect.x + rect.w);
    int closestY = std::clamp(c.y, rect.y, rect.y + rect.h);
    int dx = c.x - closestX;
    int dy = c.y - closestY;
    return (dx * dx + dy * dy) <= (c.r * c.r);
  }

  bool GeometryUtils::polygonIntersect(const Polygon& a, const Polygon& b) {
    SDL_Rect ba = polygonBounds(a);
    SDL_Rect bb = polygonBounds(b);
    return rectIntersect(ba, bb);
  }

  bool GeometryUtils::polygonCircleIntersect(const Polygon& poly, const Circle& c) {
    SDL_Rect bounds = polygonBounds(poly);
    if (!rectCircleIntersect(bounds, c)) return false;
    return true;
  }

  bool GeometryUtils::capsuleCircleIntersect(const Capsule& cap, const Circle& c) {
    float dist = pointSegmentDistance(static_cast<float>(c.x), static_cast<float>(c.y), cap.start.x, cap.start.y, cap.end.x, cap.end.y);
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

  void GeometryUtils::renderRoundedRect(SDL_Renderer* renderer, const SDL_Rect& rect, int r) {
    if (r <= 0) {
      SDL_RenderDrawRect(renderer, &rect);
      return;
    }

    int radius = std::min(r, std::min(rect.w, rect.h) / 2);
    int x = rect.x;
    int y = rect.y;
    int w = rect.w;
    int h = rect.h;

    SDL_RenderDrawLine(renderer, x + radius, y, x + w - radius - 1, y);
    SDL_RenderDrawLine(renderer, x + radius, y + h - 1, x + w - radius - 1, y + h - 1);
    SDL_RenderDrawLine(renderer, x, y + radius, x, y + h - radius - 1);
    SDL_RenderDrawLine(renderer, x + w - 1, y + radius, x + w - 1, y + h - radius - 1);
  
    SDL_Rect clip;
    clip = {x, y, radius, radius};
    SDL_RenderSetClipRect(renderer, &clip);
    renderCircle(renderer, x + radius, y + radius, radius);
    clip = {x + w - radius, y, radius, radius};

    SDL_RenderSetClipRect(renderer, &clip);
    renderCircle(renderer, x + w - 1 - radius, y + radius, radius);
    clip = {x, y + h - radius, radius, radius};
    SDL_RenderSetClipRect(renderer, &clip);
    renderCircle(renderer, x + radius, y + h - 1 - radius, radius);
    clip = {x + w - radius, y + h - radius, radius, radius};
    SDL_RenderSetClipRect(renderer, &clip);
    renderCircle(renderer, x + w - 1 - radius, y + h - 1 - radius, radius);
    SDL_RenderSetClipRect(renderer, nullptr);
  }

  void GeometryUtils::renderFilledRoundedRect(SDL_Renderer* renderer, const SDL_Rect& rect, int r) {
    if (r <= 0) {
      SDL_RenderFillRect(renderer, &rect);
      return;
    }
    
    int radius = std::min(r, std::min(rect.w, rect.h) / Constants::INDEX_TWO);

    SDL_Rect core{rect.x + radius, rect.y, rect.w - Constants::INDEX_TWO * radius, rect.h};
    SDL_RenderFillRect(renderer, &core);
    SDL_Rect left{rect.x, rect.y + radius, radius, rect.h - Constants::INDEX_TWO * radius};
    SDL_RenderFillRect(renderer, &left);
    SDL_Rect right{rect.x + rect.w - radius, rect.y + radius, radius, rect.h - Constants::INDEX_TWO * radius};
    SDL_RenderFillRect(renderer, &right);

    SDL_Rect clip;
    clip = {rect.x, rect.y, radius, radius};
    SDL_RenderSetClipRect(renderer, &clip);

    renderFilledCircle(renderer, rect.x + radius, rect.y + radius, radius);
    clip = {rect.x + rect.w - radius, rect.y, radius, radius};
    SDL_RenderSetClipRect(renderer, &clip);
    renderFilledCircle(renderer, rect.x + rect.w - 1 - radius, rect.y + radius, radius);
    clip = {rect.x, rect.y + rect.h - radius, radius, radius};
    SDL_RenderSetClipRect(renderer, &clip);
    renderFilledCircle(renderer, rect.x + radius, rect.y + rect.h - 1 - radius, radius);
    clip = {rect.x + rect.w - radius, rect.y + rect.h - radius, radius, radius};
    SDL_RenderSetClipRect(renderer, &clip);
    renderFilledCircle(renderer, rect.x + rect.w - 1 - radius, rect.y + rect.h - 1 - radius, radius);
    SDL_RenderSetClipRect(renderer, nullptr);
  }
}
