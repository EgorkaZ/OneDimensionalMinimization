#include "methods/Brent.h"

#include "ReplayData.h"
#include "VersionedData.h"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace min1d {

double Brent::find_min_impl() noexcept /*override*/
{
    return any_find_min<false>();
}

double Brent::find_min_tracked_impl() noexcept /*override*/
{
    return any_find_min<true>();
}

bool all_different(double val1, double val2, double val3, double eps)
{
    return (std::abs(val1 - val2) < eps) &&
            (std::abs(val1 - val3) < eps) &&
            (std::abs(val2 - val3) < eps);
}

#define ENTRY std::pair<double, double>
double parabole(ENTRY e1, ENTRY e2, ENTRY e3)
{
    double x1 = e1.first;
    double f1 = e1.second;
    double x2 = e2.first;
    double f2 = e2.second;
    double x3 = e3.first;
    double f3 = e3.second;

    double a1 = (f2 - f1) / (x2 - x1);
    double a2 = ((f3 - f1) / (x3 - x1) - a1) / (x3 - x2);
    return (x1 + x2 - a1 / a2) / 2;
}
#undef ENTRY

template <bool is_tracked>
double Brent::any_find_min() noexcept
{
    const auto & fn = last_func();
    auto bnds = fn.bounds();
    uint iter_num = 0;

    double x, w, v;
    double f_x, f_w, f_v;
    x = w = v = (bnds.to + bnds.from) / 2;
    f_x = f_w = f_v = fn(x);

    double step, prev_step;
    step = prev_step = bnds.length();
    while (bnds.length() / 2 > m_eps)
    {
        double prev_prev_step = prev_step;
        prev_step = step;
        double u;
        bool is_accepted = false;
        if (all_different(x, w, v, m_eps) && all_different(f_x, f_w, f_v, m_eps)) {
            u = parabole({ x, f_x }, { w, f_w }, { v, f_v });
            if (bnds.from + m_eps <= u && u <= bnds.to - m_eps && std::abs(u - x) < prev_prev_step / 2) {
                is_accepted = true;
                step = std::abs(u - x);
            }
        }
        if (!is_accepted) {
            if (x < bnds.length() / 2) {
                u = x + K * (bnds.to - x);
                step = bnds.to - x;
            } else {
                u = x - K * (x - bnds.from);
                step = x - bnds.from;
            }
        }
        if (std::abs(u - x) < m_eps) {
            u = x + m_eps * (std::signbit(u - x) ? 1 : -1);
        }
        double f_u = fn(u);
        if (f_u <= f_x) {
            if (u >= x) {
                bnds.from = x;
            } else {
                bnds.to = x;
            }

            v = w;
            w = x;
            x = u;
            f_v = f_w;
            f_w = f_x;
            f_x = f_u;
        } else {
            if (u >= x) {
                bnds.to = u;
            } else {
                bnds.from = u;
            }

            if (f_u <= f_w || w == x) {
                v = w;
                w = u;
                f_v = f_w;
                f_w = f_u;
            } else if (f_u <= f_v || v == x || v == w) {
                v = u;
                f_v = f_u;
            }
        }
        iter_num++;
    }

    return x;
}
} // namespace min1d