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
Welcome to the REPL of lamcalc, a simple lambda calculus interpreter.
Input "exit" without quotes to exit (p: parse, e: eval, r: repr).
> + 2 3
5
[ Done in 2.516ms | steps: 14 | p: 0.296, e: 1.356, r: 0.864 ]
> * 3 3
9
[ Done in 1.884ms | steps: 14 | p: 0.136, e: 0.566, r: 1.182 ]
> + 2 (* 5 6)
32
[ Done in 3.525ms | steps: 14 | p: 0.254, e: 0.861, r: 2.410 ]
> (\x.\y.x) 2 3
2
[ Done in 0.536ms | steps: 14 | p: 0.185, e: 0.312, r: 0.039 ]
> Y (\f.\n. iszero n 1 (* n (f (pred n)))) 4
24
[ Done in 1413.993ms | steps: 24 | p: 0.202, e: 2.272, r: 1411.519 ]
> exit
```
