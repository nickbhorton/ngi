#include "bezier.h"
#include "arrayalgebra.h"

namespace ngi::font
{
QuadraticBezier::QuadraticBezier(std::array<aa::dvec2, 3> const& Ps)
    : P0{Ps[0]}, P1{Ps[1]}, P2{Ps[2]}
{
    p1 = P1 - P0;
    p2 = P2 - 2.0 * P1 + P0;
}

aa::dvec2 QuadraticBezier::QuadraticBezier::p(aa::dvec2 const& P) const
{
    return P - P0;
}

aa::dvec2 QuadraticBezier::evaluate(double t) const
{
    return t * t * p2 + 2.0 * t * p1 + P0;
}

aa::dvec2 QuadraticBezier::derivative(double t) const
{
    return 2.0 * t * (P2 - 2.0 * P1 + P0) + 2.0 * (P1 - P0);
}

double QuadraticBezier::signed_distance(aa::dvec2 const& P) const
{
    double a{aa::dot(p2, p2)};
    double B{3.0 * aa::dot(p1, p2)};
    double c{2.0 * aa::dot(p1, p1) - aa::dot(p2, p(P))};
    double d{-aa::dot(p1, p(P))};
    auto rs = solve_cubic(a, B, c, d);
    std::vector<double> valid_ts = {0.0, 1.0};
    for (auto const& r : rs) {
        if (r[1] == 0.0 && r[0] > 0.0 && r[0] < 1.0) {
            valid_ts.push_back(r[0]);
        }
    }
    double min_dist{std::numeric_limits<double>::max()};
    double min_t{std::numeric_limits<double>::max()};
    for (auto const& t : valid_ts) {
        double dist{aa::magnitude(evaluate(t) - P)};
        if (dist < min_dist) {
            min_dist = dist;
            min_t = t;
        }
    }

    if (min_dist == std::numeric_limits<double>::max()) {
        std::cout << "min dist was double max indicating that no distance was "
                     "found\n";
    }
    aa::dvec2 bntmp{evaluate(min_t) - P};
    aa::dvec2 dt{derivative(min_t)};
    bool sgn{std::signbit(aa::cross(
        aa::dvec3{dt[0], dt[1], 0.0},
        aa::dvec3{bntmp[0], bntmp[1], 0.0}
    )[2])};
    return sgn ? -aa::magnitude(bntmp) : aa::magnitude(bntmp);
}

LinearBezier::LinearBezier(std::array<aa::dvec2, 2> const& Ps)
    : P0{Ps[0]}, P1{Ps[1]}
{
}

aa::dvec2 LinearBezier::evaluate(double t) const
{
    return (1.0 - t) * P0 + t * P1;
}

aa::dvec2 LinearBezier::derivative([[maybe_unused]] double t) const
{
    return P1 - P0;
}

double LinearBezier::signed_distance(aa::dvec2 const& P) const
{
    double r = (aa::dot(P - P0, P1 - P0)) / (aa::dot(P1 - P0, P1 - P0));
    std::vector<double> valid_ts = {0.0, 1.0};
    if (r >= 0.0 && r <= 1.0) {
        valid_ts.push_back(r);
    }

    double min_dist{std::numeric_limits<double>::max()};
    double min_t{std::numeric_limits<double>::max()};
    for (auto const& t : valid_ts) {
        double dist{aa::magnitude(evaluate(t) - P)};
        if (dist < min_dist) {
            min_dist = dist;
            min_t = t;
        }
    }

    if (min_dist == std::numeric_limits<double>::max()) {
        std::cout << "min dist was double max indicating that no distance was "
                     "found\n";
    }
    aa::dvec2 bntmp{evaluate(min_t) - P};
    aa::dvec2 dt{derivative(min_t)};
    bool sgn{std::signbit(aa::cross(
        aa::dvec3{dt[0], dt[1], 0.0},
        aa::dvec3{bntmp[0], bntmp[1], 0.0}
    )[2])};
    return sgn ? -aa::magnitude(bntmp) : aa::magnitude(bntmp);
}

// taken from cubic equation calculator
std::array<aa::dvec2, 3> solve_cubic(double a, double b, double c, double d)
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
} // namespace ngi::font
