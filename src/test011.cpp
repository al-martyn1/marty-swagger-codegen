/*! \file
    \brief Поиск путей в JSON, соответствующих маске $ref, и выцеп OpenAPI типов
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
    
        //INIT_TEST_INPUT_FILE_EX("swagger-example-tinkoff-openapi.yaml");
        INIT_TEST_INPUT_FILE_EX("openapi.yaml");
    
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

    lout << "Found refs:\n\n";
    for( auto matchedPath : matchedPaths )
    {
        auto refValFull = marty::json_utils::getScalarStr(apiSpecJson[marty::json_utils::makeJsonPointer(matchedPath)]);
             refValFull = marty::swagger::util::unquoteSimpleQuoted(refValFull); // umba::string_plus::unquoteSimpleQuoted - тоже самое; 
                                                //TODO: !!! надо бы сделать туда ещё разбор CSV строки
        auto refVal     = marty::swagger::util::getRefValue(refValFull);
        auto refPrefix  = marty::swagger::util::getRefPrefix(refValFull);

        lout << "" // "  " // Два пробела - таки два пробела
             // << refVal
             << left << width(94)   << matchedPath << " - " 
             // << left << width(54)   << refValFull << ", "
             << "name: "
             << refVal 
             // << ", "
             // << left << width(24)   << refVal << ", "
             // << left <</*width(70)<<*/ "prefix: " << refPrefix
             << "\n";

        auto componentName = matchedPath;
        if (marty::swagger::util::startsWithAndStrip(componentName,"/components/schemas/"))
        {
            componentName = marty::swagger::util::getFirstPathItem(componentName);
            lout << "    " << componentName << " depends on " << refVal << "\n";
        }

    }

    return 0;
}


