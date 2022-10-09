# Getting started

1. Clone the project
2. Open the task in task.pdf
3. Start working with `*.c` files according to the task

## Testing

For unit testing is used [googletest](https://github.com/google/googletest)

1. Install GoogleTest for your OS
    * `brew install googletest` for macOS
    * `sudo apt-get install libgtest-dev` for Ubuntu
    * `choco install googletest` for Windows
2. Run `make test` to run all tests
3. While developing use CMake to build and run tests