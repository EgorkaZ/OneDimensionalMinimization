#include "methods/Parabole.h"

#include "ReplayData.h"
#include "VersionedData.h"

namespace min1d {

    namespace
    {
        /*
         * Find minimum of parabole that goes through three given points
         * Pre: p1.first < p2.first < p3.first && p1.second >= p2.second && p2.second <= p3.second
         */
        double count_parabole(std::pair<double, double> p1, std::pair<double, double> p2, std::pair<double, double> p3)
        {
            double a0 = p1.second;
            double a1 = (p2.second - a0) / (p2.first - p1.first);
            double a2 = ((p3.second - a0) / (p3.first - p1.first) - a1) / (p3.first - p2.first);

            return (p1.first + p2.first - a1 / a2) / 2;
        }
    } // anonymous namespace
double Parabole::find_min_impl() noexcept /*override*/
{
    const auto & fn = last_func();
    auto bnds = fn.bounds();
    const uint ITER_MAX = 100;

    /*
     * Start with finding three points x1 < x2 < x3, so that f(x1) >= f(x2) and f(x2) <= f(x3)
     * On each iteration find quadratic polynomial q = ax^2 + bx + c, so that
     * q(x1) = f(x1), q(x2) = f(x2), q(x3) = f(x3)
     * After that find minimum (we will call it new_x) of this parabola using coefficients a, b, c.
     * Choose next segment according to new_x and function value in it.
     * Break, when the difference between new_x got on this iteration and previous new_x is less than epsilon.
     */
    double x1 = bnds.from;
    double f1 = fn(x1);
    double x3 = bnds.to;
    double f3 = fn(x3);
    double x2;

    /*
     * Though we make a small step, we guarantee f(x1) >= f(x2) <= f(x3) because of unimodality
     */
    if (f1 < f3) {
        x2 = x1 + m_eps;
    } else {
        x2 = x3 - m_eps;
    }
    double f2 = fn(x2);

    double prev_x;
    bool is_first_iteration = true;
    for (uint iter = 0; iter < ITER_MAX; iter++) {
        double new_x = count_parabole({x1, f1}, {x2, f2}, {x3, f3});
        if (!is_first_iteration && std::abs(new_x - prev_x) <= m_eps) {
            prev_x = new_x;
            break;
        }
        double new_f = fn(new_x);

        if (new_x < x2) {
            /*
             * Minimum of the parabole is to the left from middle point x2
             */
            if (new_f >= f2) {
                /*
                 * Minimum of the function is to the right from new_x
                 * Proceed with [new_x, x3].
                 * Next three points are new_x, x2, x3
                 */
                x1 = new_x;
                f1 = new_f;
            } else { // new_f < f2
                /*
                 * Minimum of the function is to the left from new_x
                 * Proceed with [x1, x2]
                 * Next three points are x1, new_x, x2
                 */
                x3 = x2;
                f3 = f2;
                x2 = new_x;
                f2 = new_f;
            }
        } else { // new_x >= x2
            /*
             * Minimum of the parabole is to the left from middle point x2
             */
            if (f2 >= new_f) {
                /*
                 * Minimum of the function is to the right from new_x
                 * Proceed with [x2, x3].
                 * Next three points are x2, new_x, x3
                 */
                x1 = x2;
                f1 = f2;
                x2 = new_x;
                f2 = new_f;
            } else { // f2 < new_f
                /*
                 * Minimum of the function is to the left from new_x
                 * Proceed with [x1, new_x].
                 * Next three points are x1, x2, new_x
                 */
                x3 = new_x;
                f3 = new_f;
            }
        }

        prev_x = new_x;
        is_first_iteration = false;
    }

    return prev_x;
}

double Parabole::find_min_tracked_impl() noexcept /*override*/
{
    const auto & fn = last_func();
    auto bnds = fn.bounds();
    const uint ITER_MAX = 100;
    uint iter_num = 0;

    double x1 = bnds.from;
    double f1 = fn(x1);
    double x3 = bnds.to;
    double f3 = fn(x3);
    double x2;
    if (f1 < f3) {
        x2 = x1 + m_eps;
    } else {
        x2 = x3 - m_eps;
    }
    double f2 = fn(x2);

    double prev_x;
    for (; iter_num < ITER_MAX; iter_num++) {
        m_replay_data.emplace_back<VdComment>(iter_num, "Points x1, x2, x3 are:");
        m_replay_data.emplace_back<VdPoint>(iter_num, x1, f1);
        m_replay_data.emplace_back<VdPoint>(iter_num, x2, f2);
        m_replay_data.emplace_back<VdPoint>(iter_num, x3, f3);

        double a0 = f1;
        double a1 = (f2 - a0) / (x2 - x1);
        double a2 = ((f3 - a0) / (x3 - x1) - a1) / (x3 - x2);
        double new_x = (x1 + x2 - a1 / a2) / 2;
        if (iter_num && std::abs(new_x - prev_x) <= m_eps) {
            prev_x = new_x;
            break;
        }
        double new_f = fn(new_x);

        m_replay_data.emplace_back<VdComment>(iter_num, "The parabole and its minimum are:");
        m_replay_data.emplace_back<VdParabole>(iter_num, a2, a1 - a2 * x1 - a2 * x2, a0 - a1 * x1 + a2 * x1 * x2);
        m_replay_data.emplace_back<VdPoint>(iter_num, new_x, new_f);

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

        prev_x = new_x;
    }

    m_replay_data.emplace_back<VdComment>(iter_num, "Answer is");
    m_replay_data.emplace_back<VdPoint>(iter_num, prev_x, fn(prev_x));
    return prev_x;
}

} // namespace min1d
