#pragma once

#include "Function.h"
#include "Misc.h"

#include <cstdint>
#include <functional>
#include <vector>

namespace min1d {

struct UserInteractor
{
    UserInteractor ();
    int run();

private:
    using FuncRef = std::reference_wrapper<const Function>;

    const std::vector<Function> m_available_funcs;
    FuncRef m_current;
};

}
