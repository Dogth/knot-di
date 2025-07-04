/** @file Strategy.hpp
 * @brief Заголовочный файл для перечисления Strategy. Перечисление
 * предназначено для указания стратегии создания сервисов.
 * @version 1.0
 *
 * Этот файл содержит определение перечисления Strategy, которое используется
 * для указания стратегии создания сервисов в контейнере Knot.
 */
#ifndef STRATEGY_HPP
#define STRATEGY_HPP

/** @brief Перечисление для стратегий создания сервисов
 * @details Это перечисление используется для указания стратегии создания
 * сервисов в контейнере Knot. Оно определяет две стратегии: SINGLETON и
 * TRANSIENT.
 *
 * @note SINGLETON - сервис, который создается один раз и используется
 * повторно для всех запросов.
 * TRANSIENT - сервис, который создается каждый раз при запросе
 * и уничтожается после использования.
 */
enum Strategy { SINGLETON, TRANSIENT, EXTERNAL, SCOPED };

#endif  // STRATEGY_HPP
