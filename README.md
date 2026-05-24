# ДЗ 3. Паралельне обчислення послідовностей Колатца

## Що реалізовано

Програма генерує натуральні числа в головному потоці та паралельно обчислює для кожного числа кількість кроків до виродження в 1 за гіпотезою Колатца.

Після завершення програма рахує:

- кількість оброблених чисел
- кількість потоків
- загальну кількість кроків
- середню кількість кроків
- загальний час виконання
- час на одне число

Результат записується у файл:

```text
results/collatz_parallel_results.csv
```

Щоб не було простоїв між потоками, використано динамічну видачу роботи через спільний атомарний індекс. Потоки беруть числа блоками, тому швидші потоки автоматично отримують нову роботу.

## Структура

```text
.
├── Makefile
├── README.md
├── src
│   └── main.cpp
└── results
    └── collatz_parallel_results.csv
```

## Швидка перевірка

```bash
make
./build/hw3_collatz_parallel --quick
```

або:

```bash
make run
```

## Повний запуск для здачі

```bash
make
./build/hw3_collatz_parallel --full
```

У повному режимі програма обробляє числа від 1 до 10 000 000.

## Ручне налаштування кількості потоків

```bash
./build/hw3_collatz_parallel --full --threads 8
```

## Завантаження на GitHub

```bash
git init
git branch -M main
git add .
git commit -m "Add homework 3 parallel Collatz"
git remote add origin https://github.com/YOUR_LOGIN/hw3-collatz-parallel.git
git push -u origin main
```
