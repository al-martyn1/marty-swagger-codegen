#pragma once

// Relative to project files location
// In MSVC debug mode by default CWD is the project folder. We go up to one level and then down into test_data
// Do not use '\\', use only '/', which is compatible between Windows and Linux
// Trailing '/' can add problems, do not use it

// #define TESTS_INPUT_ROOT "../test_data"
#define TESTS_INPUT_ROOT "../tests"

