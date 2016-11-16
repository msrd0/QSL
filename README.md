# QSL

QSL is a SQL Database Library for C++, written on top of Qt.

# The QSL language

The QSL language is a simple line-based language that goes from first to last line. Every line
starting with a `#` will be treated as a comment. Comments at the end of a line are not supported.

## Database definition

Each qsl file describes one database. The file starts with a line containing the name of
the database:

```
database "example"
```

This can be followed by setting database properties. Here is a list of all options and their defaults:

```
charset "utf-8"
#usevar
```

If `usevar` is specified then QSL will use `VARCHAR` instead of `CHAR` if available.

## Table definition

After the database has been defined, you can add tables to it. Every table starts with a line
containing its name:

```
table "example"
```

Every following line describing the table starts with a `-`. The table columns first contain the
data type used. The following datatypes are supported:

 - `int`
 - `uint`
 - `double`
 - `char`
 - `text`
 - `password`
 - `byte`
 - `blob`
 - `date`
 - `time`
 - `datetime`
 - `variant` (if the sql system doesn't support this, the `blob` type will be used)
 
Most of them can be followed by a number in parentheses to specify the minimum size of the
type. Those numbers can also be suffixed with `K`, `M` or `G` to avoid writing to many `0` behind
the number. Note that there is no guarantee that the minimum size is provided by the database.
`password` is a special type that automatically hashes and salts the given text and can later
be used to compare passwords. Note that it is not possible to get the value of the password.

Adding to these default types, you can also refer to other table. This is accomplished by
foreign keys. The syntax to create a foreign reference to another table of the same database
is as follows:

```
# this is the table being referenced
table a
- uint id !primarykey
- ...

# this is the table with the reference
table b
- uint id !primarykey
# the reference - a foreign key called ref references a.id
- &a.id ref
```

The type is followed by the name of the column and finally a list of constraints prefixed with
a `!`. A column definition could look like this:

```
- uint "id" !primarykey
- char(50) "name" !unique !notnull
```

Currently only `int` and `uint` are supported as primary keys. In future versions, more types
might be supported.

## `qsldump`

If you have an existing database and want to generate it's definition file, you can use the
`qsldump` tool. It will connect to the database, load all tables and dump their structure.

```
Usage: qsldump [options] <db-name>
Dump QSL for an existing database

Options:
  -h, --help             Displays this help.
  -v, --version          Displays version information.
  -d, --driver <driver>  The driver used to connect to the database
  -p, --port <port>      The port of the database server if required
  -u, --user <username>  The user used to connect to the database if required
  --password <password>  The password used to connect to the database if
                         required
  --pw                   Ask for a password on the command line
  -o, --out <file>       The file to write the qsl file (if - write to stdout)

Arguments:
  name                   The name (or filename) of the database
```

# Using QSL

QSL will generate a file called `db_<db-name>.h` for every database. To use it, you first have
to connect to a database. See QSLDB for an example how to do so.

Currently, CMake and qmake as the build systems are supported. If you are not using one of them,
you have to invoke `qslc` manually.

## CMake

To use QSL with CMake, just add the following to your `CMakeLists.txt`:

```cmake
find_package(QSL REQUIRED)
include_directories(${QSL_INCLUDE_DIRS})

# add an executable as usual
add_executable(foo foo.cpp)
# compile the qsl files for this executable
qsl_compile(TARGET foo FILES foo.qsl)
```

The `qsl_compile` function will add a target called `foo_qslc` that builds the qsl
files and then adds it as a dependency to the original target.

## qmake

To use QSL with qmake, add the following to your `.pro` file:

```qmake
QT += QSL
DATABASES = foo.qsl
```

## `qslc`

If you don't like to use CMake for your build, you can invoke `qslc` yourself.

```
Usage: qslc [options] <file> [<file> ...]
QSL Compiler

Options:
  -h, --help       Displays this help.
  -v, --version    Displays version information.
  -d, --dir <dir>  The target directory to put the generated files
  -q, --qtype      Use Qt types instead of std:: types
  --name           Print out the header names and exit

Arguments:
  file             The input file(s) to compile
```
