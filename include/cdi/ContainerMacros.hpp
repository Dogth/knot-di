#define KNOT_REGISTER_FACTORY_1(ContainerType, ServiceType, Dep1) \
  ServiceType* ServiceType##_factory() {                          \
    return new ServiceType(ContainerType::resolve<Dep1>());       \
  }                                                               \
  ContainerType::registerFactory<ServiceType>(ServiceType##_factory);

#define KNOT_REGISTER_FACTORY_2(ContainerType, ServiceType, Dep1, Dep2) \
  ServiceType* ServiceType##_factory() {                                \
    return new ServiceType(ContainerType::resolve<Dep1>(),              \
                           ContainerType::resolve<Dep2>());             \
  }                                                                     \
  ContainerType::registerFactory<ServiceType>(ServiceType##_factory);

#define KNOT_REGISTER_FACTORY_3(ContainerType, ServiceType, Dep1, Dep2, Dep3) \
  ServiceType* ServiceType##_factory() {                                      \
    return new ServiceType(ContainerType::resolve<Dep1>(),                    \
                           ContainerType::resolve<Dep2>(),                    \
                           ContainerType::resolve<Dep3>());                   \
  }                                                                           \
  ContainerType::registerFactory<ServiceType>(ServiceType##_factory);
