#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace ngi::font
{
typedef std::array<std::array<double, 2>, 3> BezierCurve;

typedef std::array<int16_t, 2> Point;
typedef std::vector<std::tuple<Point, bool>> Contour;
} // namespace ngi::font
