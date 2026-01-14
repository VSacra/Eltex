После того, как модуль ядра будет вмонтирован,
в системный лог будет поступать информация о пакетах,
получаемых по IP.
По умолчанию все пакеты пропускаются.
Чтобы добавить IP в чёрный список неоходимо
записать этот IP в /sys/kernel/ip_blacklist/add
После этого пакеты будут BLOCKED
Чтобы убрать IP из чёрного списка необходимо
записать IP в sys/kernel/ip_blacklist/remove
<img width="1280" height="800" alt="6" src="https://github.com/user-attachments/assets/cb49d714-d835-42f4-bf39-dae68e05d14a" />
