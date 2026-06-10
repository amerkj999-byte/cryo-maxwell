[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Language: C++](https://img.shields.io/badge/Language-C%2B%2B-blue.svg)]()
[![Verilog](https://img.shields.io/badge/FPGA-Verilog-green.svg)]()
[![FEA: PASS](https://img.shields.io/badge/FEA-PASS-brightgreen.svg)]()
[![Thermal: 40% margin](https://img.shields.io/badge/Thermal-40%25_margin-brightgreen.svg)]()
[![Chemistry: SAFE](https://img.shields.io/badge/Chemistry-SAFE-brightgreen.svg)]()
[![Stroke: 120mm](https://img.shields.io/badge/Stroke-120%20mm-orange.svg)]()
[![Force: 2016 N](https://img.shields.io/badge/Pinning_Force-2016%20N-red.svg)]()
[![Drop: 4.5m](https://img.shields.io/badge/Max_Drop-4.5%20m-critical.svg)]()
[![Temp: 40K](https://img.shields.io/badge/Operating-40%20K-blue.svg)]()
[![CAD: KOMPAS-3D](https://img.shields.io/badge/CAD-KOMPAS--3D%20v16-yellow.svg)]()

FAQ
<details> <summary><b>Почему 40 K? Нельзя ли теплее?</b></summary>
YBCO переходит в сверхпроводящее состояние при 92 K. Но сила пиннинга растёт с понижением температуры. При 77 K (жидкий азот) сила пиннинга ~5 Н/см² — слишком мало. При 40 K — 15 Н/см². Компромисс между холодопроизводительностью криокулера и силой удержания.

</details><details> <summary><b>Можно ли без криокулера? Залить жидкий азот и поехать?</b></summary>
Можно, но сила пиннинга упадёт втрое (с 2016 Н до ~670 Н). Дроп без пробоя снизится с 4.5 м до ~1.5 м. Для трейла — ок. Для DH — нет.

</details><details> <summary><b>Что будет, если отключится питание?</b></summary>
YBCO нагреется, сверхпроводимость пропадёт, магнит ляжет на сапфировые втулки. Вилка станет жёсткой, но управляемой. После восстановления питания — снова левитация.

</details><details> <summary><b>Что при пробое? Это опасно?</b></summary>
При дропе >5 м магнитная подушка пробивается. Шток бьёт в сапфировые втулки. Сапфир может треснуть. YBCO может треснуть. Вилка требует переборки. Для человека — опаснее сама высота дропа, а не отказ вилки.

</details><details> <summary><b>Почему не используется в велоспорте?</b></summary>
Масса (4.5 кг vs 2.5 кг у Fox 40), время запуска (15 мин), энергопотребление (50 Вт), цена (~$15k). Для DH-гонки — неконкурентоспособно. Для космоса, криогенного оборудования, прецизионной виброизоляции — оптимально.

</details><details> <summary><b>Какой ресурс? Когда менять?</b></summary>
Ресурс левитационного узла — не ограничен (нет контактного износа). Сильфон Inconel 718 — замена при 70 000 циклов. Сапфировые втулки — замена при трещинах (осмотр каждые 10 000 циклов). Криокулер — ресурс ~10 000 часов (замена каждые 3–5 лет).

</details><details> <summary><b>Можно ли напечатать на 3D-принтере?</b></summary>
STL-файлы готовы. Но для работы нужен криокулер, YBCO-плитки, магниты N55, сапфировые втулки, вакуумный пост. 3D-печать — только для визуализации и проверки собираемости.Так же,присутсвует текстовый чертеж.

</details>

## Быстрый старт (Компиляция C++)

Инструкция по сборке и запуску симуляций и управляющего ПО из папки `src`.

### Требования
Убедитесь, что в системе установлен компилятор C++ с поддержкой стандарта не ниже C++11 (рекомендуется GCC или Clang):
* **Linux:** `sudo apt install build-essential`
* **macOS:** `xcode-select --install`
* **Windows:** Установите [Visual Studio](https://microsoft.com) с компонентом "Разработка классических приложений на C++".

### Сборка и запуск

1. **Клонируйте репозиторий и перейдите в папку с кодом:**
   ```bash
   git clone https://github.com
   cd cryo-maxwell/src
   ```

2. **Скомпилируйте нужный модуль:**

   * **Основной симулятор вилки:**
     ```bash
     g++ -O3 CRYO-MAXWELL.cpp -o cryo_maxwell
     ```
   * **Версия без сборки (No Assembly):**
     ```bash
     g++ -O3 cryo-maxwellnoassembly.cpp -o cryo_no_assembly
     ```
   * **Расчет магнитного поля (Halbach FEA):**
     ```bash
     g++ -O3 fea_halbach_magnetostatic.cpp -o fea_magnetostatic
     ```
   * **Расчет прочности/удара сапфировых втулок:**
     ```bash
     g++ -O3 fea_sapphire_impact.cpp -o fea_impact
     ```
   *(Флаг `-O3` обязателен для высокой скорости математических вычислений FEA).*

3. **Запустите скомпилированный файл:**
   ```bash
   ./cryo_maxwell
   ```


