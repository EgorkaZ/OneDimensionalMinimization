#pragma once

#include "Misc.h"

#include <cassert>
#include <string>

namespace min1d {

enum struct VdDataKind
{
    VdPointKind, VdFunctionKind, VdSegmentKind, VdCommentKind,
};

struct VdPoint;
struct VdFunction;
struct VdSegment;
struct VdComment;

struct VersionedData
{
    explicit VersionedData(uint version)
        : m_version(version)
    {}
    virtual ~VersionedData() = default;

    virtual VdDataKind get_kind() const noexcept = 0;

    template <class Func>
    auto call_func(Func && func) const
    {
        #define M(type) case VdDataKind::type ## Kind: return func(reinterpret_cast<const type &>(*this))

        switch (get_kind()) {
            M(VdPoint);
            M(VdFunction);
            M(VdSegment);
            M(VdComment);
            default: assert(false && "There is no such VersionedData kind");
        }

        #undef M
    }

    uint version() const noexcept
    { return m_version; }
protected:
    uint m_version;
};

// Do we really need this?
struct VdPoint : VersionedData
{
    VdPoint(uint version, double x, double y)
        : VersionedData(version)
        , x(x)
        , y(y)
    {}

    VdDataKind get_kind() const noexcept override
    { return VdDataKind::VdPointKind; }

    double x, y;
};

struct VdFunction : VersionedData
{
    VdFunction(uint version, CalculateFunc func)
        : VersionedData(version)
        , m_func(std::move(func))
    {}

    VdDataKind get_kind() const noexcept override
    { return VdDataKind::VdFunctionKind; }

    double operator () (double x) const noexcept
    { return m_func(x); }

private:
    CalculateFunc m_func;
};

struct VdSegment : VersionedData
{
    VdSegment(uint version, double l, double r)
        : VersionedData(version)
        , l(l)
        , r(r)
    {}

    VdDataKind get_kind() const noexcept override
    { return VdDataKind::VdSegmentKind; }

    double l, r;
};

struct VdComment : VersionedData
{
    VdComment(uint version, std::string comment)
        : VersionedData(version)
        , comment(std::move(comment))
    {}

    VdDataKind get_kind() const noexcept override
    { return VdDataKind::VdCommentKind; }

    std::string comment;
};

}
