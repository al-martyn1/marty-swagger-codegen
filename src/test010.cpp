/*! \file
    \brief Начинаем разбор OpenAPI спеки
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


    marty::swagger::OpenApiSpecObject apiSpec;

    try
    {
        apiSpec = apiSpecJson.get<marty::swagger::OpenApiSpecObject>();
        //lout << "Data extracted" << endl;

        {
            nlohmann::json jNew = apiSpec;
           
            // auto jDiff = nlohmann::json::diff( j, jNew );
           
           
            // lout << width(2) << jDiff;
            // lout << "\n--------------------\n";

            //lout << width(2) << tmpJson;
            //lout << "\n--------------------\n";
            //lout << width(2) << jNew;
        }


    }
    catch(const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "JSON:" << std::endl;
        std::cerr << tmpJson << std::endl;
    }


    // apiSpec.paths      - Типы вида XXXRequest/Response

    // apiSpec.components.schemas - SchemaObject

    // apiSpec.components.schemas[N].items          - PropertyItemObject
    //                              . properties    - PropertyObject


    // Собираем зависимости
    
    auto componentsSchemas = apiSpecJson[marty::json_utils::makeJsonPointer("components/schemas")];

    // lout << "--- componentsSchemas:\n";
    // lout << width(2) << componentsSchemas;
    // lout << "\n--------------------\n";


    if (!marty::json_utils::isObjectNode(componentsSchemas))
    {
        lout << "componentsSchemas is not an object, but is a '" << marty::json_utils::nodeTypeName(componentsSchemas) << "'\n";
        return 1;
    }


    for (nlohmann::json::iterator it=componentsSchemas.begin(); it!=componentsSchemas.end(); ++it)
    {
        auto childPath = "components/schemas/" + it.key();
        lout << "\n--- " << childPath << "\n";
        lout << width(2) << it.value();
        lout << "\n--------------------\n";

        // Если properties.SomeProp.type: array, то properties.SomeProp.items - тип элемента
        // В итоге бы хорошо обойтись без промежуточного объекта
    }



    return 0;
}


