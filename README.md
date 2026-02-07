# file_logger

Тестовое задание.

## Build

Запустите `make` из корня репозитория.

## logger_test 

Программа для работы с библиотекой файлового логгера из терминала.

Запуск: 

```bash
./logger_test FILE_NAME LOG_LEVEL
```

Запись:
```bash
msg [LOG_LEVEL] 'MESSAGE'
```

Выставить уровень логирования:
```bash
set LOG_LEVEL
```

`LOG_LEVEL` принимает три значения (в порядке возрастания приоритета): `INFO`, `WARNING` и `ERROR`. 