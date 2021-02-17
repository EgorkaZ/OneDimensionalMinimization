#pragma once

#include "Misc.h"

#include <string>

namespace min1d {

struct Function
{
    struct Bounds { double from, to; };

public:
    Function (std::string as_string, CalculateFunc calculate, Bounds bounds);

    void reset(std::string as_string, CalculateFunc calculate);

    double operator () (double x) const;

    const std::string & to_string() const noexcept;
    friend std::ostream & operator << (std::ostream & out, const Function & func);

private:
    std::string m_as_string;
    CalculateFunc m_calculate;
    Bounds m_bounds;
};

}
