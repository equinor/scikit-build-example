#include <stdexcept>

struct zero_div : public std::domain_error {
    using domain_error::domain_error;
};

template < typename T >
T divide(T lhs, T rhs) noexcept (false) {
    if (rhs == 0)
        throw zero_div("divide-by-zero");

    return lhs / rhs;
}
