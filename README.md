# Knot-DI

Библиотека для инъекции зависимостей на C++ нацеленная на встраиваемые системы

## Возможности

- Регистрация и разрешение зависимостей по типу
- Поддержка singleton и transient сервисов
- Простое API для внедрения зависимостей в классы
- Минимальные внешние зависимости
- Контроль памяти

## Пример использования

```cpp
Container container;

container.register<IMyService>();
container.register<IRepository>(TRANSIENT, "repository");

IMyService *service = container.resolve<IMyService>();
IMyRepository *repository = container.resolve<IRepository>();

```

## Диаграмма классов

```mermaid
classDiagram
class Container {
-size_t \_service_count
-size_t \_factory_count
-size_t \_transient_count
-MemoryPool \_pool
-RegistryEntry \_registry[KNOT_MAX_SERVICES]
-IFactory* \_factories[KNOT_MAX_SERVICES]
-TransientInfo \_transients[KNOT_MAX_TRANSIENTS]
+Container()
+Container(size_t max_bytes)
+Container(void* buffer, size_t buffer_size)
+~Container()
+registerService<T>(Strategy)
+resolve<T>()
+destroyAllSingletons()
+destroyAllTransients()
+destroyTransient<T>(T\*)
}

    class RegistryEntry {
        +void* type
        +Descriptor desc
    }

    class Descriptor {
        +IFactory* factory
        +Strategy strategy
        +void* instance
        +void* storage
    }

    class IFactory {
        <<interface>>
        +~IFactory()
        +create(void* buffer)
        +destroy(void* instance)
    }

    class Factory~T~ {
        +create(void* buffer)
        +destroy(void* instance)
    }

    class TransientInfo {
        +void* ptr
        +IFactory* factory
        +size_t alloc_size
    }

    class MemoryPool {
        +MemoryPool(size_t)
        +MemoryPool(void*, size_t)
        +void* allocate(size_t, size_t, size_t* = nullptr)
        +void deallocate(void*, size_t)
        +void reset()
        +size_t getUsedBytes()
        +size_t getMaxBytes()
        +size_t getBufferOffset()
    }

    class Strategy {
        <<enum>>
        SINGLETON
        TRANSIENT
    }

    Container "1" o-- "*" RegistryEntry
    RegistryEntry "1" o-- "1" Descriptor
    Descriptor "1" o-- "1" Strategy
    Descriptor "1" o-- "1" IFactory
    Container "1" o-- "*" IFactory
    Container "1" o-- "*" TransientInfo
    Container "1" o-- "1" MemoryPool
    TransientInfo "1" o-- "1" IFactory
    Factory~T~ --|> IFactory
```
