#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "arrayalgebra.h"

namespace ngi::font
{

std::array<aa::dvec2, 3> solve_cubic(double a, double b, double c, double d);

class QuadraticBezier
{
    aa::dvec2 P0;
    aa::dvec2 P1;
    aa::dvec2 P2;
    aa::dvec2 p1;
    aa::dvec2 p2;
    aa::dvec2 p(aa::dvec2 const& P) const;

public:
    QuadraticBezier(std::array<aa::dvec2, 3> const& Ps);
    QuadraticBezier(const QuadraticBezier&) = default;
    QuadraticBezier(QuadraticBezier&&) = default;
    QuadraticBezier& operator=(const QuadraticBezier&) = default;
    QuadraticBezier& operator=(QuadraticBezier&&) = default;

    aa::dvec2 evaluate(double t) const;
    aa::dvec2 derivative(double t) const;
    double signed_distance(aa::dvec2 const& P) const;
};

class LinearBezier
{
    aa::dvec2 P0;
    aa::dvec2 P1;

public:
    LinearBezier(std::array<aa::dvec2, 2> const& Ps);
    LinearBezier(const LinearBezier&) = default;
    LinearBezier(LinearBezier&&) = default;
    LinearBezier& operator=(const LinearBezier&) = default;
    LinearBezier& operator=(LinearBezier&&) = default;

    aa::dvec2 evaluate(double t) const;
    aa::dvec2 derivative(double t) const;
    double signed_distance(aa::dvec2 const& P) const;
};

typedef std::array<std::array<double, 2>, 3> BezierCurve2;
typedef std::array<std::array<double, 2>, 2> BezierCurve1;

typedef std::array<int16_t, 2> Point;
typedef std::vector<std::tuple<Point, bool>> Contour;
} // namespace ngi::font
