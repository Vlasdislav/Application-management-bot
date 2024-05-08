# Application-management-bot
## Управление заявками в информационной системе предприятия через телеграм-бота
https://t.me/AppManagementTestTaskBot
## Сборка и запуск
Перед тем как собрать и запустить проект, необходимо установить две сторонние библиотеки `SQLiteCpp` и `tgbot-cpp`.
```
bash build
bash start.sh
```
## Команды бота
```
/start - Начало работы
/view - Просмотр заявок по табельному номеру
/view_all - Просмотр всех заявок
/add - Добавить новую заявку
/work - Взять в работу заявку
```