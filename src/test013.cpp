/*! \file
    \brief Строим зависимости для типов из OpenAPI спеки по $ref-ссылкам - поиск всех/использованных компонентов вынесен в отдельную функцию findComponents
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



inline
nlohmann::json_pointer<nlohmann::json> makeJsonPointer(const std::string &path)
{
    if (!path.empty() && path[0]!='/')
        return nlohmann::json_pointer<nlohmann::json>(std::string("/")+path);
    return nlohmann::json_pointer<nlohmann::json>(path);
}

inline
nlohmann::json_pointer<nlohmann::json> makeJsonPointer(const char* path)
{
    if (!path)
        //return nlohmann::json_pointer<json>();
        makeJsonPointer(std::string());

    return makeJsonPointer(std::string(path));
}

 


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

    std::set<std::string> foundComponents; // All found in 'components' section
    std::set<std::string> foundComponentsInPaths;

    marty::swagger::findComponents( apiSpecJson, dependencyFinder, &foundComponents, &foundComponentsInPaths );


    auto printSectionHeader = [&]( const char *title, int szFound = -1, bool bFirst = false )
        {
            if (!bFirst)
                lout << "\n\n#----------------------------------------------------------------------------\n";

            lout << "### "<<title;

            if (szFound>=0)
                lout << " (Total: " << szFound << ")";

            lout << ":\n\n";
        };



    //----------------------------------------------------------------------------

    printSectionHeader("Found components", (int)foundComponents.size(), true);
    for( auto cmp : foundComponents )
    {
        lout << left << width(24) << cmp;
        auto deps = dependencyFinder.getAllDependencies(cmp);
        if (!deps.empty())
        {
            lout << " ";
            std::size_t cnt = 0;
            for( const auto &depName : deps )
            {
                lout << (cnt==0 ? "- " : ", " ) << depName;
                ++cnt;
            }
        }

        lout << "\n";

    }



    //----------------------------------------------------------------------------

    printSectionHeader("Used components", (int)foundComponentsInPaths.size());

    for( auto cmp : foundComponentsInPaths )
    {
        lout << cmp << "\n";
    }



    std::set<std::string> foundComponentsAndDeps;
    std::set<std::string> foundComponentsInPathsAndDeps;

    auto printStringVector = [&]( const std::vector<std::string> &v )
    {
        for( unsigned i=0; i!=v.size(); ++i)
        {
            if (i)
                lout << ", ";
            lout << v[i];
        }
    };

    auto printNameAndDeps = [&]( const std::string &name, const std::vector<std::string> &deps, int nameWidth )
    {
        if (!deps.size())
        {
            lout << name; // << "\n";
        }
        else
        {
            lout << left << width(nameWidth) << name << " - ";
            printStringVector(deps);
        }
    };



    //----------------------------------------------------------------------------

    auto allUsedDeps = dependencyFinder.getAllDependencies(foundComponentsInPaths, true);
    printSectionHeader("All used components in order to declare", (int)allUsedDeps.size());

    for( auto cmp : allUsedDeps )
    {
        foundComponentsInPathsAndDeps.insert(cmp);

        auto deps = dependencyFinder.getAllDependencies(cmp);

        printNameAndDeps(cmp, deps, 32);
        lout << "\n";
    }

    

    //----------------------------------------------------------------------------

    auto allFoundDeps = dependencyFinder.getAllDependencies(foundComponents, true);
    printSectionHeader("All found components in order to declare", (int)allFoundDeps.size());

    for( auto cmp : allFoundDeps )
    {
        foundComponentsAndDeps.insert(cmp);

        auto deps = dependencyFinder.getAllDependencies(cmp);

        printNameAndDeps(cmp, deps, 32);
        lout << "\n";

    }


    // Нужно было только чтобы сравнить два списка
    #if 0
    lout << "\n";
    lout << "\n";
    lout << "#----------------------------------------------------------------------------\n";
    lout << "### List of all used components in order to declare (Total: " << foundComponentsInPathsAndDeps.size() << "):\n";
    lout << "\n";
    for( auto cmp : foundComponentsInPathsAndDeps )
    {
            lout << cmp << "\n";
    }


    lout << "\n";
    lout << "\n";
    lout << "#----------------------------------------------------------------------------\n";
    lout << "### List of all found components in order to declare (Total: " << foundComponentsAndDeps.size() << "):\n";
    lout << "\n";
    for( auto cmp : foundComponentsAndDeps )
    {
            lout << cmp << "\n";
    }
    #endif


    return 0;
}

// #include "src/gtest_main.cc"


