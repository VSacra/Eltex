Сначала требуется скомпилировать файл, используя make all
<img width="1280" height="800" alt="1" src="https://github.com/user-attachments/assets/fdc4099d-475a-414f-a92d-9a72ed047a3b" />

Затем необходимо вмонтировать образ ядра 
insmod hello.ko
После этого мы можем увидеть наше ядро среди загруженных
lsmod | grep 'hello'
<img width="1280" height="800" alt="2" src="https://github.com/user-attachments/assets/d10c0f54-a334-420c-ba91-0887148fa5fd" />

Проверим работу нашего модуля.
Для начала попробуем прочитать содержимо файла -
он оказывается пустым.
Затем мы можем записывать в файл и его содержание
изменится, что позволяет пространству ядра 
взаимодействовать с пространством пользователя.
<img width="1280" height="800" alt="3" src="https://github.com/user-attachments/assets/bc827db7-e840-4597-af64-a98ea2675960" />

По окончанию работы необходимо вынуть модуль rmmod hello.ko
Файл hello из proc пропадёт
Просмотреть информацию о модуле modinfo hello.ko
