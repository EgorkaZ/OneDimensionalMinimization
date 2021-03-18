#include "methods/Golden.h"

#include "ReplayData.h"
#include "VersionedData.h"

namespace min1d {
double Golden::find_min_impl() noexcept /*override*/
{
    const auto & fn = last_func();
    auto bnds = fn.bounds();

    /*
     * Start with finding first points x_left and x_right using golden ratio coefficient,
     * so that length of [a, x_right] == length of [x_left, b].
     * On each iteration choose the next segment according to function values in current points x_left and x_right.
     * Depending on what segment is chosen, save one point and count another point and function value in it.
     * Break, when the segment's length is less than epsilon (epsilon is required accuracy).
     */
    double x_left = bnds.to - TAU * bnds.length();
    double f_left = fn(x_left);
    double x_right = bnds.from + TAU * bnds.length();
    double f_right = fn(x_right);

    while (bnds.length() > m_eps) {
        if (f_left > f_right) {
            /*
             * Minimum is in the right part
             * Proceed with [x_left, b]
             */
            bnds.from = x_left;

            x_left = x_right; // Save x_right point. It will be x_left in the next segment.
            f_left = f_right;

            x_right = bnds.from + TAU * bnds.length(); // Count new x_right point.
            f_right = fn(x_right);
        } else { //f_left <= f_right
            /*
             * Minimum is in the left part
             * Proceed with [a, x_right]
             */
            bnds.to = x_right;

            x_right = x_left; // Save x_left point. It will be x_right in the next segment.
            f_right = f_left;

            x_left = bnds.to - TAU * bnds.length(); // Count new x_left point.
            f_left = fn(x_left);
        }
    }

    return bnds.middle();
}

double Golden::find_min_tracked_impl() noexcept /*override*/
{
    const auto & fn = last_func();
    auto bnds = fn.bounds();
    uint iter_num = 0;

    double x_left = bnds.to - TAU * bnds.length();
    double f_left = fn(x_left);
    double x_right = bnds.from + TAU * bnds.length();
    double f_right = fn(x_right);

    while (bnds.length() > m_eps) {
        m_replay_data.emplace_back<VdSegment>(iter_num, bnds.from, bnds.to);
        m_replay_data.emplace_back<VdPoint>(iter_num, x_left, f_left);
        m_replay_data.emplace_back<VdPoint>(iter_num, x_right, f_right);

        if (f_left > f_right) {
            bnds.from = x_left;
            x_left = x_right;
            f_left = f_right;
            x_right = bnds.from + TAU * bnds.length();
            f_right = fn(x_right);

            m_replay_data.emplace_back<VdComment>(iter_num, "Chose segment [x1, b]");
        } else { //f_left <= f_right
            bnds.to = x_right;
            x_right = x_left;
            f_right = f_left;
            x_left = bnds.to - TAU * bnds.length();
            f_left = fn(x_left);

            m_replay_data.emplace_back<VdComment>(iter_num, "Chose segment [a, x2]");
        }

        iter_num++;
    }

    double mid = bnds.middle();
    m_replay_data.emplace_back<VdSegment>(iter_num, bnds.from, bnds.to);
    m_replay_data.emplace_back<VdPoint>(iter_num, mid, fn(mid));
    return mid;
}

} // namespace min1d
