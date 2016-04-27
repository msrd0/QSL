# Contribution

To contribute to QSL, fork the repository, make changes, and create a pull
request. Please make sure that you use an meaningfull name for your commits
(don't call them something like _stuff_ if they change more than a forgotten `&`).
Also, please respect the following topics:

## Code Style

All code is indented by a tab. Each tab is 4 spaces wide. Each opening brace
(`{`) is put on a new line. Use the initializer of the constructor instead of
assining a value later in the constructor whenever possible
(`public clazz(int i) : _i(i)`). All private fields of a class start with an
underscore (`_`). Don't write get before getters.

### Generated code style

The generated code style is the same as the "normal" one, appart from the
fact that code is indented by 2 spaces and no tabs.

## Testing

Currently, there are no tests defined in CMake, so running `make test` has
no value. But please make sure that the examples compile and run on an
empty and on an already existing database (please don't commit your username
or password of your test database). Feel free to add tests (see the 
[todo list](./TODO.md)).
