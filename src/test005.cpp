/*! \file
    \brief nlohmann::json - printJsonPaths - visit test
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

//template< typename StreamType, typename Visitor >
template< typename StreamType >
void printJsonPaths( StreamType &ss, nlohmann::json &jNode, std::string path = "", unsigned lvl = 0 )
{
    const int indent = 2;
    auto nodeType = marty::json_utils::nodeType(jNode);

    if (marty::json_utils::isScalarNode(nodeType))
    {
        ss << marty::json_utils::makeIndentStr(lvl*indent) << "'" << path << "'" << " - " << marty::json_utils::nodeTypeName(nodeType) << "\n";
    }
    else if (marty::json_utils::isArrayNode(nodeType))
    {
        ss << marty::json_utils::makeIndentStr(lvl*indent) << "'" << path << "'" << " - " << marty::json_utils::nodeTypeName(nodeType) << "\n";

        unsigned idx = 0;
        for (nlohmann::json::iterator it=jNode.begin(); it!=jNode.end(); ++it, ++idx)
        {
            auto childPath = path + "/" + std::to_string(idx);
            printJsonPaths( ss, *it, childPath, lvl+1 );
        }
    }
    else if (marty::json_utils::isObjectNode(nodeType))
    {
        ss << marty::json_utils::makeIndentStr(lvl*indent) << "'" << path << "'" << " - " << marty::json_utils::nodeTypeName(nodeType) << "\n";

        for (nlohmann::json::iterator it=jNode.begin(); it!=jNode.end(); ++it)
        {
            auto childPath = path + "/" + it.key();
            printJsonPaths( ss, it.value(), childPath, lvl+1 );
        }
    }
}



#define USE_EXACT_TEST

int main( int argc, char* argv[] )
{
    using umba::lout;
    using namespace umba::omanip;

    #ifdef USE_EXACT_TEST
    
        INIT_TEST_INPUT_FILE_EX("001.txt");
    
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

    printJsonPaths( std::cout, j );

    return 0;
}



