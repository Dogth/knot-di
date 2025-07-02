/** @file
 * @brief Заголовочный файл для класса Container. Класс предназначен для
 * управления регистрацией и разрешением сервисов
 * @version 1.0
 *
 * Этот файл содержит определение класса Container, который управляет
 * регистрацией сервисов и их разрешением. Он включает в себя методы для
 * регистрации сервисов, разрешения сервисов, а также управления временными и
 * синглтон сервисами.
 */
#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <cstddef>
#include <new>

#include "ContainerMacros.hpp"
#include "Descriptor.hpp"
#include "Factory.hpp"
#include "MemoryPool.hpp"
#include "Strategy.hpp"
#include "Util.hpp"

#ifndef KNOT_MAX_SERVICES
#define KNOT_MAX_SERVICES 16
#endif

#ifndef KNOT_MAX_TRANSIENTS
#define KNOT_MAX_TRANSIENTS 32
#endif

namespace Knot {

/** @brief Контейнер для управления сервисами
 *
 * Этот класс предоставляет функциональность для регистрации и разрешения
 * сервисов, а также управления их жизненным циклом. Он поддерживает стратегии
 * синглтона и временных сервисов, а также использует MemoryPool для управления
 * памятью.
 */
class Container {
 private:
  Container(const Container&);             // Запрет копирования контейнера
  Container& operator=(const Container&);  // Запрет присваивания контейнера

  size_t m_service_count;    // Количество зарегистрированных сервисов
  size_t m_factory_count;    // Количество зарегистрированных фабрик
  size_t m_transient_count;  // Количество временных сервисов

  MemoryPool m_pool;  // Пул памяти для управления памятью сервисов

  RegistryEntry
      m_registry[KNOT_MAX_SERVICES];  // Реестр зарегистрированных сервисов
  IFactory* m_factories[KNOT_MAX_SERVICES];  // Массив фабрик для сервисов
  TransientInfo m_transients[KNOT_MAX_TRANSIENTS];  // Массив временных сервисов

  /** @brief метод для поиска записи в реестре по идентификатору типа
   * @param tid Указатель на идентификатор типа
   * @return Указатель на найденную запись или nullptr, если запись не найдена
   */
  RegistryEntry* find_entry(void* tid) {
    for (size_t i = 0; i < m_service_count; ++i)
      if (m_registry[i].type == tid) return &m_registry[i];
    return NULL;
  }

  /** @brief метод для регистрации синглтон сервиса
   * @param factory Указатель на фабрику, создающую сервис
   * @tparam T Тип сервиса
   * @return true, если регистрация успешна, иначе false
   */
  template <typename T>
  bool register_singleton(IFactory* factory) {
    size_t size = sizeof(T);
    void* mem = m_pool.allocate<Factory<T>>();
    if (!mem) return false;
    RegistryEntry& entry = m_registry[m_service_count++];
    entry.type = TypeId<T>();
    entry.desc.factory = factory;
    entry.desc.strategy = SINGLETON;
    entry.desc.instance = NULL;
    entry.desc.storage = mem;
    return true;
  }

  /** @brief метод для регистрации временного сервиса
   * @param factory Указатель на фабрику, создающую сервис
   * @tparam T Тип сервиса
   * @return true, если регистрация успешна, иначе false
   */
  template <typename T>
  bool register_transient(IFactory* factory) {
    RegistryEntry& entry = m_registry[m_service_count++];
    entry.type = TypeId<T>();
    entry.desc.factory = factory;
    entry.desc.strategy = TRANSIENT;
    entry.desc.instance = NULL;
    entry.desc.storage = NULL;
    return true;
  }

  /** @brief метод для выделения фабрики для сервиса
   * @tparam T Тип сервиса
   * @return Указатель на созданную фабрику или NULL, если не удалось выделить
   * память
   */
  template <typename T>
  inline IFactory* alloc_factory() {
    void* mem = m_pool.allocate<Factory<T>>();
    if (!mem) return NULL;
    IFactory* factory = new (mem) Factory<T>();
    if (m_factory_count >= KNOT_MAX_SERVICES) return NULL;
    m_factories[m_factory_count++] = factory;
    return factory;
  }

  /** @brief метод для удаления временного сервиса по индексу
   * @param idx Индекс временного сервиса в массиве m_transients
   * @note Этот метод освобождает память, занятую временным сервисом, и вызывает
   * его деструктор.
   */
  inline void destroyTransientAt(size_t idx) {
    if (m_transients[idx].ptr && m_transients[idx].factory)
      m_transients[idx].factory->destroy(m_transients[idx].ptr);
    if (m_transients[idx].ptr)
      m_pool.deallocate(m_transients[idx].ptr, m_transients[idx].alloc_size);
    m_transients[idx].ptr = NULL;
    m_transients[idx].alloc_size = 0;
    m_transients[idx].factory = NULL;
  }

  /** @brief метод для добавления сервиса в контейнер
   * @param strategy Стратегия создания сервиса (SINGLETON или TRANSIENT). По
   * умолчанию SINGLETON.
   * @param factory Указатель на фабрику, создающую сервис
   * @tparam T Тип сервиса
   *
   * @note Inline функция, которая применяетс строго внутри макроса генерации
   * сервисов различной арности.
   */
  template <typename T>
  inline bool addService(Strategy strategy, IFactory* factory) {
    void* tid = TypeId<T>();
    if (!factory || m_service_count >= KNOT_MAX_SERVICES || find_entry(tid))
      return false;
    switch (strategy) {
      case SINGLETON:
        return register_singleton<T>(factory);
        break;
      case TRANSIENT:
        return register_transient<T>(factory);
        break;
      default:
        return false;
    }
  }

  /** @brief Уничтожение всех зарегистрированных фабрик
   * @details Этот метод освобождает память, занятую всеми зарегистрированными
   * фабриками, и вызывает их деструкторы.
   *
   * @note Этот метод вызывается в деструкторе контейнера для освобождения
   * ресурсов.
   */
  inline void destroyAllFactories() {
    for (size_t i = 0; i < m_factory_count; ++i) {
      if (m_factories[i]) {
        m_factories[i]->destroy(m_factories[i]);
        m_pool.deallocate(m_factories[i], sizeof(IFactory*));
        m_factories[i] = NULL;
      }
    }
    m_factory_count = 0;
  }

 public:
  /** @brief Конструктор контейнера
   * @details Создает контейнер с нулевым счетчиком сервисов и временных
   * сервисов, а также инициализирует пул памяти с размером 4096 байт.
   *
   * @note Если требуется использовать другой размер пула памяти,
   * рекомендуется использовать конструктор с параметром max_bytes.
   *
   * @warning Этот конструктор не принимает буфер для пула памяти, поэтому
   * все выделения будут происходить в динамической памяти.
   */
  Container()
      : m_transient_count(0),
        m_factory_count(0),
        m_service_count(0),
        m_pool(4096) {}

  /** @brief Конструктор контейнера с указанием максимального размера пула
   * памяти
   * @details Создает контейнер с нулевым счетчиком сервисов и временных
   * сервисов, а также инициализирует пул памяти с заданным размером.
   * @param max_bytes Максимальный размер пула памяти в байтах.
   *
   * @note Если требуется использовать буфер для пула памяти, рекомендуется
   * использовать конструктор с указанием буфера и его размера.
   *
   * @warning Этот конструктор не принимает буфер для пула памяти, поэтому
   * все выделения будут происходить в динамической памяти.
   */
  Container(size_t max_bytes)
      : m_transient_count(0),
        m_factory_count(0),
        m_service_count(0),
        m_pool(max_bytes) {}

  /** @brief Конструктор контейнера с указанием буфера и его размера
   * @details Создает контейнер с нулевым счетчиком сервисов и временных
   * сервисов, а также инициализирует пул памяти с заданным буфером и его
   * размером.
   * @param buffer Указатель на буфер, который будет использоваться в качестве
   * пула памяти.
   * @param buffer_size Размер буфера в байтах.
   *
   * @note Если требуется использовать пул памяти без буфера, рекомендуется
   * использовать конструктор без параметров или с указанием максимального
   * размера пула памяти.
   */
  template <size_t N>
  Container(uint8_t (&buffer)[N])
      : m_transient_count(0),
        m_factory_count(0),
        m_service_count(0),
        m_pool(buffer) {}

  /** @brief Конструктор контейнера с указанием буфера и его размера, а также
   * его типа. Применяется для инициализации контейнера с фиксированным буфером
   * и определенным выравнением.
   * @details Создает контейнер с нулевым счетчиком сервисов и временных
   * сервисов, а также инициализирует пул памяти с заданным буфером и его
   * размером.
   * @param buffer Указатель на буфер, который будет использоваться в качестве
   * пула памяти.
   * @tparam T Тип буфера, который должен быть массивом фиксированного размера.
   * @param N Размер буфера в элементах.
   *
   * @note Если требуется использовать пул памяти без буфера, рекомендуется
   * использовать конструктор без параметров или с указанием максимального
   * размера пула памяти.
   */
  template <typename T, size_t N>
  Container(T (&buffer)[N])
      : m_transient_count(0),
        m_factory_count(0),
        m_service_count(0),
        m_pool(buffer) {}

  /** @brief Деструктор контейнера
   * @details Освобождает все зарегистрированные сервисы и временные сервисы,
   * вызывая соответствующие методы для уничтожения синглтонов и временных
   * сервисов.
   */
  ~Container() {
    destroyAllSingletons();
    destroyAllTransients();
    destroyAllFactories();
  }

  /** @brief Регистрация сервиса в контейнере
   * @details Этот метод позволяет зарегистрировать сервис в контейнере с
   * заданной стратегией создания (SINGLETON или TRANSIENT). Если сервис уже
   * зарегистрирован, регистрация не будет выполнена.
   * @param strategy Стратегия создания сервиса
   * @tparam T Тип сервиса, который нужно зарегистрировать
   * @return true, если регистрация успешна, иначе false.
   *
   * @note Все сервисы регистрируются через фабрики, которые создаются
   *	внутри этого метода и хранятся в массиве _factories внутри буфера
   *контейнера.
   */
  template <typename T>
  bool registerService(Strategy strategy = SINGLETON) {
    return addService<T>(strategy, alloc_factory<T>());
  }

  /** @brief Регистрация экземпляра сервиса в контейнере
   * @details Этот метод позволяет зарегистрировать уже существующий
   * экземпляр сервиса в контейнере. Экземпляр должен быть создан заранее.
   * @tparam T Тип сервиса, который нужно зарегистрировать
   * @param instance Указатель на экземпляр сервиса
   * @return true, если регистрация успешна, иначе false.
   *
   * @note Экземпляр будет зарегистрирован с стратегией EXTERNAL, что
   * означает, что он не будет уничтожен контейнером при вызове
   * destroyAllSingletons() или destroyAllTransients().
   */
  template <typename T>
  bool registerInstance(T* instance) {
    if (!instance || m_service_count >= KNOT_MAX_SERVICES) return false;
    void* tid = TypeId<T>();
    if (find_entry(tid)) return false;
    RegistryEntry& entry = m_registry[m_service_count++];
    entry.type = tid;
    entry.desc.factory = NULL;
    entry.desc.strategy = EXTERNAL;
    entry.desc.instance = instance;
    entry.desc.storage = NULL;
    return true;
  }

  REGISTER_GEN  // Макрос для регистрации сервисов с различной арностью

      /** @brief Получение зарегистрированного сервиса по его типу
       * @details Этот метод позволяет получить зарегистрированный сервис по
       * его типу.
       * @tparam T Тип сервиса, который нужно получить
       * @return Указатель на сервис типа T, или nullptr, если сервис не
       * зарегистрирован или не может быть создан.
       *
       * @note Если сервис зарегистрирован как SINGLETON, он будет создан при
       * первом вызове resolve и сохранен для последующих вызовов. Если сервис
       * зарегистрирован как TRANSIENT, он будет создан каждый раз при вызове
       * resolve.
       */
      template <typename T>
      T* resolve() {
    RegistryEntry* entry = find_entry(TypeId<T>());
    if (!entry) return NULL;
    Descriptor& desc = entry->desc;
    switch (desc.strategy) {
      case SINGLETON: {
        if (!desc.instance) desc.instance = desc.factory->create(desc.storage);
        return static_cast<T*>(desc.instance);
      }
      case TRANSIENT: {
        if (m_transient_count >= KNOT_MAX_TRANSIENTS) return NULL;
        void* mem = m_pool.allocate<T>();
        if (!mem) return NULL;
        T* ptr = static_cast<T*>(desc.factory->create(mem));
        m_transients[m_transient_count].factory = desc.factory;
        m_transients[m_transient_count].ptr = ptr;
        m_transients[m_transient_count].alloc_size = sizeof(T);
        ++m_transient_count;
        return ptr;
      }
      case EXTERNAL: {
        if (!desc.instance) return NULL;
        return static_cast<T*>(desc.instance);
      }
      default:
        return NULL;
    }
  }

  /** @brief Уничтожение всех синглтон сервисов
   * @note Этот метод освобождает память, занятую всеми синглтон сервисами, и
   * вызывает их деструкторы.
   */
  void destroyAllSingletons() {
    for (size_t i = 0; i < m_service_count; ++i) {
      Descriptor& desc = m_registry[i].desc;
      if (desc.strategy == SINGLETON && desc.instance) {
        size_t alloc_size = sizeof(desc.instance);
        desc.factory->destroy(desc.instance);
        if (desc.storage) {
          m_pool.deallocate(desc.storage, alloc_size);
          desc.storage = NULL;
        }
        desc.instance = NULL;
      }
    }
  }

  /** @brief Уничтожение всех временных сервисов
   * @note Этот метод освобождает память, занятую всеми временными сервисами,
   * и вызывает их деструкторы.
   */
  void destroyAllTransients() {
    for (size_t i = 0; i < m_transient_count; ++i) {
      destroyTransientAt(i);
    }
    m_transient_count = 0;
  }

  /** @brief Уничтожение временного сервиса по указателю
   * @details Этот метод освобождает память, занятую временным сервисом, и
   * вызывает его деструктор.
   * @note Если сервис не найден, ничего не происходит.
   *
   * @tparam T Тип временного сервиса, который нужно уничтожить
   * @param ptr Указатель на временный сервис, который нужно уничтожить
   *
   * @note Этот метод используется для управления жизненным циклом временных
   * сервисов, которые были созданы с помощью метода .resolve().
   */
  template <typename T>
  void destroyTransient(T* ptr) {
    for (size_t idx = 0; idx < m_transient_count; idx++) {
      if (m_transients[idx].ptr == ptr) {
        destroyTransientAt(idx);
        m_transients[idx] = m_transients[m_transient_count - 1];
        m_transient_count--;
        break;
      }
    }
  }
};
}  // namespace Knot

#endif  // CONTAINER_HPP
