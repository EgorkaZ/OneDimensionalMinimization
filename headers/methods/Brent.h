#pragma once

#include "Function.h"
#include "MinSearcher.h"
#include "ReplayData.h"

#include <cmath>

namespace min1d {

struct Brent : public MinSearcher
{
    static inline const double TAU = (3 - sqrt(5)) / 2; // coefficient as in gloden ratio

    Brent(double eps)
        : m_eps(eps)
    {}


    std::string_view method_name() const noexcept override { return "Brent"; }

protected:
    /*
     * Find unimodal function's minimum
     * using Brent's method.
     */
    double find_min_impl() noexcept override;

    /*
     * Find unimodal function's minimum
     * using Brent's method.
     * Outputs tracing information.
     */
    double find_min_tracked_impl() noexcept override;

    double m_eps; // required accuracy
};

} // namespace min1d
