# TODO

Before contributing please read the [contribution guidelines](./Contribution.md).

## Bugs:

- in SQLite: `QSqlQuery::value: unknown field name 'origin'`
- in SQLite: Database with foreign keys gets updated because "type changed"
- qmake: if database name is changed we need to manually re-run qmake (cmake uses a static timestamp file)

## Features:

- add support for PostgreSQL, MySQL and maybe other database servers
- recursive forein key support (a foreign ref to a table containing another foreign ref)
- implement order by columns other than the primary key
- add filters that are usefull but don't exist in SQL like contains or startsWith
- pass errors to the application instead of just printing them to the command line
- use qts plugin system instead of linux dynamic linker
- qmake: dont force the user to use the qtype option
