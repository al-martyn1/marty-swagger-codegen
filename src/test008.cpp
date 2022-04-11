/*! \file
    \brief Remove a paths that matches the taken regexes
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

#include "yaml_utils.h"
#include "json_utils.h"

#include "umba/simple_formatter.h"
#include "umba/filename.h"

#include "test_utils.h"

 


 
using namespace std;


#define USE_EXACT_TEST


int main( int argc, char* argv[] )
{
    using umba::lout;
    using namespace umba::omanip;

    #ifdef USE_EXACT_TEST
    
        //INIT_TEST_INPUT_FILE_EX("001.txt");
        //INIT_TEST_INPUT_FILE_EX("amazonaws_athena_openapi.yaml");
        //INIT_TEST_INPUT_FILE_EX("amazonaws-application-autoscaling-openapi.yaml");
        INIT_TEST_INPUT_FILE_EX("description.yaml");
    
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

    // std::vector< std::basic_regex<char> >
    std::vector< std::regex >
    regexes = { std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/example^" , true ) ) // use anchors
              , std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/examples^", true ) ) // use anchors
              };

    marty::json_utils::removePaths( j, regexes );

    std::cout << j.dump(4) << std::endl;


    return 0;

}

// #include "src/gtest_main.cc"


