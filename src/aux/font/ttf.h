#pragma once

#include <string>

#include "bezier.h"
#include "ttf_types.h"

namespace ngi::font
{
std::vector<SimpleGlyph> get_simple_glyphs(std::string const& font_filename);
std::vector<Contour> get_contours(SimpleGlyph const& g);
} // namespace ngi::font
