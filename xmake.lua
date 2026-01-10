set_project("lamcalc")
set_version("0.1.0")
set_languages("c11")

add_rules("mode.debug", "mode.release")

if is_mode("debug") then
    set_policy("build.sanitizer.address", true)
    set_policy("build.sanitizer.undefined", true)
    set_optimize("none")
    set_warnings("all", "extra")
end

add_includedirs("include")

target("lamcalc")
    set_kind("binary")
    add_files("src/*.c")
    add_syslinks("readline")
    if is_mode("debug") then
        add_runenvs("ASAN_OPTIONS", "detect_leaks=1:detect_stack_use_after_return=1:check_initialization_order=1:strict_string_checks=1")
        add_runenvs("UBSAN_OPTIONS", "print_stacktrace=1")
    end

target("tests")
    set_kind("binary")
    add_files("src/*.c")
    remove_files("src/main.c")
    add_files("tests/*.c")
    if is_mode("debug") then
        add_runenvs("ASAN_OPTIONS", "detect_leaks=1:detect_stack_use_after_return=1:check_initialization_order=1:strict_string_checks=1")
        add_runenvs("UBSAN_OPTIONS", "print_stacktrace=1")
    end
