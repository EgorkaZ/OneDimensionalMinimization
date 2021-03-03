#include "methods/Parabole.h"

#include "ReplayData.h"
#include "VersionedData.h"

#include <iostream>

namespace min1d {
double Parabole::find_min_impl() noexcept /*override*/
{
    return any_find_min<false>();
}

double Parabole::find_min_tracked_impl() noexcept /*override*/
{
    return any_find_min<true>();
}

template <bool is_tracked>
double Parabole::any_find_min() noexcept
{
    const auto & fn = last_func();
    auto bnds = fn.bounds();
    uint iter_num = 0;

    double x1 = bnds.from;
    double f1 = fn(x1);
    double x3 = bnds.to;
    double f3 = fn(x3);
    double x2 = (bnds.from + bnds.to) / 2;
    double f2 = fn(x2);

    double exit_delta = m_eps + 1;
    double prev_x = 0;
    while (exit_delta > m_eps) {
        if constexpr (is_tracked) {
            m_replay_data.emplace_back<VdComment>(iter_num, "Points x1, x2, x3 are:");
            m_replay_data.emplace_back<VdPoint>(iter_num, x1, f1);
            m_replay_data.emplace_back<VdPoint>(iter_num, x2, f2);
            m_replay_data.emplace_back<VdPoint>(iter_num, x3, f3);
        }

        double a0 = f1;
        double a1 = (f2 - a0) / (x2 - x1);
        double a2 = ((f3 - a0) / (x3 - x1) - a1) / (x3 - x2);
        double new_x = (x1 + x2 - a1 / a2) / 2;
        double new_f = fn(new_x);

        if constexpr (is_tracked) {
            m_replay_data.emplace_back<VdComment>(iter_num, "The parabola and the x_dash point are:");
            m_replay_data.emplace_back<VdFunction>(iter_num, [=](double x) {
                return a0 + a1 * (x - x1) + a2 * (x - x1) * (x - x2);
            });
            m_replay_data.emplace_back<VdPoint>(iter_num, new_x, new_f);
        }

        if (new_x < x2) {
            if (new_f >= f2) {
                x1 = new_x;
                f1 = new_f;
            } else {
                x3 = x2;
                f3 = f2;
                x2 = new_x;
                f2 = new_f;
            }
        } else {
            if (f2 >= new_f) {
                x1 = x2;
                f1 = f2;
                x2 = new_x;
                f2 = new_f;
            } else {
                x3 = new_x;
                f3 = new_f;
            }
        }

        if (iter_num) {
            exit_delta = abs(new_x - prev_x);
        }
        prev_x = new_x;
        iter_num++;
    }

    if constexpr (is_tracked) {
        m_replay_data.emplace_back<VdComment>(iter_num, "Answer is");
        m_replay_data.emplace_back<VdPoint>(iter_num, prev_x, fn(prev_x));
    }
    return prev_x;
}
} // namespace min1d
