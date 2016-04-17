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
 
Most of them can be followed by a number in parentheses to specify the minimum size of the
type. Those numbers can also be suffixed with `K`, `M` or `G` to avoid writing to many `0` behind
the number. Note that there is no guarantee that the minimum size is provided by the database.
`password` is a special type that automatically hashes and salts the given text and can later
be used to compare passwords. Note that it is not possible to get the value of the password.

The type is followed by the name of the column and finally a list of constraints prefixed with
a `!`. A column definition could look like this:

```
- uint "id" !primarykey
- char(50) "name" !unique !notnull
```

# Using QSL

QSL will generate a file called `db_<db-name>.h` for every database. To use it, you first have
to connect to a database. See QSLDB for an example how to do so.

Currently, only CMake as the build system is supported. Support for qmake may follow.

## CMake

To use QSL with CMake, just add the following to your `CMakeLists.txt`:

```
find_package(QSL REQUIRED)
include_directories(${QSL_INCLUDE_DIRS})

# add an executable as usual
add_executable(foo foo.cpp)
# compile the qsl files for this executable
qsl_compile(TARGET foo FILES foo.qsl)
```

The `qsl_compile` function will add a target called `foo_qslc` that builds the qsl
files and then adds it as a dependency to the original target.

## `qslc`

If you don't like to use CMake for your build, you can invoke `qslc` yourself.

```
Usage: qslc [options] <file> [<file> ...]
QSL Compiler

Options:
  -h, --help       Displays this help.
  -v, --version    Displays version information.
  -d, --dir <dir>  The target directory to put the generated files

Arguments:
  file             The input file(s) to compile
```
