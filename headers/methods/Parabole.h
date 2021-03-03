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

    std::string_view method_name() const noexcept override
    {
        return "Parabole";
    }

protected:
    double find_min_impl() noexcept override;
    double find_min_tracked_impl() noexcept override;

private:
    template <bool is_tracked>
    double any_find_min() noexcept;

    double m_eps;
};
} // namespace min1d
