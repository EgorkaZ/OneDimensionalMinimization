#include "methods/Dichotomy.h"

#include "ReplayData.h"
#include "VersionedData.h"

namespace min1d {

double Dichotomy::find_min_impl() noexcept /*override*/ { return any_find_min<false>(); }

double Dichotomy::find_min_tracked_impl() noexcept /*override*/ { return any_find_min<true>(); }

template <bool is_tracked>
double Dichotomy::any_find_min() noexcept // TODO: more optimized implementation for non-tracked implementation
{
    const auto & fn = last_func();
    auto bnds = fn.bounds();

    uint iter_num = 0;
    double left, right, f_left, f_right, mid;
    for (mid = (bnds.from + bnds.to) / 2; bnds.length() > m_eps; mid = (bnds.from + bnds.to) / 2) {

        left = mid - m_sigma;
        right = mid + m_sigma;
        f_left = fn(left);
        f_right = fn(right);

        if constexpr (is_tracked) {
            m_replay_data.emplace_back<VdSegment>(iter_num, bnds.from, bnds.to);
            m_replay_data.emplace_back<VdPoint>(iter_num, left, fn(left));
            m_replay_data.emplace_back<VdPoint>(iter_num, right, fn(right));
        }

        const bool goes_left = f_left < f_right;
        if (goes_left) { // goes down to the left
            bnds.to = right;
        } else { // goes down to the right
            bnds.from = left;
        }

        if constexpr (is_tracked) {
            m_replay_data.emplace_back<VdComment>(iter_num, (goes_left ? "goes left" : "goes right"));
        }

        ++iter_num;
    }

    if constexpr (is_tracked) {
        m_replay_data.emplace_back<VdPoint>(iter_num, mid, fn(mid));
    }

    return mid;
}

} // namespace min1d
