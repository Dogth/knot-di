#ifndef TYPECOMPARATOR_HPP
#define TYPECOMPARATOR_HPP

#include <typeinfo>

namespace Knot {
struct TypeComparator {
  bool operator()(const std::type_info *lhs, const std::type_info *rhs) const {
    return lhs->before(*rhs) != 0;
  }
};
}; // namespace Knot

#endif // TYPECOMPARATOR_HPP
