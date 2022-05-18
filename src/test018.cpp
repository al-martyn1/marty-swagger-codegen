/*! \file
    \brief Пытаюсь разобраться, что не так с плюсовыми массивами и прочим объектами, которые передаются в Jinja2Cpp
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


// #define USE_EXACT_TEST

int main( int argc, char* argv[] )
{
    using umba::lout;
    using namespace umba::omanip;

    // INIT_TEST_INPUT_FILE_EX("001");
    // INIT_TEST_INPUT_FILE();
    // INIT_TEST_INPUT_CIN();
    // INIT_TEST_INPUT_FILE_ARG();

    std::string jsonFile;

    #ifdef USE_EXACT_TEST
    
        //INIT_TEST_INPUT_FILE_EX("test017_004.txt");
        INIT_TEST_INPUT_FILE_EX("test018_004.txt");
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


    #ifdef USE_EXACT_TEST

        //tpl.Load("ints reverse: {{ ints | reverse | pprint }}\nints len: {{ ints | length }}\n");

    #else

        // auto parse_result = tpl.Load(source);
        // LoadFromFile(const std::string& fileName);
        // tpl.Load(in,testInputFileName);

    #endif

    auto parse_result = tpl.Load(in,testInputFileName);

    if (!parse_result)
    {
        lout << "error: " << parse_result.error().ToString() << "\n";
        return 1;
    }
    


    std::vector<int>            ints{1,2,3,4};
    std::vector<std::string>    strs{ "s1","s2","s3","s4"};

    jinja2::ValuesMap renderParams = 
    {   
          { "jsonCallableGenericMap", 
            jinja2::MakeCallable( []( const jinja2::GenericMap& val )
                                  {
                                      return "GenericMap";
                                  }
                                , jinja2::ArgInfo{"val"}
                                )
          }
        , { "jsonCallableGenericList", 
            jinja2::MakeCallable( []( const jinja2::GenericList& val )
                                  {
                                      return "GenericList";
                                  }
                                , jinja2::ArgInfo{"val"}
                                )
          }
        , { "jsonCallableValuesMap", 
            jinja2::MakeCallable( []( const jinja2::ValuesMap& val )
                                  {
                                      return "ValuesMap";
                                  }
                                , jinja2::ArgInfo{"val"}
                                )
          }
        , { "jsonCallableGenericList", 
            jinja2::MakeCallable( []( const jinja2::GenericList& val )
                                  {
                                      return "GenericList";
                                  }
                                , jinja2::ArgInfo{"val"}
                                )
          }
        , { "simpleCallable", 
            jinja2::MakeCallable( []( const std::string& val )
                                  {
                                      return "std::string";
                                  }
                                , jinja2::ArgInfo{"val"}
                                )
          }
        , { "j", 
            jinja2::MakeCallable( []( const nlohmann::json &j )
                                  {
                                      return j;
                                  }
                                , jinja2::ArgInfo{"val"}
                                )
          }

        , { "test",
            jinja2::MakeCallable( [](const std::string& str1, const std::string& str2)
                                  {
                                      return str1 + "_" + str2;
                                  }
                                , jinja2::ArgInfo{"str1"}, jinja2::ArgInfo{"str2", false, "default"}
                                )
          }

        #ifdef USE_EXACT_TEST
        , { "ints"         , jinja2::Reflect(&ints) }
        , { "strs"         , jinja2::Reflect(&strs) }
        #endif
    };



    std::vector<std::string> componentTypes;

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


        componentTypes = dependencyFinder.getSortedByDependenciesCount(  allFoundDeps
                                                        // , &allFoundInOrderToDeclareItemDeps
                                                        );


        // renderParams["components"] = jinja2::Reflect(std::move( j[marty::json_utils::makeJsonPointer("components/schemas")] ));
        renderParams["components"] = jinja2::Reflect(  j[marty::json_utils::makeJsonPointer("components/schemas")] );
        renderParams["json"] = jinja2::Reflect(std::move( j ));



    }

    // renderParams["hasJson"]       = jinja2::Reflect(&hasJson);
    // renderParams["isOpenApiSpec"] = jinja2::Reflect(&isOpenApiSpec);
    // renderParams["specFormat"]    = jinja2::Reflect(&specFormat);
    renderParams["ComponentTypes"] = jinja2::Reflect(&componentTypes);






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

// #include "gtest/../../src/gtest_main.cc"


