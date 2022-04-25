/*! \file
    \brief Типа "Hello World", но шаблоны берём из файла. Тестим Reflect для структурк, MakeCallable с разными типами аргумента
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


#include "json_utils.h"
#include "dependency_finder.h"
#include "marty_swagger.h"


#include "jinja2cpp/template.h"
#include "jinja2cpp/template_env.h"
#include "jinja2cpp/reflected_value.h"
#include "jinja2cpp/user_callable.h"
#include "jinja2cpp/binding/nlohmann_json.h"

#include "jinja2cpp_test_data.h"

 
using namespace std;
// using namespace jinja2;




#define USE_EXACT_TEST

int main( int argc, char* argv[] )
{
    using umba::lout;
    using namespace umba::omanip;

    std::string jsonFile;

    #ifdef USE_EXACT_TEST
    
        //INIT_TEST_INPUT_FILE_EX("test017_004.txt");
        INIT_TEST_INPUT_FILE_EX("test017_007.txt");
        // jsonFile = "F:\\_github\\marty-swagger-codegen\\tests\\description.yaml";
        jsonFile = "F:\\_github\\marty-swagger-codegen\\tests\\swagger-example-tinkoff-openapi.yaml";
    
    #else
    
        INIT_TEST_INPUT_FILE_ARG();
        if (argc>2)
            jsonFile = argv[2];
    
    #endif


    jinja2::TemplateEnv env;

    // Fill env here

    auto &tplEnvSettings = env.GetSettings();

    tplEnvSettings.extensions.Do = true;

    // tplEnvSettings.useLineStatements = true;

    // tplEnvSettings.trimBlocks = true;

    // tplEnvSettings.lstripBlocks = true;

    // tplEnvSettings.cacheSize = 4096;

    // tplEnvSettings.autoReload = false;

    // tplEnvSettings.jinja2CompatMode = jinja2::Jinja2CompatMode::None;
    // tplEnvSettings.jinja2CompatMode = jinja2::Jinja2CompatMode::Vesrsion_2_10;

    //tplEnvSettings.m_defaultMetadataType = "json"; // default val



    jinja2::Template tpl(&env);

    // auto parse_result = tpl.Load(source);
    tpl.Load(in,testInputFileName);
    // LoadFromFile(const std::string& fileName);

    auto tplMetadata                   = tpl.GetMetadata();
    auto tplMetadataValue              = tpl.GetMetadata().value();
    auto tplMetadataRaw                = tpl.GetMetadataRaw();
    auto tplMetadataRawValue           = tpl.GetMetadataRaw().value();
    auto tplMetadataRawValueMetadata   = tpl.GetMetadataRaw().value().metadata;


    #if 0

    ---------------
    test/expressions_test.cpp:105

    std::string source = R"(
{{ data.strValue }}{% do setData('Inner Value') %}
{{ data.strValue }}
)";

    ValuesMap params = {
        {"data", Reflect(&innerStruct)},
        {"setData", MakeCallable(
            [&innerStruct](const std::string& val) -> Value {
                innerStruct.strValue = val;
                return "String not to be shown";
            },
            ArgInfo{"val"})
            },
    };

    ---------------
    test/expressions_test.cpp:142

    const std::string source = R"(>> {{ 2 < '6' | int }} <<
  >> {{ -30 | abs < str | int }} <<)";

    const ValuesMap params = {{"str", "20"}};

    ---------------

    #endif

    TestInnerStruct innerStruct;
    innerStruct.strValue = "Outer Value";

    TestStruct testStruct;
    testStruct.intValue    = 3;
    testStruct.dblValue    = 2.5;
    testStruct.boolValue   = true;
    testStruct.strValue    = "testStruct string value";
    testStruct.wstrValue   = L"testStruct wide string value";
    testStruct.innerStruct = std::make_shared<TestInnerStruct>( TestInnerStruct{"innerStruct field of type TestInnerStruct"} );
    for (int n = 0; n < 7; ++n)
        testStruct.strings.emplace_back((std::string("Str #") + std::to_string(n+1)));
    for (int n = 0; n < 3; ++n)
        testStruct.innerStructList.emplace_back(std::make_shared<TestInnerStruct>(TestInnerStruct{std::string("List item #") + std::to_string(n+1)}));

    bool hasJson                 = false;
    bool isOpenApiSpec           = false;
    std::string specFormat       = "unknown";
    std::vector<std::string> foundTypes;

    jinja2::ValuesMap renderParams = 
    {   { "data"         , jinja2::Reflect(&innerStruct) }
      , { "testStruct"   , jinja2::Reflect(&testStruct)  }
      , { "hasJson"      , jinja2::Reflect(&hasJson)     }
      , { "isOpenApiSpec", jinja2::Reflect(&isOpenApiSpec)  }
      , { "specFormat"   , jinja2::Reflect(&specFormat)  }
      , { "specTypes"    , jinja2::Reflect(&foundTypes)  }
      , { "setData"      , jinja2::MakeCallable( [&innerStruct](const std::string& val) -> jinja2::Value
                                                 {
                                                     innerStruct.strValue = val;
                                                     return "String not to be shown";
                                                 }
                                               , jinja2::ArgInfo{"val"}
                                               )
        }
      , { "setIntValueAsStr", jinja2::MakeCallable( [&testStruct](const std::string& val) -> jinja2::Value
                                                {
                                                    testStruct.intValue = std::stoi(val);
                                                    return "String not to be shown";
                                                }
                                              , jinja2::ArgInfo{"val"}
                                              )
        }
      , { "setIntValue"  , jinja2::MakeCallable( [&testStruct](int val) -> jinja2::Value
                                                 {
                                                     testStruct.intValue = val;
                                                     return "String not to be shown";
                                                 }
                                               , jinja2::ArgInfo{"val"}
                                               )
        }
    };


    if (!jsonFile.empty())
    {
        std::string errMsg;
        std::string tmpJson;
        marty::json_utils::FileFormat detectedFormat = marty::json_utils::FileFormat::unknown;

        nlohmann::json j = marty::json_utils::parseJsonOrYamlFromFile( jsonFile /* std::string(argv[2]) */ , true /* allowComments */ , &errMsg, &tmpJson, &detectedFormat );

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

        
        
        hasJson       = true;
        renderParams["hasJson"] = jinja2::Reflect(&hasJson);

        // isOpenApiSpec = false;
        specFormat    = detectedFormat==marty::json_utils::FileFormat::json ? "json" : "yaml";
        renderParams["specFormat"] = jinja2::Reflect(&specFormat);

        static const
        std::vector< std::regex >
        removePathRegexes = { std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/example^" , true ) ) // use anchors
                            , std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/examples^", true ) ) // use anchors
                            , std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/x-waiters^", true ) ) // use anchors
                            , std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/x-aws-exception^", true ) ) // use anchors
                            , std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/xml/wrapped^", true ) ) // use anchors
                            // /info/x-unofficialSpec
                            };
       
        marty::json_utils::removePaths( j, removePathRegexes );

        marty::swagger::DependencyFinder<std::string>  dependencyFinder;

        std::set<std::string> foundComponents              ; // All found in 'components' section
        std::set<std::string> foundComponentsInPaths       ; // All found in 'paths' section
       
        marty::swagger::findComponents( j, dependencyFinder, &foundComponents, &foundComponentsInPaths );
        auto allFoundDeps = dependencyFinder.getAllDependencies(foundComponents, true);


        //std::vector< std::vector<std::string> > allFoundInOrderToDeclareItemDeps;
        // std::vector<std::string> allFoundInOrderToDeclare
        foundTypes = dependencyFinder.getSortedByDependenciesCount(  allFoundDeps
                                                        // , &allFoundInOrderToDeclareItemDeps
                                                        );
        renderParams["specTypes"] = jinja2::Reflect(&foundTypes);

        // , { "isOpenApiSpec", jinja2::Reflect(&isOpenApiSpec)  }

        auto apiSpecTest = j.get<marty::swagger::OpenApiSpecDetectStubObject>();
        if (apiSpecTest.openapi && !apiSpecTest.openapi->empty())
        {
            isOpenApiSpec = true;
            renderParams["isOpenApiSpec"] = jinja2::Reflect(&isOpenApiSpec);
        }

        renderParams["json"] = jinja2::Reflect(std::move(j));

    }



    try
    {
        lout << tpl.RenderAsString(renderParams).value() << endl;
    }
    catch(const std::exception &e)
    {
        lout << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}



