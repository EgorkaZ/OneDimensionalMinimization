#include "methods/Fibonacci.h"

#include "ReplayData.h"
#include "VersionedData.h"

#include <vector>

namespace min1d {

double Fibonacci::find_min_impl() noexcept /*override*/
{
    const auto & fn = last_func();
    auto bnds = fn.bounds();

    /*
     * Count and store fibonacci numbers for future use. 
     * Also count number of iterations needed to get desired accuracy.
     * Stop condition is (b_0 - a_0) / epsilon < F_n (epsilon is required accuracy, n is number of iterations).
     */
    const double limit = bnds.length() / m_eps;
    std::vector<double> fib = {1, 1}; // type is "double" to avoid casts in future
    uint n = fib.size();
    while (fib.back() < limit) {
        fib.emplace_back(fib[n - 1] + fib[n - 2]);
        n++;
    }
    n--; // so that fib[n] is valid

    /*
     * Start with finding first points x_left and x_right using (n - 2)th and (n - 1)th Fibonacci numbers.
     * On each iteration choose the next segment according to function values in current points x_left and x_right.
     * Depending on what segment is chosen, save one point and count another point and function value in it.
     * Number of iterations is precounted earlier.
     */
    double x_left = bnds.from + fib[n - 2] / fib[n] * bnds.length();
    double x_right = bnds.from + fib[n - 1] / fib[n] * bnds.length();
    double f_left = fn(x_left);
    double f_right = fn(x_right);

    for (int k = 1; k < n - 2; k++) {
        if (f_left > f_right) {
            /*
             * Minimum is in the right part
             * Proceed with [x_left, b]
             */
            bnds.from = x_left;

            x_left = x_right; // Save x_right point. It will be x_left in the next segment.
            f_left = f_right;

            x_right = bnds.from + fib[n - k - 1] / fib[n - k] * bnds.length(); // Count new x_right point.
            f_right = fn(x_right);
        } else { //f_left <= f_right
             /*
              * Minimum is in the left part
              * Proceed with [a, x_right]
              */
            bnds.to = x_right;

            x_right = x_left; // Save x_left point. It will be x_right in the next segment.
            f_right = f_left;

            x_left = bnds.from + fib[n - k - 2] / fib[n - k] * bnds.length(); // Count new x_left point.
            f_left = fn(x_left);
        }
    }
    return bnds.middle();
}

double Fibonacci::find_min_tracked_impl() noexcept /*override*/
{
    const auto & fn = last_func();
    auto bnds = fn.bounds();

    const double limit = bnds.length() / m_eps;
    std::vector<double> fib = {1, 1}; //double to avoid casts in future
    uint n = fib.size();
    while (fib.back() < limit) {
        fib.emplace_back(fib[n - 1] + fib[n - 2]);
        n++;
    }
    n--; // so that fib[n] is valid

    std::string comment = "The limit is ";
    comment += std::to_string(limit);
    comment += "; the biggest fibonacci number is ";
    comment += std::to_string(fib.back());
    m_replay_data.emplace_back<VdComment>(0, comment);


    double x_left = bnds.from + fib[n - 2] / fib[n] * bnds.length();
    double x_right = bnds.from + fib[n - 1] / fib[n] * bnds.length();
    double f_left = fn(x_left);
    double f_right = fn(x_right);

    for (int k = 1; k < n - 2; k++) {
        m_replay_data.emplace_back<VdSegment>(k, bnds.from, bnds.to);
        m_replay_data.emplace_back<VdPoint>(k, x_left, f_left);
        m_replay_data.emplace_back<VdPoint>(k, x_right, f_right);

        if (f_left > f_right) {
            bnds.from = x_left;
            x_left = x_right;
            f_left = f_right;
            x_right = bnds.from + fib[n - k - 1] / fib[n - k] * bnds.length();
            f_right = fn(x_right);

            m_replay_data.emplace_back<VdComment>(k, "Chose segment [x1, b]");
        } else // f_left <= f_right
        {
            bnds.to = x_right;
            x_right = x_left;
            f_right = f_left;
            x_left = bnds.from + fib[n - k - 2] / fib[n - k] * bnds.length();
            f_left = fn(x_left);

            m_replay_data.emplace_back<VdComment>(k, "Chose segment [a, x2]");
        }
    }

    double mid = bnds.middle();
    m_replay_data.emplace_back<VdSegment>(n - 2, bnds.from, bnds.to);
    m_replay_data.emplace_back<VdPoint>(n - 2, mid, fn(mid));
    return mid;
}

} // namespace min1d
