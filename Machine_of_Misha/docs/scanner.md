# scanner.md

## 1. Общее описание назначения кода
Модуль `scanner` реализует систему управления координатным столом с роторами. Обеспечивает:
- Линейное перемещение по осям X и Z (в мм)
- Прецизионное вращение сканера и стола (в шагах)
- Комбинированные движения
- Обработку концевых выключателей
- Возврат в нулевую позицию

## 2. API модуля

### Класс Scanner
```cpp
class Scanner {
public:
    // Конструктор: инициализация моторов и параметров системы
    Scanner(Motor x_motor, Motor z_motor, Motor scanner_motor, Motor table_motor,
            STEP_SCALER scanner_step_scaler, STEP_SCALER table_step_scaler,
            size_t x_stop_pin, size_t z_stop_pin);
    
    // Инициализация системы (обязательна к вызову перед использованием)
    void init();
    
    // Линейное перемещение: x,z - расстояние в мм
    void move(int32_t x, int32_t z);
    
    // Вращение сканера: degree - количество шагов (1 шаг = 360/SCALER градусов)
    void rotate(int32_t steps);
    
    // Вращение стола: degree - количество шагов
    void table_rotate(int32_t steps);
    
    // Комбинированное перемещение и вращение сканера
    void move_and_rotate(int32_t x, int32_t z, int32_t s_steps);
    
    // Комбинированное перемещение и вращение стола
    void move_and_rotate_table(int32_t x, int32_t z, int32_t t_steps);
    
    // Полное комбинированное движение
    void move_and_rotate_scanner_and_table(int32_t x, int32_t z, 
                                         int32_t s_steps, int32_t t_steps);
    
    // Возврат линейных осей в нулевую позицию
    void move_to_zero();
    
    // Возврат сканера в нулевую позицию
    void rotate_to_zero();
    
    // Возврат стола в нулевую позицию
    void table_rotate_to_zero();
};
```

### Пользовательские типы данных
```cpp
enum STEP_SCALER {
    SCALER_128 = 128, // 1 шаг = 360/128 = 2.8125 градусов
};
```

## 3. Подробное описание функций

### `Scanner::Scanner(...)`
- **Назначение**: Создает объект управления сканирующей системой
- **Параметры**:
  - x_motor, z_motor: Моторы линейных осей
  - scanner_motor, table_motor: Моторы роторов
  - scanner_step_scaler, table_step_scaler: Делители шага для роторов
  - x_stop_pin, z_stop_pin: Пины концевых выключателей
- **Инициализирует**:
  - Скорости по умолчанию (DEFAULT_AXIS_SPEED, DEFAULT_ROTOR_SPEED)
  - Коэффициенты преобразования мм/шаг и градус/шаг

### `void Scanner::init()`
- **Назначение**: Полная инициализация системы
- **Выполняет**:
  1. Инициализацию всех моторов
  2. Настройку пинов концевых выключателей (INPUT_PULLUP)
  3. Привязку прерываний
  4. Автоматический возврат в нулевую позицию
- **Обязателен**: Должен быть вызван один раз в setup()

### `void Scanner::rotate(int32_t steps)`
- **Назначение**: Вращение сканера на заданное количество шагов
- **Параметры**:
  - steps: Количество шагов (положительное - по часовой, отрицательное - против)
- **Преобразование**:
  - Угол поворота = steps * (360 / SCALER_128) градусов
- **Особенности**:
  - Автоматически аккумулирует положение для последующего возврата в ноль
  - Использует текущую скорость (DEFAULT_ROTOR_SPEED)

### `void Scanner::move_to_zero()`
- **Назначение**: Возврат линейных осей в нулевую позицию
- **Алгоритм**:
  1. Движение до срабатывания концевых выключателей
  2. Коррекция позиции после срабатывания
  3. Сброс флагов прерываний
- **Блокирующий**: Не возвращает управление пока не достигнет нуля

## 4. Пример программы (исправленный)
```cpp
#include "scanner.h"

// Конфигурация пинов
#define X_STOP_PIN 2
#define Z_STOP_PIN 3

// Создание моторов
Motor x_motor(8, 9, 10, STEP_8);        // Ось X
Motor z_motor(11, 12, 13, STEP_8);     // Ось Z
Motor scanner_motor(4, 5, 6, STEP_NEMA); // Сканер
Motor table_motor(14, 15, 16, STEP_57HS100); // Стол

// Создание сканера (SCALER_128: 1 шаг = 2.8125 градусов)
Scanner scanner(x_motor, z_motor, scanner_motor, table_motor,
                SCALER_128, SCALER_128, X_STOP_PIN, Z_STOP_PIN);

void setup() {
    scanner.init(); // Обязательная инициализация!
}

void loop() {
    // Перемещение по X на 10 мм, по Z на 5 мм
    scanner.move(10, 5);
    delay(1000);
    
    // Поворот сканера на 90 градусов (90 / 2.8125 = 32 шага)
    scanner.rotate(32);
    delay(1000);
    
    // Комбинированное движение:
    // X: -5 мм, Z: -3 мм, Сканер: -45 градусов (16 шагов)
    scanner.move_and_rotate(-5, -3, -16);
    delay(1000);
    
    // Возврат всех осей в нулевое положение
    scanner.move_to_zero();
    scanner.rotate_to_zero();
    scanner.table_rotate_to_zero();
    delay(5000);
}
```