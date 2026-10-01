cd ~/generator-project
cat > README.md <<'EOF'
# Generator — модульная система сборки текстовых данных

## Сборка

    mkdir -p build
    cd build
    cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
    make

## Запуск

Из корня проекта:

    ./build/generator --template templates/template.txt \
                      --data     data/data.json \
                      --output   output/out.txt

## Структура

- `src/`      — исходники основного приложения
- `modules/`  — исходники динамических модулей (собираются в build/modules/*.so)
- `templates/` — шаблоны
- `data/`     — JSON и CSV данные
- `output/`   — сгенерированные файлы

## Синтаксис шаблонов

- `{{key}}` — подстановка значения из JSON
- `{{sum(10,20)}}` — вызов функции из модуля libmath.so
- `{{csv(data/test.csv)}}` — вставка содержимого CSV-файла через libcsv.so

## Модули

Модуль — это .so-файл, экспортирующий функцию:

    extern "C" char* execute(const char* func, const char* args);

Он возвращает malloc'нутую строку с результатом или nullptr,
если функция ему незнакома. Загрузка — через dlopen/dlsym.
Перезагрузка происходит автоматически при изменении mtime .so-файла.
EOF# generator-project
