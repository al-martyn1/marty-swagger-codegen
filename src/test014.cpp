/*! \file
    \brief Как test013, но выводятся только списки "all used components"/"all found components". Нужно было только чтобы сравнить два списка
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
#include <unordered_map>
#include <unordered_set>

#include "umba/simple_formatter.h"

#include "test_utils.h"

#include "marty_swagger.h"

#include "dependency_finder.h"

#include "prj_utils.h"


 


#define USE_EXACT_TEST


int main( int argc, char* argv[] )
{
    using umba::lout;
    using namespace umba::omanip;

    #ifdef USE_EXACT_TEST
    
        //INIT_TEST_INPUT_FILE_EX("openapi.yaml");
        INIT_TEST_INPUT_FILE_EX("swagger-example-tinkoff-openapi.yaml");
    
    #else
    
        INIT_TEST_INPUT_FILE_ARG();
    
    #endif

    std::string errMsg;
    std::string tmpJson;
    marty::json_utils::FileFormat detectedFormat = marty::json_utils::FileFormat::unknown;
    nlohmann::json apiSpecJson = marty::json_utils::parseJsonOrYaml( in, true /* allowComments */ , &errMsg, &tmpJson, &detectedFormat );

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


    static const
    std::vector< std::regex >
    removePathRegexes = { std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/example^" , true ) ) // use anchors
                        , std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/examples^", true ) ) // use anchors
                        , std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/x-waiters^", true ) ) // use anchors
                        , std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/x-aws-exception^", true ) ) // use anchors
                        , std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/xml/wrapped^", true ) ) // use anchors
                        // /info/x-unofficialSpec
                        };

    marty::json_utils::removePaths( apiSpecJson, removePathRegexes );




    marty::swagger::DependencyFinder<std::string>  dependencyFinder;

    std::set<std::string> foundComponents              ; // All found in 'components' section
    std::set<std::string> foundComponentsInPaths       ; // All found in 'paths' section
    std::set<std::string> foundComponentsAndDeps       ; // all deps and 'components'
    std::set<std::string> foundComponentsInPathsAndDeps; // all deps and 'paths'

    marty::swagger::findComponents( apiSpecJson, dependencyFinder, &foundComponents, &foundComponentsInPaths );
    auto allUsedDeps = dependencyFinder.getAllDependencies(foundComponentsInPaths, dependencyFinder.addSourceNamesToResult);

    marty::swagger::findComponents( apiSpecJson, dependencyFinder, &foundComponents, &foundComponentsInPaths );
    auto allFoundDeps = dependencyFinder.getAllDependencies(foundComponents, true);


    //----------------------------------------------------------------------------

    printSectionHeader("List of all used components", (int)allUsedDeps.size(), bFirst);

    for( auto cmp : allUsedDeps )
    {
        lout << cmp << "\n";
    }


    printSectionHeader("List of all found components", (int)allFoundDeps.size());

    for( auto cmp : allFoundDeps )
    {
        lout << cmp << "\n";
    }


    return 0;
}

// #include "src/gtest_main.cc"


