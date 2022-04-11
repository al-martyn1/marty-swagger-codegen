/*! \file
    \brief Print a regex matches on the JSON paths
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

/*  Notes for iterator

    https://json.nlohmann.me/api/basic_json/erase/#exceptions

    - Invalidates iterators and references at or after the point of the erase, including the end() iterator.
    - References and iterators to the erased elements are invalidated. Other references and iterators are not affected.

 */

#include "umba/regex_helpers.h"


//template< typename StreamType, typename Visitor >
template< typename StreamType >
void printJsonPathMatch( StreamType &ss, nlohmann::json &jNode
                       , const std::string &regexStr, const std::basic_regex<char> &r
                       , std::regex_constants::match_flag_type flags = std::regex_constants::match_default
                       , std::string path = "", unsigned lvl = 0
                       )
{
    const int indent = 2;
    auto nodeType = marty::json_utils::nodeType(jNode);

    if (marty::json_utils::isScalarNode(nodeType))
    {
        // ss << marty::json_utils::makeIndentStr(lvl*indent) << "'" << path << "'" << " - " << marty::json_utils::nodeTypeName(nodeType) << "\n";
    }
    else if (marty::json_utils::isArrayNode(nodeType))
    {
        // ss << marty::json_utils::makeIndentStr(lvl*indent) << "'" << path << "'" << " - " << marty::json_utils::nodeTypeName(nodeType) << "\n";

        unsigned idx = 0;
        for (nlohmann::json::iterator it=jNode.begin(); it!=jNode.end(); ++it, ++idx)
        {
            auto childPath = path + "/" + std::to_string(idx);
            if (umba::regex_helpers::regexMatch(childPath, r, flags))
            {
                ss << "'" << childPath << "'" << " - " << "match to '" << regexStr << "'\n";
            }
            else
            {
                //ss << "'" << childPath << "'" << " - " << "NOT match\n"; //" to '" << regexStr << "'\n";
                printJsonPathMatch( ss, *it, regexStr, r, flags, childPath, lvl+1 );
            }
        }
    }
    else if (marty::json_utils::isObjectNode(nodeType))
    {
        //ss << marty::json_utils::makeIndentStr(lvl*indent) << "'" << path << "'" << " - " << marty::json_utils::nodeTypeName(nodeType) << "\n";

        for (nlohmann::json::iterator it=jNode.begin(); it!=jNode.end(); ++it)
        {
            auto childPath = path + "/" + it.key();

            if (umba::regex_helpers::regexMatch(childPath, r, flags))
            {
                ss << "'" << childPath << "'" << " - " << "match to '" << regexStr << "'\n";
            }
            else
            {
                //ss << "'" << childPath << "'" << " - " << "NOT match\n";// " to '" << regexStr << "'\n";
                printJsonPathMatch( ss, it.value(), regexStr, r, flags, childPath, lvl+1 );
            }
        }
    }
}




 
using namespace std;


// #define USE_EXACT_TEST


int main( int argc, char* argv[] )
{
    using umba::lout;
    using namespace umba::omanip;

    #ifdef USE_EXACT_TEST
    
        //INIT_TEST_INPUT_FILE_EX("001.txt");
        INIT_TEST_INPUT_FILE_EX("amazonaws_athena_openapi.yaml");
    
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

    std::string exampleSimpleRegexStr = "^/*/example*^";
    std::string exampleRegexStr       = umba::regex_helpers::expandSimpleMaskToEcmaRegex(exampleSimpleRegexStr, true ); // use anchors

    printJsonPathMatch( std::cout, j, exampleSimpleRegexStr
                      , std::basic_regex<char>(exampleRegexStr)
                      //, std::regex_constants::match_flag_type flags = std::regex_constants::match_default
                      // , std::string path = "", unsigned lvl = 0
                      );

    return 0;

}

// #include "src/gtest_main.cc"


