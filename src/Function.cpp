#include "Function.h"

#include <iostream>

namespace min1d {

Function::Function(std::string as_string, CalculateFunc calculate, Bounds bounds)
    : m_as_string(std::move(as_string))
    , m_calculate(std::move(calculate))
    , m_bounds(bounds)
{}

void Function::reset(std::string as_string, CalculateFunc calculate)
{
    m_as_string = std::move(as_string);
    m_calculate = std::move(calculate);
}

double Function::operator () (double x) const
{ return m_calculate(x); }

const std::string & Function::to_string() const noexcept
{ return m_as_string; }

std::ostream & operator << (std::ostream & out, const Function & func)
{ return out << func.to_string() << " [" << func.m_bounds.from << ", " << func.m_bounds.to << ']'; }

}
