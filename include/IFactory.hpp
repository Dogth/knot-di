#ifndef FACTORY_HPP
#define FACTORY_HPP

namespace Knot {
class IFactory {
public:
  virtual ~IFactory() {};
  virtual void *create() = 0;
  virtual void deleteInstance(void *instance) = 0;
};
}; // namespace Knot

#endif // FACTORY_HPP
