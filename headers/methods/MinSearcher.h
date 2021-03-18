#pragma once

#include "Function.h"
#include "ReplayData.h"

#include <ostream>
#include <string_view>

namespace min1d {

struct MinSearcher
{
    virtual ~MinSearcher() = default;

    double find_min(Function func)
    {
        m_last_func.emplace(std::move(func));
        m_last_func.reset();
        return find_min_impl();
    }
    const ReplayData & find_min_tracked(Function func)
    {
        m_last_func.emplace(std::move(func));
        m_replay_data.clear();
        m_last_func.reset();
        find_min_tracked_impl();
        return m_replay_data;
    }

    const ReplayData & replay_data() const noexcept { return m_replay_data; }

    const Function & last_func() const noexcept { return *m_last_func; }

public:
    virtual std::string_view method_name() const noexcept = 0;

protected:
    virtual double find_min_impl() noexcept = 0;
    virtual double find_min_tracked_impl() noexcept = 0;

protected:
    ReplayData m_replay_data;
    std::optional<Function> m_last_func;
};

} // namespace min1d
