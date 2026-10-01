#!/bin/bash
cd "$(dirname "$0")/.."

PASS=0
FAIL=0

# Тест 1: базовый запуск
./build/generator --template templates/template.txt \
                  --data     data/data.json \
                  --output   output/out.txt > /dev/null 2>&1
if grep -q "Hello, Иван!" output/out.txt && \
   grep -q "Sum: 30"    output/out.txt && \
   grep -q "a,b,c"      output/out.txt; then
    echo "OK: базовый запуск"
    PASS=$((PASS+1))
else
    echo "FAIL: базовый запуск"
    FAIL=$((FAIL+1))
fi

# Тест 2: отсутствующий шаблон
if ./build/generator --template no_such.txt --data data/data.json \
                     --output output/x.txt > /dev/null 2>&1; then
    echo "FAIL: несуществующий шаблон должен падать"
    FAIL=$((FAIL+1))
else
    echo "OK: несуществующий шаблон -> ошибка"
    PASS=$((PASS+1))
fi

# Тест 3: неизвестная переменная
echo "{{unknown_var}}" > templates/bad.txt
if ./build/generator --template templates/bad.txt --data data/data.json \
                     --output output/x.txt > /dev/null 2>&1; then
    echo "FAIL: неизвестная переменная должна падать"
    FAIL=$((FAIL+1))
else
    echo "OK: неизвестная переменная -> ошибка"
    PASS=$((PASS+1))
fi

echo
echo "PASS: $PASS, FAIL: $FAIL"
exit $FAIL
