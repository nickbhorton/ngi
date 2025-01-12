#include "arrayalgebra.h"
#include "aux/font/bezier.h"
#include "aux/font/ttf.h"
#include "aux/png/image.h"
#include <cmath>
#include <limits>

aa::dvec2 bezier2(ngi::font::BezierCurve const& b, double t)
{
    return b[0] + 2.0 * t * (b[1] - b[0]) + t * t * (b[2] - 2.0 * b[1] + b[0]);
}

// taken from cubic equation calculator
std::array<std::array<double, 2>, 3>
cubic_solve(double a, double b, double c, double d)
{
    std::array<std::array<double, 2>, 3> result{};
    if (a == 0.0) {
        std::cout
            << "The coefficient of the cube of x is 0. Please use the utility "
               "for a SECOND degree quadratic. No further action taken.";
        return {};
    }

    if (d == 0.0) {
        std::cout
            << "One root is 0. Now divide through by x and use the utility for "
               "a SECOND degree quadratic to solve the resulting equation for "
               "the other two roots. No further action taken.";
        return {};
    }
    b /= a;
    c /= a;
    d /= a;
    double disc{}, q{}, r{}, dum1{}, s{}, t{}, term1{}, r13{};
    q = (3.0 * c - (b * b)) / 9.0;
    r = -(27.0 * d) + b * (9.0 * c - 2.0 * (b * b));
    r /= 54.0;
    disc = q * q * q + r * r;
    result[0][1] = 0; // The first root is always real.
    term1 = (b / 3.0);
    if (!std::signbit(disc)) { // one root real, two are complex
        s = r + std::sqrt(disc);
        s = (std::signbit(s)) ? -std::pow(-s, (1.0 / 3.0))
                              : std::pow(s, (1.0 / 3.0));
        t = r - std::sqrt(disc);
        t = (std::signbit(t)) ? -std::pow(-t, (1.0 / 3.0))
                              : std::pow(t, (1.0 / 3.0));
        result[0][0] = -term1 + s + t;
        term1 += (s + t) / 2.0;
        result[2][0] = result[1][0] = -term1;
        term1 = std::sqrt(3.0) * (-t + s) / 2;
        result[1][1] = term1;
        result[2][1] = -term1;
        return result;
    }
    // The remaining options are all real
    result[2][1] = result[1][1] = 0;
    if (disc == 0.0) { // All roots real, at least two are equal.
        r13 = ((r < 0) ? -std::pow(-r, (1.0 / 3.0)) : std::pow(r, (1.0 / 3.0)));
        result[0][0] = -term1 + 2.0 * r13;
        result[2][0] = result[1][0] = -(r13 + term1);
        return result;
    } // End if (disc == 0)
    // Only option left is that all roots are real and unequal (to get here, q <
    // 0)
    q = -q;
    dum1 = q * q * q;
    dum1 = std::acos(r / std::sqrt(dum1));
    r13 = 2.0 * std::sqrt(q);
    result[0][0] = -term1 + r13 * std::cos(dum1 / 3.0);
    result[1][0] = -term1 + r13 * std::cos((dum1 + 2.0 * M_PI) / 3.0);
    result[2][0] = -term1 + r13 * std::cos((dum1 + 4.0 * M_PI) / 3.0);
    return result;
} // End of cubicSolve

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "enter a glyph index\n";
        std::exit(1);
    }

    // try catch abstraction because throwing ensures destruction of all
    // objects. This is primarily important for the log class which outputs
    // somewhere on its destruction.
    try {
        /*
        std::string font_filename{"/usr/share/fonts/TTF/FiraCode-Regular.ttf"};
        auto glyphs{ngi::font::get_simple_glyphs(font_filename)};
        size_t glyph_index{atoi(argv[1]) % glyphs.size()};

        std::cout << "contour count: "
                  << glyphs[glyph_index].gc.number_of_contours << "\n";

        auto glyph_contours = ngi::font::get_contours(glyphs[glyph_index]);

        std::cout << "point count: " << glyph_contours[0].size() << "\n";

        double x_min{static_cast<double>(glyphs[glyph_index].gc.x_min)};
        double x_max{static_cast<double>(glyphs[glyph_index].gc.x_max)};
        double y_min{static_cast<double>(glyphs[glyph_index].gc.y_min)};
        double y_max{static_cast<double>(glyphs[glyph_index].gc.y_max)};

        std::vector<ngi::font::BezierCurve> beziers{};

        */
        double space_max{1.0};
        /*

        for (auto i = 0; i < glyph_contours.size(); i++) {
            std::vector<aa::vec3> vertex_positions{};
            for (size_t j = 0; j < glyph_contours[i].size(); j += 2) {
                auto const& [x1, y1] = std::get<0>(glyph_contours[i].at(j));
                auto const& [x2, y2] = std::get<0>(
                    glyph_contours[i].at((j + 1) % glyph_contours[i].size())
                );
                auto const& [x3, y3] = std::get<0>(
                    glyph_contours[i].at((j + 2) % glyph_contours[i].size())
                );
                aa::dvec2 p0{
                    space_max *
                        ((static_cast<double>(x1) - x_min) / (x_max - x_min)),
                    space_max *
                        ((static_cast<double>(y1) - y_min) / (y_max - y_min))
                };
                aa::dvec2 p1{
                    space_max *
                        ((static_cast<double>(x2) - x_min) / (x_max - x_min)),
                    space_max *
                        ((static_cast<double>(y2) - y_min) / (y_max - y_min))
                };
                aa::dvec2 p2{
                    space_max *
                        ((static_cast<double>(x3) - x_min) / (x_max - x_min)),
                    space_max *
                        ((static_cast<double>(y3) - y_min) / (y_max - y_min))
                };
                beziers.push_back({p0, p1, p2});
            }
        }
        std::cout << "bezier count: " << beziers.size() << "\n";
        */

        // auto const& bezier{beziers[0]};
        ngi::font::BezierCurve bezier{};
        bezier[0] = {0.25, 0.5};
        bezier[1] = {0.5, 0.5};
        bezier[2] = {0.5, 0.25};

        size_t constexpr x_discr{128};
        size_t constexpr y_discr{128};
        double x_half{space_max / (2.0 * static_cast<double>(x_discr))};
        double y_half{space_max / (2.0 * static_cast<double>(y_discr))};
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
                // std::cout << P << "  ";
                aa::dvec2 p{P - bezier[0]};
                aa::dvec2 p1{bezier[1] - bezier[0]};
                aa::dvec2 p2{bezier[2] - 2.0 * bezier[1] + bezier[0]};
                double a{aa::dot(p2, p2)};
                double b{3.0 * aa::dot(p1, p2)};
                double c{2.0 * aa::dot(p1, p1) - aa::dot(p2, p)};
                double d{-aa::dot(p1, p)};
                auto rs = cubic_solve(a, b, c, d);
                std::vector<double> valid_ts = {0.0, 1.0};
                for (auto const& r : rs) {
                    if (r[1] == 0.0 && r[0] > 0.0 && r[0] < 1.0) {
                        valid_ts.push_back(r[0]);
                    }
                }
                double min_dist{std::numeric_limits<double>::max()};
                double min_t = -1.0;

                /*
                std::cout << "[";
                for (auto const& t : valid_ts) {
                    std::cout << t << " ";
                }
                std::cout << "] ";
                */
                for (auto const& t : valid_ts) {
                    double dist{aa::magnitude(bezier2(bezier, t) - P)};
                    // std::cout << dist << " ";
                    if (dist < min_dist) {
                        min_dist = dist;
                        min_t = t;
                    }
                }

                if (min_dist > 0.1) {
                    img.ref(y, x) = ftou8({1.0, 1.0, 1.0, 1.0});
                }
                // std::cout << min_t << ":" << min_dist << "\n";
            }
            img.save("test.png");
        }
    } catch (int& e) {
        std::cout << "error throw: " << e << "\n";
    }
}
