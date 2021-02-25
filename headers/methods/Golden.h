#pragma once

#include "Function.h"
#include "MinSearcher.h"
#include "ReplayData.h"
#include <cmath>

namespace min1d {

struct Golden : public MinSearcher
{
    static constexpr double PHI_L = (3 - sqrt(5)) / 2;
    static constexpr double PHI_R = (sqrt(5) - 1) / 2;

    Golden(double eps)
        : m_eps(eps)
    {}


    std::string_view method_name() const noexcept override
    { return "Golden ratio"; }

    protected:
    double find_min_impl() noexcept override;
    double find_min_tracked_impl() noexcept override;

private:
    template <bool is_tracked>
    double any_find_min() noexcept;

    double m_eps;
};

}
