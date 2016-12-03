# TODO

Before contributing please read the [contribution guidelines](./Contribution.md).

## Bugs:

### Severity = LOW

- in SQLite: Database with primary keys that dont specify notnull get updated because sqlite
  insists on them being notnull

## Features:

- make `spisdump` (and especially the drivers) detect foreign keys in the database

### SPIS 0.1.3

- add foreign keys to sqlite driver in `ensureTable()`

### SPIS 0.2.0

SPIS language:

- support default values for columns (especially those with notnull constraint)
- add an option to use another name in spis than in the db itself (makes renaming easier,
  currently the old name will be dropped and the new name created)

Core/Driver library:

- implement order by columns other than the primary key
- pass errors to the application instead of just printing them to the command line
- add filters that are usefull but don't exist in SQL like contains or startsWith

Drivers:

- MySQL support (see [mysql](https://github.com/msrd0/SPIS/tree/mysql) branch)

### future

- use qts plugin system instead of linux dynamic linker
- recursive forein key support (a foreign ref to a table containing another foreign ref)
- add support for PostgreSQL and maybe other database servers
- add option for spisdump to not only print schema but also the data inside the table. this can be either
  accomplished by extending qsl language or specifying a backup-file where the data is exported.

qmake problems:

- qmake: dont force the user to use the qtype option
- qmake: if database name is changed we need to manually re-run qmake (cmake uses a static timestamp file)

## Long-Term:

- add support for other languages like Java
- jit-compiler for spis files
- cling-like interpreter to inspect the spis database
