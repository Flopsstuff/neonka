# TKB — Teensy Keyboard Bridge for IBM Wheelwriter 6

Подключение к IBM Wheelwriter 6 через **Option Interface** порт с помощью Teensy 4.0.

## Цель проекта

Превратить IBM Wheelwriter 6 в управляемый принтер/терминал — отправлять текст на печать с компьютера и (в перспективе) читать нажатия клавиш машинки.

## Оборудование

| Компонент | Описание |
|---|---|
| IBM Wheelwriter 6 | Электронная печатная машинка с дейзи-колесом |
| Teensy 4.0 | Микроконтроллер (ARM Cortex-M7, 600 МГц), S/N 19175250 |
| Кабель/адаптер | Подключение к Option Interface разъёму на задней панели машинки |

## Option Interface

IBM Wheelwriter 6 имеет на задней панели разъём **Option Interface** — проприетарный последовательный интерфейс (internal bus), по которому клавиатура общается с механизмом печати. Протокол использует:

- **Сигнал:** последовательная передача данных, ~187.5 kbaud (инвертированный, open-collector)
- **Линии:** Bus (данные), +5V, GND
- **Формат команд:** пакеты по 3 байта (команда + параметры)
- **Логические уровни:** 5V (Teensy 4.0 — 3.3V, нужен level shifter или резистивный делитель)

## Структура проекта

```
tkb/
├── README.md
├── firmware/          # Прошивка Teensy (Arduino/PlatformIO)
├── host/              # Утилиты на стороне компьютера
├── docs/              # Документация по протоколу Wheelwriter
└── hardware/          # Схемы подключения, распиновка
```

## Статус

В начале разработки.

## Ссылки

- [PJRC Teensy 4.0](https://www.pjrc.com/store/teensy40.html)
- [Wheelwriter bus protocol (Beihl)](http://www.interlockroc.org/2012/07/02/wheelwriter-speak/)
- [IBM Wheelwriter hack (Hacker's Delight)](https://github.com/acediac/WheelWriter)
