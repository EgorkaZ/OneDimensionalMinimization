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

protected:
    double find_min_impl() noexcept override;
    double find_min_tracked_impl() noexcept override;

private:
    template <bool is_tracked>
    double any_find_min() noexcept;

private:
    double m_sigma;
    double m_eps;
};

} // namespace min1d
