# Tests for the firmware code

This is unittests for the firmware functions. These are not meant to be run on the target, but rather on the host as part of the build process. The tests are located in the `tests` directory and are built using CMake. The tests are run using the `ctest` command after building the project. 

## Running tests

From the `tests` directory:

```sh
mkdir -p build
cd build
cmake ..
cmake --build . -j$(nproc)
```

List discovered tests:

```sh
ctest -N
```

Run all tests:

```sh
ctest --output-on-failure
```

Run a subset of tests by name pattern:

```sh
ctest -R "topic|MeasurementAsDouble" --output-on-failure
```

## Running the GoogleTest binary directly

From `tests/build`:

```sh
./test
```

List tests:

```sh
./test --gtest_list_tests
```

Run a subset using GoogleTest filter:

```sh
./test --gtest_filter="*topic*:*MeasurementAsDouble*"
```
