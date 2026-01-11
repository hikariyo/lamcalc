# LamCalc

A tiny untyped lambda calculus interpreter written in C, designed to explore the basic concepts of lambda calculus.

## Project Goals

The primary purpose of this project is to serve as a hands-on exercise in:

+ Manual Ownership Management: Implementing strict memory lifecycle rules for complex data structures in C, including deep copying, recursive destruction, and preventing memory leaks.

+ Multi-file Project Architecture: Organizing multiple files using `xmake`.

## Core Functionality

+ Church Arithmetic: Supports addition and multiplication. It can automatically convert between Lambda expressions and normal numbers (e.g., transforming a Church term `\f.\x.(f (f x))` into `2`).

+ Built-in Test Framework: I used `__attribute__((constructor))` to make tests run automatically. When you run the test binary, it gathers all test cases and prints logs in Google Test Style.

## Usage

### Run tests

To verify the memory safety (using AddressSanitizer):
```bash
xmake f -m debug
xmake run tests
```

Note for **macOS** users: The default Apple Clang does not support detect_leaks. To enable leak detection, install LLVM via Homebrew and configure the toolchain:

```bash
xmake f --toolchain=llvm --sdk=$(brew --prefix llvm) -m debug
```

Note: Ensure the `--sdk` path matches your local Homebrew installation.

### Run REPL

**Prerequisites**: Ensure the `readline` library is installed on your system.

```bash
xmake f -m release # or debug if you want
xmake run lamcalc
```

**Example interaction**:

```text
> + 2 3
5 (646.00 μs)
> * 3 3
9 (958.00 μs)
> + 2 (* 5 6)
32 (1.47 ms)
> (\x.\y.x) 2 3
2 (489.00 μs)
> Y (\f.\n. iszero n 1 (* n (f (pred n)))) 4
24 (1288.64 ms)
> exit
```
