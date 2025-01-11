#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace ngi::font
{
typedef std::array<int16_t, 2> Point;
typedef std::array<Point, 3> BezierCurve;
typedef std::vector<std::tuple<Point, bool>> Contour;
} // namespace ngi::aux::font
