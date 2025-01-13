#include "arrayalgebra.h"
#include "aux/font/bezier.h"
#include "aux/font/ttf.h"
#include "aux/png/image.h"
#include <cmath>
#include <limits>
#include <variant>

using namespace ngi::font;

bool double_eq(double a, double b, float mult)
{
    return std::abs(a - b) < std::numeric_limits<double>::epsilon() * mult;
}

bool dvec2_eq(aa::dvec2 a, aa::dvec2 b, float mult)
{
    return double_eq(a[0], b[0], mult) && double_eq(a[1], b[1], mult);
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "enter a glyph index\n";
        std::exit(1);
    }

    try {
        std::string font_filename{"/usr/share/fonts/TTF/FiraCode-Regular.ttf"};
        auto glyphs{get_simple_glyphs(font_filename)};
        size_t glyph_index{atoi(argv[1]) % glyphs.size()};

        std::cout << "contour count: "
                  << glyphs[glyph_index].gc.number_of_contours << "\n";

        auto glyph_contours = get_contours(glyphs[glyph_index]);

        std::cout << "point count: " << glyph_contours[0].size() << "\n";

        double x_min{static_cast<double>(glyphs[glyph_index].gc.x_min)};
        double x_max{static_cast<double>(glyphs[glyph_index].gc.x_max)};
        double y_min{static_cast<double>(glyphs[glyph_index].gc.y_min)};
        double y_max{static_cast<double>(glyphs[glyph_index].gc.y_max)};

        std::vector<QuadraticBezier> quads{};
        std::vector<LinearBezier> lins{};

        if (glyph_contours.size() > 1) {
            std::cerr
                << "glyphs with more than 1 contour are not implemented\n";
            std::exit(1);
        }

        double glyph_scale{0.5};
        double glyph_shift{0.25};

        for (auto i = 0; i < glyph_contours.size(); i++) {
            for (size_t j = 0; j < glyph_contours[i].size(); j += 2) {
                auto const& [x1, y1] = std::get<0>(glyph_contours[i].at(j));
                auto const& [x2, y2] = std::get<0>(
                    glyph_contours[i].at((j + 1) % glyph_contours[i].size())
                );
                auto const& [x3, y3] = std::get<0>(
                    glyph_contours[i].at((j + 2) % glyph_contours[i].size())
                );
                aa::dvec2 p0{
                    glyph_scale * ((static_cast<double>(x1) - x_min) /
                                   (x_max - x_min)) +
                        glyph_shift,
                    glyph_scale * ((static_cast<double>(y1) - y_min) /
                                   (y_max - y_min)) +
                        glyph_shift
                };
                aa::dvec2 p1{
                    glyph_scale * ((static_cast<double>(x2) - x_min) /
                                   (x_max - x_min)) +
                        glyph_shift,
                    glyph_scale * ((static_cast<double>(y2) - y_min) /
                                   (y_max - y_min)) +
                        glyph_shift
                };
                aa::dvec2 p2{
                    glyph_scale * ((static_cast<double>(x3) - x_min) /
                                   (x_max - x_min)) +
                        glyph_shift,
                    glyph_scale * ((static_cast<double>(y3) - y_min) /
                                   (y_max - y_min)) +
                        glyph_shift
                };
                aa::dvec2 pm{0.5 * (p0 + p2)};
                if (dvec2_eq(pm, p1, 2.0)) {
                    lins.push_back(LinearBezier({p0, p2}));
                } else {
                    quads.push_back(QuadraticBezier({p0, p1, p2}));
                }
            }
        }

        size_t constexpr x_discr{256};
        size_t constexpr y_discr{256};
        double x_half{1.0 / (2.0 * static_cast<double>(x_discr))};
        double y_half{1.0 / (2.0 * static_cast<double>(y_discr))};
        Image<x_discr, y_discr> img{};
        for (size_t y = 0; y < y_discr; y++) {
            for (size_t x = 0; x < x_discr; x++) {
                double xl{
                    static_cast<double>(x) / static_cast<double>(x_discr)
                };
                double yl{
                    static_cast<double>(y) / static_cast<double>(y_discr)
                };

                aa::dvec2 P{xl + x_half, yl + y_half};
                double signed_distance{std::numeric_limits<double>::max()};
                for (auto const& quad : quads) {
                    double csd{quad.signed_distance(P)};
                    if (std::abs(csd) < std::abs(signed_distance)) {
                        signed_distance = csd;
                    }
                }
                for (auto const& lin : lins) {
                    double csd{lin.signed_distance(P)};
                    if (std::abs(csd) < std::abs(signed_distance)) {
                        signed_distance = csd;
                    }
                }
                size_t hi = size_t(int(y_discr) - int(y) - 1);
                size_t wi = x;
                if (std::signbit(signed_distance)) {
                    img.ref(hi, wi) = ftou8(
                        {0.0,
                         0.0,
                         static_cast<float>(std::abs(signed_distance)),
                         1.0}
                    );
                } else {
                    img.ref(hi, wi) = ftou8(
                        {static_cast<float>(std::abs(signed_distance)),
                         0.0,
                         0.0,
                         1.0}
                    );
                }
            }
        }
        img.save("test.png");
    } catch (int& e) {
        std::cout << "error throw: " << e << "\n";
    }
}
