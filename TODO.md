# TODO

Before contributing please read the [contribution guidelines](./Contribution.md).

## Bugs:

- in SQLite: Database with foreign keys gets updated because "type changed"
- qmake: if database name is changed we need to manually re-run qmake (cmake uses a static timestamp file)

## Features:

### SPIS 0.1.3

- qmake: dont force the user to use the qtype option

### SPIS 0.2.0

SPIS language:

- support default values for columns (especially those with notnull constraint)
- add an option to use another name in spis than in the db itself (makes renaming easier,
  currently the old name will be droppend and the new name created)

Core/Driver library:

- implement order by columns other than the primary key
- pass errors to the application instead of just printing them to the command line
- add filters that are usefull but don't exist in SQL like contains or startsWith
- use qts plugin system instead of linux dynamic linker

Drivers:

- recursive forein key support (a foreign ref to a table containing another foreign ref)


### future

- add support for PostgreSQL, MySQL and maybe other database servers


## Long-Term:

- add support for other languages like Java
- jit-compiler for spis files
- cling-like interpreter to inspect the spis database
