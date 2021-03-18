#include "UserInteractor.h"
#include "VersionedData.h"
#include "Misc.h"
#include "ReplayData.h"
#include "methods/Dichotomy.h"

#include "VersionedData.h"

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <variant>
namespace min1d {

namespace chrono = std::chrono;
namespace {

template <class... Ts>
void println(Ts &&... el) { (std::cout << ... << std::forward<Ts>(el)) << std::endl; }

template <class T>
void print_tab(T && el) { std::cout << std::forward<T>(el) << '\t'; }

template <class... Ts>
void print_tabbed(Ts &&... els)
{
    (print_tab(els), ...);
}

constexpr auto format_to_tabbed = overload(
    [](const VdPoint & point) { print_tabbed(point.x, point.y); },
    [](const VdSegment & segment) { print_tabbed(segment.l, segment.r); },
    [](const VdParabole & parabole) { print_tabbed(parabole.a, parabole.b, parabole.c); },
    [](const auto & other) {});

void print_replay_data_table(const ReplayData & data)
{
    ReplayData row;
    std::for_each(data.begin(), data.end() - 1, [&, prev_version(-1)](auto & element_ptr) mutable {
        if (element_ptr->version() != prev_version) {
            prev_version = element_ptr->version();
            std::cout << '\n';
            std::cout << element_ptr->version() << "\t";
        }
        element_ptr->call_func(format_to_tabbed);
    });
    std::cout << '\n';
}

} // namespace

UserInteractor::UserInteractor()
    : m_available_funcs({{"f(x) = 10x ln(x) - (x ^ 2) / 2", [](double x) { return 10 * x * std::log(x) - x * x / 2; }, {0.1, 2.5}}})
    , m_current_method(std::in_place_index<0>, m_eps)
    , m_current_func(m_available_funcs.front())
{}

void UserInteractor::set_method(int new_method) {
    if (new_method < std::variant_size_v<MethodVariant>) {
        #define M(num) case num: m_current_method.emplace<num>(m_eps); break
        switch (new_method) {
            M(0);
            M(2);
            M(3);
            M(4);
        case 1:
            m_current_method.emplace<1>(m_eps / 10., m_eps);
            break;
        }
        #undef M
    }

}

void UserInteractor::set_func(const Function & new_func) {
    m_current_func = new_func;
}

void UserInteractor::set_eps(double new_eps) {
    m_eps = new_eps;
    std::visit(overload(
        [new_eps](Dichotomy & dichotomy) {
            double new_sigma = new_eps / 10.;
            dichotomy.change_parameters(new_eps, new_sigma);
        },
        [new_eps](auto & method) {
            method.change_parameters(new_eps);
        }
    ), m_current_method);
}

double UserInteractor::search_min() {
    return std::visit([this](auto & method) -> double { return method.find_min(m_current_func); }, m_current_method);
}

const ReplayData& UserInteractor::search_traced() {
    return std::visit([this](auto & method) -> const ReplayData & {
        return method.find_min_tracked(m_current_func);
    }, m_current_method);
}

int UserInteractor::run()
{
    println("Usage:");
    println("enter 'funcs' to choose function to count");
    println("enter 'methods' to choose method to use");
    println("enter 'search' to search for min");
    println("enter 'search_traced' to search for min and print tracked data");
    println("enter 'search_table' to search for min and print tracked data in table format");
    println("enter 'change_eps' to change epsilon");
    println("enter 'research' to compare methods and different epsilons on the function.");
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
                set_func(m_available_funcs[chosen]);
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
            set_method(chosen);
            std::visit([](auto & method) { std::cout << "chosen " << method.method_name() << '\n'; }, m_current_method);
        } else if (in == "search") {
            auto start = chrono::steady_clock::now();
            double res = search_min();
            auto end = chrono::steady_clock::now();

            println("x = ", res);
            println((end - start).count(), "ns have passed");
            println(method_last_func().call_count(), " function calls");
        } else if (in == "search_traced") {
            const auto & replay_data = search_traced();

            std::cout << "found\n";

            constexpr auto callback = overload(
                    [](const VdComment & comm) { println(comm.version(), ": '", comm.comment, "'"); },
                    [](const VdPoint & point) { println(point.version(), ": {x: ", point.x, "; y: ", point.y, "}"); },
                    [](const VdSegment & segment) { println(segment.version(), ": [", segment.l, "; ", segment.r, "]"); },
                    [](const VdParabole & parabole) { println(parabole.version(), ": a=", parabole.a, ", b=", parabole.b, ", c=", parabole.c); },
                    [](const auto & other) { println(other.version(), ": not implemented kind: ", static_cast<int>(other.get_kind())); });
            std::for_each(replay_data.begin(), replay_data.end(), [&callback](auto & ptr) { ptr->call_func(callback); });
        } else if (in == "search_table") {
            const auto & replay_data = search_traced();

            print_replay_data_table(replay_data);
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
        } else if (in == "research") {
            std::ofstream out("research.csv");
            out << std::fixed << std::setprecision(std::numeric_limits<double>::digits10 + 1);
            double standard_eps = m_eps;
            for(int method = 0; method < std::variant_size_v<MethodVariant>; method++) {
                set_method(method);
                set_eps(standard_eps);
                std::string method_name = std::visit([](auto & method) -> std::string { return (std::string)method.method_name(); }, m_current_method);
                {
                    out << "Function log for " << method_name << std::endl;
                    uint prev_version = 0;
                    auto callback = overload(
                        [&out](const VdComment & comm)  { out << ",c," << comm.comment; },
                        [&out](const VdPoint & point)  { out << ",p," << point.x << ',' << point.y; },
                        [&out](const VdSegment & segment)  { out << ",s," << segment.l << ',' << segment.r; },
                        [&out](const VdParabole & parabole)  { out << ",f," << parabole.a << ',' << parabole.b << ',' << parabole.c; },
                        [&out](const auto & other)  { out << ",errtype:" << static_cast<int>(other.get_kind()); });
                    constexpr auto get_version = [](const auto & other) -> uint { return other.version(); };
                    const auto& replay_data = search_traced();
                    out << 0;
                    std::for_each(replay_data.begin(), replay_data.end(), [&](auto & ptr) {
                        uint ver = ptr->call_func(get_version);
                        if(ver != prev_version) {
                            out << std::endl << ver;
                            prev_version = ver;
                        }
                        ptr->call_func(callback);
                    });
                    out << std::endl << "Epsilon:," << m_eps << ",Call count:," << method_last_func().call_count() << std::endl;
                }
                {
                    out << "Epsilon comparison for " << method_name << std::endl << "eps,ns,calls" << std::endl;
                    double eps = 1;
                    for(int i = 0; i < 16; i++) {
                        set_eps(eps /= 10);
                        out << m_eps << ',';
                        auto start = chrono::steady_clock::now();
                        double res = search_min();
                        auto end = chrono::steady_clock::now();
                        out << (end - start).count() << ',';
                        out << method_last_func().call_count() << std::endl;
                    }
                    out << std::endl;
                }
            }
        } else if (in == "q") {
            break;
        }
    }
    return 0;
}

} // namespace min1d
