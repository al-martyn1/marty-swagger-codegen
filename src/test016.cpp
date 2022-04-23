/*! \file
    \brief "Hello World" при помощи Jinja2Cpp
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <queue>
#include <deque>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <random>
#include <cstdlib>
#include <exception>
#include <stdexcept>

#include "umba/simple_formatter.h"

#include "test_utils.h"

#include "jinja2cpp/template.h"

// Old variant?
//#include <jinja2/template.h>

 
using namespace std;



int main( int argc, char* argv[] )
{
    using umba::lout;
    using namespace umba::omanip;

    jinja2::Template tpl;

    tpl.Load("{{'Hello World' }}!!!");

    lout << tpl.RenderAsString(jinja2::ValuesMap{}).value() << endl;

    // INIT_TEST_INPUT_FILE_EX("001");
    // INIT_TEST_INPUT_FILE();
    // INIT_TEST_INPUT_CIN();
    // INIT_TEST_INPUT_FILE_ARG();

    return 0;
}

// #include "gtest/../../src/gtest_main.cc"


