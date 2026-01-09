set_project("lamcalc")
set_version("0.1.0")
set_languages("c11")

add_rules("mode.debug", "mode.release")

if is_mode("debug") then
    add_policy("build.sanitizer.address", true)
    add_policy("build.sanitizer.undefined", true)
end

add_includedirs("include")
add_cxflags("-Wall", "-Wextra", "-g")

target("lamcalc")
    set_kind("binary")
    add_files("src/*.c")

target("test")
    set_kind("binary")
    add_files("src/*.c")
    remove_files("src/main.c")
    add_files("tests/*.c")
