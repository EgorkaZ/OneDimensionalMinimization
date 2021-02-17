#include "UserInteractor.h"

#include <cmath>
#include <iostream>
#include <string>
namespace min1d {

UserInteractor::UserInteractor()
    : m_available_funcs({
            {"f(x) = x", [](double x) { return x; }, {0., 10.}},
            {"f(x) = x ^ 2", [](double x) { return x * x; }, {-1., 1.}},
            {"f(x) = sin(x)", [](double x) { return std::sin(x); }, {3.14, 6.28}}
            })
    , m_current(m_available_funcs.front())
{}

int UserInteractor::run()
{
    std::string in;
    while (std::getline(std::cin, in)) {
        if (in == "funcs") {
            for (uint i = 0; i < m_available_funcs.size(); ++i) {
                std::cout << i << ") " << m_available_funcs[i] << '\n';
            }
            std::cout << "choose (wisely): ";
            uint chosen;
            std::cin >> chosen;
            if (chosen < m_available_funcs.size()) {
                m_current = m_available_funcs[chosen];
            }
            std::cout << "you chose: " << m_current << '\n';
        } else if (in == "q") {
            break;
        }
    }
    return 0;
}

}
