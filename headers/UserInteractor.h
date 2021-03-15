#pragma once

#include "Function.h"
#include "Misc.h"

#include "methods/Brent.h"
#include "methods/Dichotomy.h"
#include "methods/Fibonacci.h"
#include "methods/Golden.h"
#include "methods/Parabole.h"

#include <cstdint>
#include <functional>
#include <variant>
#include <vector>

namespace min1d {

struct UserInteractor
{
    UserInteractor();
    int run();

private:
    using FuncRef = std::reference_wrapper<const Function>;
    using MethodRef = std::reference_wrapper<MinSearcher>;
    using Methods = TypeList<Brent, Dichotomy, Fibonacci, Golden, Parabole>;
    using MethodVariant = FromTypeList<std::variant, Methods>;

    double m_eps = 0.000001;

    const std::vector<Function> m_available_funcs;
    MethodVariant m_current_method;
    FuncRef m_current_func;
};

} // namespace min1d
