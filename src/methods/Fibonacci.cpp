#include "methods/Fibonacci.h"

#include "ReplayData.h"
#include "VersionedData.h"

#include <vector>

namespace min1d {

double Fibonacci::find_min_impl() noexcept /*override*/ { return any_find_min<false>(); }

double Fibonacci::find_min_tracked_impl() noexcept /*override*/ { return any_find_min<true>(); }

template <bool is_tracked>
double Fibonacci::any_find_min() noexcept
{
    const auto & fn = last_func();
    auto bnds = fn.bounds();

    const double limit = bnds.length() / m_eps;
    std::vector<long long int> fib = {1, 1};
    uint n = fib.size();
    while (fib.back() < limit) {
        fib.emplace_back(fib[n - 1] + fib[n - 2]);
        n++;
    }
    n--; // so that fib[n] is valid
    if constexpr (is_tracked) {
        std::string comment = "The limit is ";
        comment += std::to_string(limit);
        comment += "; the biggest fibonacci number is ";
        comment += std::to_string(fib.back());
        m_replay_data.emplace_back<VdComment>(0, comment);
    }

    double x_left = bnds.from + static_cast<double>(fib[n - 2]) / fib[n] * bnds.length();
    double x_right = bnds.from + static_cast<double>(fib[n - 1]) / fib[n] * bnds.length();
    double f_left = fn(x_left);
    double f_right = fn(x_right);

    for (int k = 1; k < n - 2; k++) {
        if constexpr (is_tracked) {
            m_replay_data.emplace_back<VdSegment>(k, bnds.from, bnds.to);
            m_replay_data.emplace_back<VdPoint>(k, x_left, f_left);
            m_replay_data.emplace_back<VdPoint>(k, x_right, f_right);
        }

        if (f_left > f_right) // aka step 2
        {
            bnds.from = x_left;
            x_left = x_right;
            f_left = f_right;
            x_right = bnds.from + static_cast<double>(fib[n - k - 1]) / fib[n - k] * bnds.length();
            f_right = fn(x_right);

            if constexpr (is_tracked) {
                m_replay_data.emplace_back<VdComment>(k, "Chose segment [x1, b]");
            }
        } else // f_left <= f_right aka step 3
        {
            bnds.to = x_right;
            x_right = x_left;
            f_right = f_left;
            x_left = bnds.from + static_cast<double>(fib[n - k - 2]) / fib[n - k] * bnds.length();
            f_left = fn(x_left);

            if constexpr (is_tracked) {
                m_replay_data.emplace_back<VdComment>(k, "Chose segment [a, x2]");
            }
        }
    }

    double mid = (bnds.from + bnds.to) / 2;
    if constexpr (is_tracked) {
        m_replay_data.emplace_back<VdSegment>(n - 2, bnds.from, bnds.to);
        m_replay_data.emplace_back<VdPoint>(n - 2, mid, fn(mid));
    }
    return mid;
}
} // namespace min1d