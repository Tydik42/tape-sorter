[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++ CI/CD](https://github.com/Tydik42/tape-sorter/actions/workflows/cpp-ci.yml/badge.svg)](https://github.com/Tydik42/tape-sorter/actions/workflows/cpp-ci.yml)

# Tape sorter

Реализация алгоритма внешней сортировки на C++ для устройства хранения типа Tape.

## Описание

Эта программа имитирует сортировку данных на ленточных устройствах хранения, используя алгоритм внешней сортировки слиянием. Учитывает настроенные через config файл задержки для каждой операции с лентой.
## Формат входных данных

Входной файл должен содержать целые числа типа `int32_t`, разделенные пробелами или переносами строк. Например:
```
112 42 35 0 1239 -1238 914 392 13 2
```

## Конфигурация

Вы можете настроить задержки для операций с лентой с помощью config файла. Формат:
```
read_delay=<milliseconds>
write_delay=<milliseconds>
rewind_delay=<milliseconds>
move_delay=<milliseconds>
```

Пример config файла:
```
read_delay=5
write_delay=10
rewind_delay=50
move_delay=2
```

## Сборка проекта

### Предварительные требования
- Компилятор, совместимый с C++20 (gcc, clang, MSVC и т.д.)
- CMake 3.24 или выше

### Шаги сборки
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Запуск программы

```bash
./tape-sorter --input <input-file> --output <output-file>
```

### Опции
- `-i, --input FILE` - Input tape file (required)
- `-o, --output FILE` - Output tape file (required)
- `-c, --config FILE` - Configuration file (default is 0 delay for all operations)
- `-b, --block-size SIZE` - Memory block size (default: 32)
- `-h, --help` - Show help message

### Пример
```bash
./tape-sorter --input example/input.txt --output output.txt --config example/config.txt
```

## Тестирование

### Сборка тестов
Для включения тестов:
```bash
cd build
cmake -DENABLE_TESTS=ON ..
cmake --build . 
```

### Запуск тестов
После сборки с включенными тестами:
```bash
ctest --output-on-failure
```