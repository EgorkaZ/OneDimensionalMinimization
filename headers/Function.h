#pragma once

#include "Misc.h"

#include <string>

namespace min1d {

struct Function
{
    struct Bounds 
    { 
        double from, to; 

        double length() noexcept
        { return to - from; }
    };

public:
    Function (std::string as_string, CalculateFunc calculate, Bounds bounds);

    void reset(std::string as_string, CalculateFunc calculate);

    /*
     * Calculate function's value in point x
     */ 
    double operator () (double x) const
    { return m_calculate(x); }

    Bounds bounds() const noexcept
    { return m_bounds; }

    const std::string & to_string() const noexcept
    { return m_as_string; }

    friend std::ostream & operator << (std::ostream & out, const Function & func);

private:
    std::string m_as_string;
    CalculateFunc m_calculate;
    Bounds m_bounds;
};

}
