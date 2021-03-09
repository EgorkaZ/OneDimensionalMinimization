#include "UserInteractor.h"

#include "VersionedData.h"
#include "methods/Brent.h"
#include "methods/Dichotomy.h"
#include "methods/Fibonacci.h"
#include "methods/Golden.h"
#include "methods/Parabole.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
namespace min1d {

namespace chrono = std::chrono;
namespace {

template <class... Ts>
void println(Ts &&... el) { (std::cout << ... << std::forward<Ts>(el)) << std::endl; }

} // namespace

UserInteractor::UserInteractor()
    : m_available_funcs({{"f(x) = x", [](double x) { return x; }, {0., 10.}},
                         {"f(x) = x ^ 2", [](double x) { return x * x; }, {-1., 1.}},
                         {"f(x) = sin(x)", [](double x) { return std::sin(x); }, {3.14, 6.28}},
                         {"f(x) = 10x ln(x) - (x ^ 2) / 2", [](double x) { return 10 * x * std::log(x) - x * x / 2; }, {0.1, 2.5}}})
    , m_available_searchers({new Dichotomy(EPS / 100, EPS),
                             new Golden(EPS),
                             new Fibonacci(EPS),
                             new Parabole(EPS),
                             new Brent(EPS)})
    , m_current_func(m_available_funcs.front())
    , m_current_method(*m_available_searchers.front())
{}

int UserInteractor::run()
{
    println("Usage:");
    println("enter 'funcs' to choose function to count");
    println("enter 'methods' to choose method to use");
    println("enter 'search' to search for min");
    println("enter 'search_traced' to search for min and print tracked data");
    println("enter 'q' to exit");

    std::string in;
    while (std::cout << "> ", std::getline(std::cin, in)) {
        if (in == "funcs") {
            for (uint i = 0; i < m_available_funcs.size(); ++i) {
                std::cout << i << ") " << m_available_funcs[i] << '\n';
            }
            std::cout << "choose (wisely): ";
            uint chosen;
            std::cin >> chosen;
            if (chosen < m_available_funcs.size()) {
                m_current_func = m_available_funcs[chosen];
            }
            std::cout << "you chose: " << m_current_func << '\n';
        } else if (in == "methods") {
            for (uint i = 0; i < m_available_searchers.size(); ++i) {
                std::cout << i << ") " << m_available_searchers[i]->method_name() << '\n';
            }
            std::cout << "choose (wisely): ";
            uint chosen;
            std::cin >> chosen;
            if (chosen < m_available_searchers.size()) {
                m_current_method = *m_available_searchers[chosen];
            }
        } else if (in == "search") {
            double res;
            auto start = chrono::steady_clock::now();
            res = m_current_method.get().find_min(m_current_func);
            auto end = chrono::steady_clock::now();

            println("x = ", res);
            println((end - start).count(), "ns have passed");
        } else if (in == "search_traced") {
            const auto & replay_data = m_current_method.get().find_min_tracked(m_current_func);
            auto callback = overload(
                    [](const VdComment & comm) { println(comm.version(), ": '", comm.comment, "'"); },
                    [](const VdPoint & point) { println(point.version(), ": {x: ", point.x, "; y: ", point.y, "}"); },
                    [](const VdSegment & segment) { println(segment.version(), ": [", segment.l, "; ", segment.r, "]"); },
                    [](const auto & other) { println(other.version(), ": not implemented kind: ", static_cast<int>(other.get_kind())); });
            std::for_each(replay_data.begin(), replay_data.end(), [&callback](auto & ptr) { ptr->call_func(callback); });
        } else if (in == "q") {
            break;
        }
    }
    return 0;
}

} // namespace min1d
