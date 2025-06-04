#ifndef FACTORY_HPP
#define FACTORY_HPP

namespace Knot {
class IFactory {
public:
  virtual ~IFactory() {};
  virtual void *create(void *buffer) = 0;
  virtual void destroy(void *instance) = 0;
};
}; // namespace Knot

#endif // FACTORY_HPP
