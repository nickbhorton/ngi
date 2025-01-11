#pragma once

#include <array>
#include <cstdint>
#include <vector>

// #include "arrayalgebra.h"

struct BezierCurve {
    std::array<std::array<int16_t, 2>, 3> curve;

    BezierCurve() = default;
    BezierCurve(const BezierCurve&) = default;
    BezierCurve(BezierCurve&&) = default;
    BezierCurve& operator=(const BezierCurve&) = default;
    BezierCurve& operator=(BezierCurve&&) = default;
};

struct Contour {
    std::vector<BezierCurve> curves;

    Contour(const Contour&) = default;
    Contour(Contour&&) = default;
    Contour& operator=(const Contour&) = default;
    Contour& operator=(Contour&&) = default;
};
