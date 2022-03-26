/*! \file
    \brief Проверка универсального чтения JSON/YAML в nlohmann::json
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

#include "json_utils.h"

 
using namespace std;


//#define USE_EXACT_TEST

int main( int argc, char* argv[] )
{
    using umba::lout;
    using namespace umba::omanip;


    #ifdef USE_EXACT_TEST
    
        INIT_TEST_INPUT_FILE_EX("006");
    
    #else
    
        INIT_TEST_INPUT_FILE_ARG();
    
    #endif

    std::string errMsg;
    std::string tmpJson;
    //nlohmann::json j = marty::json_utils::parseJsonOrYaml( "null" );
    nlohmann::json j = marty::json_utils::parseJsonOrYaml( in, true /* allowComments */ , &errMsg, &tmpJson );

    if (j.is_null() && !errMsg.empty())
    {
        std::cerr << testInputFileName << ": error: " << errMsg << std::endl;
        if (!tmpJson.empty())
        {
            std::cerr << "JSON:" << std::endl;
            std::cerr << tmpJson << std::endl;
        }
        
        return 1;
    }

    lout << width(2) << j;

    if (!tmpJson.empty())
    {
        std::cerr << "JSON:" << std::endl;
        std::cerr << tmpJson << std::endl;
    }

    return 0;
}



