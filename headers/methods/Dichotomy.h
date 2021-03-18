#pragma once

#include "Function.h"
#include "MinSearcher.h"
#include "ReplayData.h"

namespace min1d {

struct Dichotomy : public MinSearcher
{
    Dichotomy(double sigma, double eps)
        : m_sigma(sigma)
        , m_eps(eps)
    {}

    std::string_view method_name() const noexcept override { return "Dichotomy"; }

    void change_parameters(double new_eps, double new_sigma) noexcept { m_eps = new_eps; m_sigma = new_sigma; }

protected:
    /*
     * Find unimodal function's minimum
     * using dichotomy method.
     */
    double find_min_impl() noexcept override;

    /*
     * Find unimodal function's minimum
     * using dichotomy method.
     * Outputs tracing information.
     */
    double find_min_tracked_impl() noexcept override;

private:
    double m_sigma; // method's parameter
    double m_eps;   // required accuracy
};

} // namespace min1d
