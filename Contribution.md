# Contribution

To contribute to SPIS, fork the repository, make changes, and create a pull
request. Please make sure that you use an meaningfull name for your commits
(don't call them something like _stuff_ if they change more than a forgotten `&`).
Also, please respect the following topics:

## Code Style

All code is indented by a tab. Each tab is 4 spaces wide. Each opening brace
(`{`) is put on a new line. Use the initializer of the constructor instead of
assining a value later in the constructor whenever possible
(`public clazz(int i) : _i(i)`). All private fields of a class start with an
underscore (`_`). Don't write `get` before getters.

### Generated code style

The generated code style is the same as the "normal" one, appart from the
fact that code is indented by 2 spaces and no tabs.

## Testing

CMake enables the generation of tests by default but you need to manually
run them using `make test`. If test fail you can get more output using
`ctest --verbose`. Also please make sure that the examples compile and run on an
empty and on an already existing database (please don't commit your username
or password of your test database). Keep in mind that this doesn't cover
all things you can possibly mess up - so try to use your changes to see
if you run into strange behaviour.

