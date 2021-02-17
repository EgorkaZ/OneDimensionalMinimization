#pragma once

#include "Function.h"
#include "Misc.h"

#include <cstdint>
#include <iterator>
#include <iostream>
#include <memory>
#include <type_traits>

namespace min1d {

enum struct VdDataKind
{
    VdPointKind, VdFunctionKind,
};

struct VdPoint;
struct VdFunction;

struct VersionedData
{
    virtual ~VersionedData() = default;

    virtual VdDataKind get_kind() const noexcept = 0;

    template <class Func>
    auto call_func(Func && func)
    {
        #define M(type) case VdDataKind::type ## Kind: return func(reinterpret_cast<type &>(*this))

        switch (get_kind()) {
            M(VdPoint);
            M(VdFunction);
        }

        #undef M
    }

    uint version() const noexcept
    { return m_version; }
private:
    uint m_version;
};

struct VdPoint : VersionedData
{
    VdDataKind get_kind() const noexcept override
    { return VdDataKind::VdPointKind; }

    double x, y;
};

struct VdFunction : VersionedData
{
    VdFunction(CalculateFunc func)
        : m_func(std::move(func))
    {}

    VdDataKind get_kind() const noexcept override
    { return VdDataKind::VdFunctionKind; }
private:
    CalculateFunc m_func;
};

struct ReplayData
{
    using VdDataPtr = std::unique_ptr<const VersionedData>;
    using iterator = std::vector<VdDataPtr>::const_iterator;

    iterator begin() const
    { return m_data.begin(); }
    iterator end() const
    { return m_data.end(); }

    template <class Container>
    auto push_back(Container && cont) -> std::enable_if_t<
                                            is_container_v<std::decay_t<Container>> &&
                                            std::is_base_of_v<VersionedData, typename std::decay_t<Container>::value_type>>
    {
        using ValueType = typename std::decay_t<Container>::value_type;

        m_data.reserve(m_data.size() + std::distance(cont.begin(), cont.end()));
        for (auto & el : cont) {
            if constexpr (std::is_rvalue_reference_v<Container &&>) {
                m_data.emplace_back(std::make_unique<ValueType>(std::move(el)));
            } else {
                m_data.emplace_back(std::make_unique<ValueType>(el));
            }
            m_total_versions = std::max(m_total_versions, el.version());
        }

        if constexpr (std::is_rvalue_reference_v<Container &&>) {
            cont.clear();
        }
    }

    template <class VdData>
    auto push_back(VdData && vd_data) -> std::enable_if_t<std::is_base_of_v<VersionedData, std::decay_t<VdData>>>
    {
        using ValueType = std::decay_t<VdData>;

        m_data.emplace_back(std::make_unique<const ValueType>(std::forward<VdData>(vd_data)));
        m_total_versions = std::max(m_total_versions, vd_data.version());
    }

private:
    std::vector<VdDataPtr> m_data;
    uint m_total_versions = 0;
};

}
