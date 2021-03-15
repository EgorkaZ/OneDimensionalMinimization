#include "UserInteractor.h"
#include "VersionedData.h"
#include "ReplayData.h"
#include "methods/Dichotomy.h"

#include "VersionedData.h"

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <variant>
namespace min1d {

namespace chrono = std::chrono;
namespace {

template <class... Ts>
void println(Ts &&... el) { (std::cout << ... << std::forward<Ts>(el)) << std::endl; }

} // namespace

UserInteractor::UserInteractor()
    : m_available_funcs({{"f(x) = 10x ln(x) - (x ^ 2) / 2", [](double x) { return 10 * x * std::log(x) - x * x / 2; }, {0.1, 2.5}}})
    , m_current_method(std::in_place_type<Dichotomy>, m_eps, m_eps / 10)
    , m_current_func(m_available_funcs.front())
{}

int UserInteractor::run()
{
    println("Usage:");
    println("enter 'funcs' to choose function to count");
    println("enter 'methods' to choose method to use");
    println("enter 'search' to search for min");
    println("enter 'search_traced' to search for min and print tracked data");
    println("enter 'change_eps' to change epsilon");
    println("enter 'q' to exit");

    std::cout << std::setprecision(std::numeric_limits<double>::digits10 + 1);
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
            println("0) Brent");
            println("1) Dichotomy");
            println("2) Fibonacci");
            println("3) Golden ratio");
            println("4) Parabole");
            std::cout << "choose (wisely): ";
            uint chosen;
            std::cin >> chosen;
            if (chosen < std::variant_size_v<MethodVariant>) {
#define M(num) case num: m_current_method.emplace<num>(m_eps); break
                switch (chosen) {
                    M(0);
                    M(2);
                    M(3);
                    M(4);
                case 1:
                    m_current_method.emplace<1>(m_eps, m_eps / 10.);
                    break;
                }
            }

            std::visit([](auto & method) { std::cout << "chosen " << method.method_name() << '\n'; }, m_current_method);
        } else if (in == "search") {
            double res;
            auto start = chrono::steady_clock::now();
            std::visit([&res, this](auto &  method) { res = method.find_min(m_current_func); }, m_current_method);
            auto end = chrono::steady_clock::now();

            println("x = ", res);
            println((end - start).count(), "ns have passed");
        } else if (in == "search_traced") {
            const auto & replay_data = std::visit([this](auto & method) -> const ReplayData & { return method.find_min_tracked(m_current_func); }, m_current_method);
            auto callback = overload(
                    [](const VdComment & comm) { println(comm.version(), ": '", comm.comment, "'"); },
                    [](const VdPoint & point) { println(point.version(), ": {x: ", point.x, "; y: ", point.y, "}"); },
                    [](const VdSegment & segment) { println(segment.version(), ": [", segment.l, "; ", segment.r, "]"); },
                    [](const VdParabole & parabole) { println(parabole.version(), ": a=", parabole.a, ", b=", parabole.b, ", c=", parabole.c); },
                    [](const auto & other) { println(other.version(), ": not implemented kind: ", static_cast<int>(other.get_kind())); });
            std::for_each(replay_data.begin(), replay_data.end(), [&callback](auto & ptr) { ptr->call_func(callback); });
        } else if (in == "change_eps") {
            std::cout << "enter new eps: ";
            double new_eps;
            std::cin >> new_eps;
            std::visit(overload(
                [new_eps](Dichotomy & dichotomy) {
                    std::cout << "enter new sigma (or 0 for default eps / 10):";
                    double new_sigma;
                    std::cin >> new_sigma;
                    if (new_sigma == 0.) {
                        new_sigma = new_eps / 10.;
                    }
                    dichotomy.change_parameters(new_eps, new_sigma);
                },
                [new_eps](auto & method) {
                    method.change_parameters(new_eps);
                }
            ), m_current_method);
        } else if (in == "q") {
            break;
        }
    }
    return 0;
}

} // namespace min1d
