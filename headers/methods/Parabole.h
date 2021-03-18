#pragma once

#include "Function.h"
#include "MinSearcher.h"
#include "ReplayData.h"

namespace min1d {
struct Parabole : public MinSearcher
{
    Parabole(double eps)
        : m_eps(eps)
    {}

    std::string_view method_name() const noexcept override { return "Parabole"; }

    void change_parameters(double new_eps) noexcept { m_eps = new_eps; }

protected:
    /*
     * Find unimodal function's minimum
     * using parabolic interpolation method.
     */
    double find_min_impl() noexcept override;

    /*
     * Find unimodal function's minimum
     * using parabolic interpolation method.
     * Outputs tracing information.
     */
    double find_min_tracked_impl() noexcept override;

    double m_eps; // required accuracy
};
} // namespace min1d
