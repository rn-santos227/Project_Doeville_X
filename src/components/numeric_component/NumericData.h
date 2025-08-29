#ifndef NUMERIC_DATA_H
#define NUMERIC_DATA_H

namespace Project::Components {

  struct NumericData {
    float value = 0.0f;
    float limit = 0.0f;

    NumericData() = default;
    NumericData(float val, float lim) : value(val), limit(lim) {
      clamp();
    }

    void set(float val) {
      value = val;
      clamp();
    }

    void add(float delta) {
      value += delta;
      clamp();
    }

    void subtract(float delta) {
      value -= delta;
      if (value < 0.0f) value = 0.0f;
    }

    void multiply(float factor) {
      value *= factor;
      clamp();
    }

    void divide(float divisor) {
      if (divisor != 0.0f) {
        value /= divisor;
        clamp();
      }
    }

    float get() const {
      return value;
    }

    float getLimit() const {
      return limit;
    }

    void setLimit(float lim) {
      limit = lim;
      clamp();
    }

    bool isFull() const {
      return value >= limit;
    }

    bool isEmpty() const {
      return value <= 0.0f;
    }

  private:
    void clamp() {
      if (value > limit) value = limit;
      if (value < 0.0f) value = 0.0f;
    }
  };

}

#endif
