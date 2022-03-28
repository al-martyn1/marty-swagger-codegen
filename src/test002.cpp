/*! \file
    \brief [ ]
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
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

#include "marty_swagger.h"

 
using namespace std;



// #define USE_EXACT_TEST


int main( int argc, char* argv[] )
{
    using umba::lout;
    using namespace umba::omanip;

    #ifdef USE_EXACT_TEST
    
        INIT_TEST_INPUT_FILE_EX("swagger-example-tinkoff-openapi.yaml");
    
    #else
    
        INIT_TEST_INPUT_FILE_ARG();
    
    #endif

    std::string errMsg;
    std::string tmpJson;
    nlohmann::json j = marty::json_utils::parseJsonOrYaml( in, true /* allowComments */ , &errMsg, &tmpJson );

    if (j.is_null() && !errMsg.empty())
    {
        std::cerr << testInputFileName << ": error: " << errMsg << std::endl;
        if (!tmpJson.empty())
        {
            //std::cerr << "JSON:" << std::endl;
            //std::cerr << tmpJson << std::endl;
        }
        
        return 1;
    }

    //lout << width(2) << j;

    if (!tmpJson.empty())
    {
        //std::cerr << "JSON:" << std::endl;
        //std::cerr << tmpJson << std::endl;
    }


    try
    {
        auto apiSpec = j.get<marty::swagger::OpenApiSpecObject>();
        lout << "Data extracted" << endl;
    }
    catch(const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "JSON:" << std::endl;
        //std::cerr << std::setw(2) << tmpJson << std::endl;
        std::cerr << tmpJson << std::endl;
    }


    return 0;
}

// #include "src/gtest_main.cc"


