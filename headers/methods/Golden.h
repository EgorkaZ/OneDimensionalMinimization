#pragma once

#include "Function.h"
#include "MinSearcher.h"
#include "ReplayData.h"

#include <cmath>

namespace min1d {

struct Golden : public MinSearcher
{
    static inline const double TAU = (sqrt(5) - 1) / 2; // golden ratio coefficient

    Golden(double eps)
        : m_eps(eps)
    {}


    std::string_view method_name() const noexcept override { return "Golden ratio"; }

    void change_parameters(double new_eps) noexcept { m_eps = new_eps; }

protected:
    /*
     * Find unimodal function's minimum
     * using golden ratio method.
     */
    double find_min_impl() noexcept override;

    /*
     * Find unimodal function's minimum
     * using golden ratio method.
     * Outputs tracing information.
     */
    double find_min_tracked_impl() noexcept override;

    double m_eps; // required accuracy
};

} // namespace min1d
