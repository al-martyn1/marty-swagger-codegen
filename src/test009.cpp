/*! \file
    \brief Версия с удалением ненужных элементов. Сканируем каталог OpenAPI описаний APIs-guru и процессим - делаем Diff между входной OpenAPI спекой, и тем, что (де)сериализовалось в C++ структуры

    https://github.com/APIs-guru/openapi-directory/tree/main/APIs
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
#include <filesystem>

#include "umba/simple_formatter.h"
#include "umba/filename.h"

#include "test_utils.h"

#include "marty_swagger.h"


 
using namespace std;

inline
std::string makeLogFileName( std::string logPath )
{
    for( auto &ch : logPath )
    {
        switch(ch)
        {
            case ':':
            case '/':
            case '\\':
            case '.':
                ch = '_';
        }
    }

    return logPath;
}


// #define USE_EXACT_TEST

// F:\_github\marty-swagger-codegen\tests\\..\..\__3dp\APIs_guru_openapi_directory\APIs  F:\_github\marty-swagger-codegen\tests\\APIs_log

int main( int argc, char* argv[] )
{
    using umba::lout;
    using namespace umba::omanip;

    std::string inputRootPath;
    std::string outputLogsPath;

    static std::string dotYaml = ".yaml";

    #if !defined(USE_EXACT_TEST)

        if (argc<3)
        {
            std::cerr << "Usage: test004 OpenApiDirecotoryRootPath LogsPath" << "\n";
            return 1;
        }
       
        inputRootPath  = argv[1];
        outputLogsPath = argv[2];

    #else

        inputRootPath  = "F:\\_github\\marty-swagger-codegen\\tests\\\\..\\..\\__3dp\\APIs_guru_openapi_directory\\APIs";
        outputLogsPath = "F:\\_github\\marty-swagger-codegen\\tests\\\\APIs_log";

    #endif


    namespace fs = std::filesystem;

    fs::path logsPath = outputLogsPath;


    std::queue<std::string> scanPaths;
    scanPaths.emplace( inputRootPath );

    std::vector<std::string> foundFiles;


    //------------------------------
    while(!scanPaths.empty())
    {
        std::string curPath = scanPaths.front();
        scanPaths.pop();


        fs::directory_iterator scanPathDirectoryIterator;
        try
        {
            scanPathDirectoryIterator = fs::directory_iterator(curPath);
        }
        catch(...)
        {
            std::cerr << "Error: invalid scan path: " << curPath << "\n";
            continue;
        }

        for (const auto & entry : scanPathDirectoryIterator)
        {
            // https://en.cppreference.com/w/cpp/filesystem/directory_entry

            if (!entry.exists())
                continue;

            //------------------------------
            std::string entryName = entry.path().string();

            if (entry.is_directory())
            {
                //scanPaths.push_back(entryName);
                // std::cout << entry.path() << "\n";

                // std::cout << "D: " << entryName << "\n";
                scanPaths.emplace( entryName );
                continue;
            }

            //------------------------------
            if (!entry.is_regular_file())
            {
                continue; // Какая-то шляпа попалась
            }

            // std::cout << "F: " << entryName << "\n";

            if (entryName.size()<dotYaml.size())
                continue;

            std::string ext = entryName.substr(entryName.size()-dotYaml.size());

            //std::cout << "E: " << ext << "\n";

            if (ext!=dotYaml)
                continue;

            foundFiles.emplace_back(entryName);
        }


    } // while(!scanPaths.empty())



    unsigned processedCounter = 0;

    //------------------------------
    for( const auto &foundFile : foundFiles )
    {
        if (foundFile.size()<inputRootPath.size())
            continue;


        std::string prefix = foundFile.substr(0, inputRootPath.size());

        std::string::size_type logNameStartPos = inputRootPath.size();
        while(logNameStartPos!=foundFile.size() && (foundFile[logNameStartPos]=='/' || foundFile[logNameStartPos]=='\\'))
            ++logNameStartPos;

        if (logNameStartPos==foundFile.size())
        {
            continue;
        }

        std::string logName     = foundFile.substr(logNameStartPos);
        std::string logFileName = makeLogFileName(logName) + ".log";

        std::string logFullFileName = (logsPath / fs::path(logFileName)).string();
        

        #if 0
        std::cout << "F: " << foundFile << "\n";
        std::cout << "P: " << prefix << "\n";
        std::cout << "L: " << logName << "\n";
        std::cout << "N: " << logFileName << "\n";
        std::cout << "U: " << logFullFileName << "\n";
        std::cout << "--- " << "\n";
        #endif


        // Пытаемся загрузить в тестовый стаб, и проверяем наличие поля 'openapi'
        // Само собой, вход должен быть корректным YAML/JSON
        try
        {
            std::ifstream in(foundFile);
            if (!in)
                continue;

            std::string errMsg;
            std::string tmpJson;
            marty::json_utils::FileFormat detectedFormat = marty::json_utils::FileFormat::unknown;
            nlohmann::json j = marty::json_utils::parseJsonOrYaml( in, true /* allowComments */ , &errMsg, &tmpJson, &detectedFormat );
           
            if (detectedFormat==marty::json_utils::FileFormat::unknown)
                continue;

            auto apiSpecTest = j.get<marty::swagger::OpenApiSpecDetectStubObject>();
            if (!apiSpecTest.openapi || apiSpecTest.openapi->empty())
                continue;


        }
        catch(...)
        {
            continue;
        }

        std::ifstream in(foundFile);
        if (!in)
            continue;

        std::ofstream out(logFullFileName, std::ios::out | std::ios::trunc);


        std::string errMsg;
        std::string tmpJson;
        marty::json_utils::FileFormat detectedFormat = marty::json_utils::FileFormat::unknown;
        nlohmann::json j = marty::json_utils::parseJsonOrYaml( in, true /* allowComments */ , &errMsg, &tmpJson, &detectedFormat );
        
        if (detectedFormat==marty::json_utils::FileFormat::unknown)
            continue;

        auto canonicalInputName = umba::filename::makeCanonical(foundFile);

        static const
        std::vector< std::regex >
        removePathRegexes = { std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/example^" , true ) ) // use anchors
                            , std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/examples^", true ) ) // use anchors
                            , std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/x-waiters^", true ) ) // use anchors
                            , std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/x-aws-exception^", true ) ) // use anchors
                            , std::regex(umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/xml/wrapped^", true ) ) // use anchors
                            // /info/x-unofficialSpec
                            };

        try
        {
            marty::json_utils::removePaths( j, removePathRegexes );

            auto apiSpec = j.get<marty::swagger::OpenApiSpecObject>();

            nlohmann::json jNew = apiSpec;
            auto jDiff = nlohmann::json::diff( j, jNew );
            out << "// File: " << canonicalInputName << "\n";
            out << jDiff.dump(2);

            // processedCounter++;
            // if (processedCounter>10)
            //     break;


        }
        catch(const std::exception &e)
        {
            out << "// File: " << canonicalInputName << "\n";
            out << "Error: " << e.what() << std::endl;
            out << "JSON:" << std::endl;
            out << tmpJson << std::endl;
        }
        catch(...)
        {
            continue;
        }








        // std::cout << "F: " << foundFile << "\n";
        // std::cout << "U: " << logFullFileName << "\n";


    
    } // for( const auto &foundFile : foundFiles )


    //scanFolders(appConfig, foundFiles, excludedFiles, foundExtentions);



    // INIT_TEST_INPUT_FILE_EX("001");
    // INIT_TEST_INPUT_FILE();
    // INIT_TEST_INPUT_CIN();
    // INIT_TEST_INPUT_FILE_ARG();

    return 0;
}



