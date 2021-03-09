#include "methods/Dichotomy.h"

#include "ReplayData.h"
#include "VersionedData.h"

namespace min1d {

double Dichotomy::find_min_impl() noexcept /*override*/
{
    const auto & fn = last_func();
    auto bnds = fn.bounds();

    /*
     * Start with finding the middle point of segment.
     * On each iteration count x_left and x_right, which are (mid - sigma) and (mid + sigma) respectively.
     * Choose the next segment according to function values in points x_left and x_right.
     * Break, when the segment's length is less than epsilon (epsilon is required accuracy).
     */
    double mid;
    for (mid = bnds.middle(); bnds.length() > m_eps; mid = bnds.middle()) {
        double x_left = mid - m_sigma;
        double f_left = fn(x_left);
        double x_right = mid + m_sigma;
        double f_right = fn(x_right);

        if (f_left < f_right) {
            /*
             * Minimum is in the left part
             * Proceed with [a, x_right]
             */
            bnds.to = x_right;
        } else { // f_left >= f_right
            /*
             * Minimum is in the right part
             * Proceed with [x_left, b]
             */
            bnds.from = x_left;
        }
    }
    return mid;
}

double Dichotomy::find_min_tracked_impl() noexcept /*override*/
{
    const auto & fn = last_func();
    auto bnds = fn.bounds();
    uint iter_num = 0;

    double left, right, f_left, f_right, mid;
    for (mid = bnds.middle(); bnds.length() > m_eps; mid = bnds.middle()) {
        left = mid - m_sigma;
        right = mid + m_sigma;
        f_left = fn(left);
        f_right = fn(right);

        m_replay_data.emplace_back<VdSegment>(iter_num, bnds.from, bnds.to);
        m_replay_data.emplace_back<VdPoint>(iter_num, left, fn(left));
        m_replay_data.emplace_back<VdPoint>(iter_num, right, fn(right));

        const bool goes_left = f_left < f_right;
        if (goes_left) { // goes down to the left
            bnds.to = right;
        } else { // goes down to the right
            bnds.from = left;
        }
        m_replay_data.emplace_back<VdComment>(iter_num, (goes_left ? "goes left" : "goes right"));

        ++iter_num;
    }
    m_replay_data.emplace_back<VdPoint>(iter_num, mid, fn(mid));

    return mid;
}

} // namespace min1d
