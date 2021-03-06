# GB_UE_Basics // Основы работы с Unreal Engine :video_game:
 Introduction. Preparing for Unreal Engine 4 Development

## Урок 1. Введение. Подготовка к разработке на Unreal Engine 4

- [X] 1. Скачайте и установите Unreal Engine, настройте среду
- [X] 2. Создайте первый проект на основе шаблона ThirdPerson
- [X] 3. Добавьте в проект пакет StarterContent
- [X] 4. Создайте лабиринт, используя стандартные примитивы материалы из установленного пакета

[![Video HW1](https://img.youtube.com/vi/fbO7a53LAKw/0.jpg)](https://youtu.be/fbO7a53LAKw)

## Урок 2. Основные объекты. Жизненный цикл. Перемещение в мире

- [X] 1. Поэкспериментируйте с настройками камеры так, чтоб уровень был виден как можно лучше, но при этом сам танк не терялся из виду
- [X] 2. Добавьте несколько новых уровней из разных шаблонов. Попробуйте настроить их.
- [X] 3. *Попробуйте реализовать движение танка вбок. Завяжите движение на команду MoveRight используя клавиши Q для движения влево, а E - вправо. Для определения направления движения вправо можно воспользоваться методом GetActorRightVector()*

[![Video HW2](https://img.youtube.com/vi/kjDGuU6tjlE/0.jpg)](https://youtu.be/kjDGuU6tjlE)

## Урок 3. Повороты. Создание объектов
- [X] 1. Добавьте орудию альтернативный вариант стрельбы. Допустим, что при нажатии правой кнопки мыши орудие будет стрелять другой логикой, описываемой в методе FireSpecial()
- [X] 2. Добавьте учет количества снарядов. При каждом выстреле запас снарядов уменьшается. Если снарядов не осталось - пушка не стреляет
- [X] 3. *Сделайте возможность стрельбы серией снарядов - то есть сделайте автоматическую пушку. То есть при одном нажатии на левую кнопку мыши будет произведено заданное количество выстрелов с заданными интервалом между ними. Параметры для этой стрельбы задаются в пушке. Новая серия не может быть начата до того, как закончилась текущая серия выстрелов. При такой стрельбе запас снарядов уменьшается на 1 за всю серию*
- [X] 4. *Сделать независимое от частоты кадров сглаживание поворотов и движения*

- Дополнительно:
- [X] +  *Исправлен поворот башни на цель за курсором мыши*

[![Video HW2](https://img.youtube.com/vi/LnMZ6TF3G2A/0.jpg)](https://youtu.be/LnMZ6TF3G2A)
