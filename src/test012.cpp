/*! \file
    \brief Строим зависимости для типов из OpenAPI спеки по $ref-ссылкам - тест
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



 


// #define USE_EXACT_TEST


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


    std::vector<std::string> matchedPaths;

    std::string regexStr = umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/$ref^", true );

    marty::json_utils::findPathMatches( apiSpecJson, matchedPaths
                                      , std::regex( regexStr ) // use anchors
                                      );

    #if 0
        /components/schemas/SearchMarketInstrument/properties/type/$ref - '#/components/schemas/InstrumentType'
        /components/schemas/SearchMarketInstrumentResponse/properties/payload/$ref - '#/components/schemas/SearchMarketInstrument'
        /components/schemas/UserAccount/properties/brokerAccountType/$ref - '#/components/schemas/BrokerAccountType'
        /components/schemas/UserAccounts/properties/accounts/items/$ref - '#/components/schemas/UserAccount'
        /components/schemas/UserAccountsResponse/properties/payload/$ref - '#/components/schemas/UserAccounts'
        /paths/~1sandbox~1currencies~1balance/post/requestBody/content/application~1json/schema/$ref - '#/components/schemas/SandboxSetCurrencyBalanceRequest'
        /paths/~1sandbox~1currencies~1balance/post/responses/200/content/application~1json/schema/$ref - '#/components/schemas/Empty'
        /paths/~1sandbox~1currencies~1balance/post/responses/500/content/application~1json/schema/$ref - '#/components/schemas/Error'
        /paths/~1sandbox~1positions~1balance/post/requestBody/content/application~1json/schema/$ref - '#/components/schemas/SandboxSetPositionBalanceRequest'
    #endif

    marty::swagger::DependencyFinder<std::string>  dependencyFinder;


    std:: /* unordered_ */ set<std::string> foundComponents;
    std:: /* unordered_ */ set<std::string> foundComponentsInPaths;

    //lout << "Found refs:\n\n";
    for( auto matchedPath : matchedPaths )
    {
        auto refValFull = marty::json_utils::getScalarStr(apiSpecJson[marty::json_utils::makeJsonPointer(matchedPath)]);
             refValFull = marty::swagger::util::unquoteSimpleQuoted(refValFull); // umba::string_plus::unquoteSimpleQuoted - тоже самое; 

        auto refVal     = marty::swagger::util::getRefValue(refValFull);
        auto refPrefix  = marty::swagger::util::getRefPrefix(refValFull);

        if (refPrefix!="/components/schemas")
             continue;

        auto componentName = matchedPath;
        if (marty::swagger::util::startsWithAndStrip(componentName,"/components/schemas/"))
        {
            componentName = marty::swagger::util::getFirstPathItem(componentName);
            dependencyFinder.addDependency(componentName, refVal);
            foundComponents.insert(componentName);
        }
        else if (marty::swagger::util::startsWithAndStrip(componentName,"/paths/"))
        {
            foundComponentsInPaths.insert(refVal);
        }

    }

    // Тут мы ищем вообще всё из раздела /components/schemas/
    std::vector<std::string> matchedComponentPaths;

    std::string componentsRegexStr = umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/components/schemas/*^", true );

    marty::json_utils::findPathMatches( apiSpecJson, matchedComponentPaths
                                      , std::regex( componentsRegexStr ) // use anchors
                                      );

    // lout << "### Found component paths:\n";
    for( const auto &mcp : matchedComponentPaths )
    {
        // lout << mcp << "\n";
        auto componentName = mcp;
        if (marty::swagger::util::startsWithAndStrip(componentName,"/components/schemas/"))
        {
            componentName = marty::swagger::util::getFirstPathItem(componentName);
            if (!componentName.empty())
                foundComponents.insert(componentName);
        }
    }



    lout << "\n";
    lout << "### Found components (Total: " << foundComponents.size() << "):\n";
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


    lout << "\n";
    lout << "### Used components (Total: " << foundComponentsInPaths.size() << "):\n";

    for( auto cmp : foundComponentsInPaths )
    {
        lout << cmp << "\n";
    }



    std::set<std::string> foundComponentsAndDeps;
    std::set<std::string> foundComponentsInPathsAndDeps;



    auto allUsedDeps = dependencyFinder.getAllDependencies(foundComponentsInPaths, true);
    lout << "\n";
    lout << "### All used components in order to declare (Total: " << allUsedDeps.size() << "):\n";

    //for( auto cmp : dependencyFinder.getAllNames() )
    for( auto cmp : allUsedDeps )
    {
        foundComponentsInPathsAndDeps.insert(cmp);

        auto deps = dependencyFinder.getAllDependencies(cmp);

        if (deps.empty())
        {
            lout << cmp;
        }
        else
        {
            lout << left << width(24) << cmp;

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

    
    auto allFoundDeps = dependencyFinder.getAllDependencies(foundComponents, true);
    lout << "\n";
    lout << "### All found components in order to declare (Total: " << allFoundDeps.size() << "):\n";

    //for( auto cmp : dependencyFinder.getAllNames() )
    for( auto cmp : allFoundDeps )
    {
        foundComponentsAndDeps.insert(cmp);

        auto deps = dependencyFinder.getAllDependencies(cmp);

        if (deps.empty())
        {
            lout << cmp;
        }
        else
        {
            lout << left << width(24) << cmp;

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



    lout << "\n";
    lout << "### List of all used components in order to declare (Total: " << foundComponentsInPathsAndDeps.size() << "):\n";
    for( auto cmp : foundComponentsInPathsAndDeps )
    {
            lout << cmp << "\n";
    }


    lout << "\n";
    lout << "### List of all found components in order to declare (Total: " << foundComponentsAndDeps.size() << "):\n";
    for( auto cmp : foundComponentsAndDeps )
    {
            lout << cmp << "\n";
    }



    return 0;
}


