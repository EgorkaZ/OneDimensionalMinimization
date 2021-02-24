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
private:
    template <class Container>
    using ContValType = typename std::decay_t<Container>::value_type;

    

    template <class T>
    static constexpr bool IsVersionedData = std::is_base_of_v<VersionedData, T>;
public:
    using VdDataPtr = std::unique_ptr<const VersionedData>;
    using iterator = std::vector<VdDataPtr>::const_iterator;

    iterator begin() const
    { return m_data.begin(); }
    iterator end() const
    { return m_data.end(); }

    template <class Container>
    auto push_back(Container && cont) -> std::enable_if_t<
                                            is_container_v<std::decay_t<Container>> &&
                                            IsVersionedData<ContValType<Container>>>
    {
        if constexpr (std::is_rvalue_reference_v<Container &&>) {
            push_back_cont(Container{std::move(cont)}); // this way 'cont' will get moved from and thus cleared
        } else {
            push_back_cont(std::forward<Container>(cont));
        }
    }

    template <class Container>
    auto push_back(Container && cont) -> std::enable_if_t<
                                            is_container_v<std::decay_t<Container>> &&
                                            is_unique_ptr_v<ContValType<Container>> &&
                                            IsVersionedData<typename ContValType<Container>::element_type>>
    {
        static_assert(std::is_rvalue_reference_v<Container &&>, "Container of unique_ptr's should be moved");
        push_back_cont(Container{std::move(cont)}); // this way 'cont' will get moved from and thus cleared
    }

    template <class VdData>
    auto push_back(VdData && vd_data) -> std::enable_if_t<std::is_base_of_v<VersionedData, std::decay_t<VdData>>>
    {
        using ValueType = std::decay_t<VdData>;

        m_data.emplace_back(std::make_unique<const ValueType>(std::forward<VdData>(vd_data)));
        m_total_versions = std::max(m_total_versions, vd_data.version());
    }
private:
    template <class Container>
    void push_back_cont(Container && cont)
    {
        m_data.reserve(m_data.size() + cont.size());
        for (auto & el : cont) {
            m_data.emplace_back(to_unique_ptr(to_same_ref_as(el, std::forward<Container>(cont))));
            m_total_versions = std::max(m_total_versions, el.version());
        }
    }

    template <class T>
    static auto to_unique_ptr(T && from)
    {
        if constexpr (is_unique_ptr_v<T>) {
            std::move(from);
        } else {
            return std::make_unique<std::decay_t<T>>(std::forward<T>(from));
        }
    }

private:
    std::vector<VdDataPtr> m_data;
    uint m_total_versions = 0;
};

}
