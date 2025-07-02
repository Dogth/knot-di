# Knot-DI

**Knot-DI** - это библиотека для внедрения зависимостей на C++03, ориентированная на встраиваемые, устаревшие или ресурсоограниченные среды, где современные возможности C++ недоступны.

## Возможности

- Только заголовочные файлы: не требует линковки, просто подключите заголовки.
- Совместимость с C++03: работает на старых компиляторах.
- Поддержка жизненных циклов Singleton и Transient
- Пользовательский пул памяти
- Макросы для регистрации сервисов с разным количеством конструкторов

## Ограничения

- Требуется компилятор с поддержкой C++03 (например, GCC, Clang)
- CMake ≥ 3.10
- [Опционально] lcov/genhtml для покрытия, clang-format для форматирования, Doxygen для документации

## Сборка и тестирование

```sh
# Клонируйте и перейдите в репозиторий
git clone <repo-url>
cd knot-di

# Сборка и запуск тестов
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
cd build
ctest --output-on-failure
```

## Покрытие кода

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=ON
cmake --build build
cd build
ctest
make coverage  # Генерирует coverage-report/ с HTML

```

## Форматирование

```sh
cmake --build build --target format
```

## Документация

```sh
cmake --build build --target doc
```

## Пример использования

```cpp
#include <knot-di/Container.hpp>

Knot::Container container;
container.registerService<MyType>(SINGLETON);
MyType* instance = container.resolve<MyType>();
```

Смотрите `tests/ContainerTests.cpp` для дополнительных примеров использования.

## Разработка в среде Nix

Если вы используете [Nix](https://nixos.org):

```sh
nix develop
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
