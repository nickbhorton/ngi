#include "arrayalgebra.h"
#include "aux/font/bezier.h"
#include "aux/font/ttf.h"
#include "aux/png/image.h"
#include <cmath>
#include <limits>

using namespace ngi::font;

// taken from cubic equation calculator
std::array<std::array<double, 2>, 3>
solve_cubic(double a, double b, double c, double d)
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
}

aa::dvec2 bezier2_evaluate(BezierCurve2 const& b, double t)
{
    return b[0] + 2.0 * t * (b[1] - b[0]) + t * t * (b[2] - 2.0 * b[1] + b[0]);
}

double bezier2_min_dist(BezierCurve2 const& b, aa::dvec2 P)
{
    aa::dvec2 p{P - b[0]};
    aa::dvec2 p1{b[1] - b[0]};
    aa::dvec2 p2{b[2] - 2.0 * b[1] + b[0]};
    double a{aa::dot(p2, p2)};
    double B{3.0 * aa::dot(p1, p2)};
    double c{2.0 * aa::dot(p1, p1) - aa::dot(p2, p)};
    double d{-aa::dot(p1, p)};
    auto rs = solve_cubic(a, B, c, d);
    std::vector<double> valid_ts = {0.0, 1.0};
    for (auto const& r : rs) {
        if (r[1] == 0.0 && r[0] > 0.0 && r[0] < 1.0) {
            valid_ts.push_back(r[0]);
        }
    }
    double min_dist{std::numeric_limits<double>::max()};
    double min_t = -1.0;

    for (auto const& t : valid_ts) {
        double dist{aa::magnitude(bezier2_evaluate(b, t) - P)};
        // std::cout << dist << " ";
        if (dist < min_dist) {
            min_dist = dist;
            min_t = t;
        }
    }
    if (min_dist == std::numeric_limits<double>::max()) {
        std::cout << "min dist was double max indicating that no distance was "
                     "found\n";
    }
    return min_dist;
}

aa::dvec2 bezier1_evaluate(BezierCurve1 const& b, double t)
{
    return (1.0 - t) * b[0] + t * b[1];
}

double bezier1_min_dist(BezierCurve1 const& b, aa::dvec2 P)
{
    aa::dvec2 const& p0{b[0]};
    aa::dvec2 const& p1{b[1]};
    double r = (aa::dot(P - p0, p1 - p0)) / (aa::dot(p1 - p0, p1 - p0));
    std::vector<double> valid_ts = {0.0, 1.0};
    if (r >= 0.0 && r <= 1.0) {
        valid_ts.push_back(r);
    }
    double min_dist{std::numeric_limits<double>::max()};
    double min_t = -1.0;

    for (auto const& t : valid_ts) {
        double dist{aa::magnitude(bezier1_evaluate(b, t) - P)};
        // std::cout << dist << " ";
        if (dist < min_dist) {
            min_dist = dist;
            min_t = t;
        }
    }
    if (min_dist == std::numeric_limits<double>::max()) {
        std::cout << "min dist was double max indicating that no distance was "
                     "found\n";
    }
    return min_dist;
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

        std::vector<BezierCurve2> beziers2{};
        std::vector<BezierCurve1> beziers1{};

        if (glyph_contours.size() > 1) {
            std::cerr << "glyphs with more than 1 contour are not implemented\n";
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
                double epsilon_mult{2.0};
                bool eq0{
                    std::abs(pm[0] - p1[0]) <
                    std::numeric_limits<double>::epsilon() * epsilon_mult
                };
                bool eq1{
                    std::abs(pm[1] - p1[1]) <
                    std::numeric_limits<double>::epsilon() * epsilon_mult
                };
                if (eq0 && eq1) {
                    beziers1.push_back({p0, p2});
                } else {
                    beziers2.push_back({p0, p1, p2});
                }
            }
        }
        // std::cout << "bezier2 count: " << beziers2.size() << "\n";
        // std::cout << "bezier1 count: " << beziers1.size() << "\n";

        size_t constexpr x_discr{64};
        size_t constexpr y_discr{64};
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
                double min_dist{std::numeric_limits<double>::max()};
                for (auto const& bezier2 : beziers2) {
                    double dist{bezier2_min_dist(bezier2, P)};
                    if (dist < min_dist) {
                        min_dist = dist;
                    }
                }
                for (auto const& bezier1 : beziers1) {
                    double dist{bezier1_min_dist(bezier1, P)};
                    if (dist < min_dist) {
                        min_dist = dist;
                    }
                }

                size_t hi = size_t(int(y_discr) - int(y) - 1);
                size_t wi = x;
                img.ref(hi, wi) = ftou8(
                    {static_cast<float>(min_dist),
                     static_cast<float>(min_dist),
                     static_cast<float>(min_dist),
                     1.0}
                );
                // std::cout << min_t << ":" << min_dist << "\n";
            }
            img.save("test.png");
        }
    } catch (int& e) {
        std::cout << "error throw: " << e << "\n";
    }
}
