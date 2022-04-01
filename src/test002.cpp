/*! \file
    \brief Проверка десериализации спеки в формате OpenAPI в C++ структуры и обратной сериализации
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
    marty::json_utils::FileFormat detectedFormat = marty::json_utils::FileFormat::unknown;
    nlohmann::json j = marty::json_utils::parseJsonOrYaml( in, true /* allowComments */ , &errMsg, &tmpJson, &detectedFormat );

    if (detectedFormat==marty::json_utils::FileFormat::unknown)
    {
        std::cerr << testInputFileName << ": error: " << errMsg << std::endl;
        if (!tmpJson.empty())
        {
            std::cerr << "JSON:" << std::endl;
            std::cerr << tmpJson << std::endl;
        }
        
        return 1;
    }


    try
    {
        auto apiSpec = j.get<marty::swagger::OpenApiSpecObject>();
        //lout << "Data extracted" << endl;
        nlohmann::json jSpec = apiSpec;
        lout << width(4) << jSpec;

    }
    catch(const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "JSON:" << std::endl;
        std::cerr << tmpJson << std::endl;
    }



    return 0;
}

// #include "src/gtest_main.cc"


