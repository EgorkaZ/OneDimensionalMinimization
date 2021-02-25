#pragma once

#include "Function.h"
#include "Misc.h"
#include "methods/MinSearcher.h"

#include <cstdint>
#include <functional>
#include <vector>

namespace min1d {

struct UserInteractor
{
    UserInteractor ();
    int run();
    static constexpr double EPS = 0.000001;

private:
    using FuncRef = std::reference_wrapper<const Function>;
    using MethodRef = std::reference_wrapper<MinSearcher>;

    const std::vector<Function> m_available_funcs;
    std::vector<MinSearcher *> m_available_searchers;
    FuncRef m_current_func;
    MethodRef m_current_method;
};

}
