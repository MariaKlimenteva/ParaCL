#!/bin/bash

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
cmake --build build

# Список всех тестов
tests=$(find tests -name "test_*.dat")

# Директория с файлами правильных ответов
correct_answers_dir="correct_answers"

# Директория с файлами ввода
input_files_dir="input_files"

# Существует ли директория с правильными ответами
if [ ! -d "$correct_answers_dir" ]; then
    echo "Директория с правильными ответами не найдена!"
    exit 1
fi

# Есть ли директория с файлами ввода
if [ ! -d "$input_files_dir" ]; then
    echo "Директория с файлами ввода не найдена!"
    exit 1
fi

# Запускаем каждый тест и сравниваем с правильным ответом
for test in $tests
do
    echo "Running test: $test"
    test_name=$(basename "$test")
    output_file="output_${test_name}.txt"
    input_file="$input_files_dir/${test_name/test_/input_}"

    # Проверяем, существует ли файл ввода и не пуст ли он
    if [ -f "$input_file" ] && [ -s "$input_file" ]; then
        ./build/ParaCL "$test" < "$input_file" > "$output_file"
    else
        ./build/ParaCL "$test" > "$output_file"
    fi
    
    correct_answer_file="$correct_answers_dir/${test_name/test_/answer_}"
    if [ ! -f "$correct_answer_file" ]; then
        echo "Файл с правильным ответом для теста $test_name не найден!"
        continue
    fi
    if diff -q "$output_file" "$correct_answer_file" >/dev/null; then
        echo "Тест $test_name пройден успешно!"
    else
        echo "Тест $test_name не пройден: вывод отличается от правильного ответа!"
    fi
done

# Удаляем файлы вывода после завершения работы скрипта
rm -f output_*.txt
