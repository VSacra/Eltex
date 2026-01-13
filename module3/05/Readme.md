Тестостировка программы
Задание 
Запустить программу в фоновом режиме (myprogram &). Узнать 
идентификатор процесса и протестировать команды управления (SIGINT, 
SIGQUIT, SIGABRT, SIGKILL, SIGTERM, SIGTSTP, SIGSTOP, SIGCONT). 
Для сдачи задания нужно прислать исходный код программы и скриншоты с 
комментариями (что тестируется, какое результат вы ожидаете и что 
фактически получилось).

Для начала просто запустим программу и проверим 
Заканчивается ли программа только при 3 SIGINIT 
и игнорирует ли SIGQUIT (просто записывая, что сигнал был)
![3m1](https://github.com/user-attachments/assets/aaf8f371-c7a1-4996-8637-c92dddd3207a)
Программе сначала был отправлен сигнал SIGQUIT сочетанием клавиш Ctrl+/
а затем 3 сигнала SIGINIT. Как видим, первые два SIGINIT и правда были
проигнорированы, а на 3 - программа завершилась
![3m2](https://github.com/user-attachments/assets/9edc5dde-e360-41ae-ba48-b9bea64324df)
Также можем просмотреть файл output.txt
Как видим - сигнал SIGQUIT поступал и был проигнорирован, точно также как первые 2 SIGINIT

Теперь будем запускать программу и поочерёдно будет подавать ей сигналы
(SIGABRT, SIGKILL, SIGTERM, SIGTSTP, SIGSTOP, SIGCONT)
Ожидаемые результаты:
SIGABRT - программа завершится
SIGKILL - программа завершится
SIGTERM - программа завершится
SIGTSTP - программа приостановится
SIGSTOP - программа приостановится
SIGCONT - программа возобновится после приостановления

SIGABRT - программа завершилась. Сообщения о завершении не было
![3m3](https://github.com/user-attachments/assets/0773927a-f2b3-492c-8ba8-564da10bbaeb)

SIGKILL - программа также завершилась без предупреждения
![3m4](https://github.com/user-attachments/assets/6f6fab2f-fdad-4ec2-8100-b3cc5180fb2d)

SIGTERM - программа завершилась, сообщения не было
![3m5](https://github.com/user-attachments/assets/99c99b6b-af91-474a-8a24-574106496046)

SIGTSTP - программа приостановилась, но не завершилась (это может быть понятно 
с вызова pidof)
![3m6](https://github.com/user-attachments/assets/f1c258b7-d6b6-4a62-b028-043fe955b080)
![3m7pidof](https://github.com/user-attachments/assets/92999f52-f37f-4109-9600-829c930a3f80)

SIGSTOP - программа приостанавливается, но не завершается
![3m7](https://github.com/user-attachments/assets/e0d18a09-e818-4e83-a0c0-4b6354e4e44a)
![3m8](https://github.com/user-attachments/assets/85780dd7-2df2-4c90-9db5-c0227cc7dbd2)

SIGCONT - программа продолжает работу после приостановки.
![3mSTOP](https://github.com/user-attachments/assets/a332d8d4-5658-4b02-baf4-bb108285e476)
![3mCONT](https://github.com/user-attachments/assets/6747e7c5-7210-4017-bf27-ab825f5c8699)

В фоновом режиме программа точно также реагирует на SIGINT и SIGQUIT,
как при обычном запуске
![SIGINT](https://github.com/user-attachments/assets/9f6f7038-6d7c-45c6-9c11-64b60e091cce)
![CAT](https://github.com/user-attachments/assets/c1f83fde-2956-470c-9fb4-93e0b1077ba0)




