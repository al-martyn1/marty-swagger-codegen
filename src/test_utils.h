#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "tests_config.h"

#include "gtest/gtest.h"


#ifdef _MSC_VER
    #pragma comment(lib, "_gtest.lib")
    //#pragma comment(lib, "_gmock.lib")
#endif

//----------------------------------------------------------------------------
// cout << "From: [" << getCurTestName(argv[0]) << "]" << endl;
// cout << "From: [" << getCurTestName("c:\\tests\\") << "]" << endl;

inline
std::string getCurTestName(const char *argv0)
{
    if (!argv0)
    {
        std::cerr << "Fatal error: argv[0] is invalid" << std::endl;
        std::exit(1);
    }

    std::string strArgv0 = argv0;
    std::string fileName;
    
    auto pathSepPos = strArgv0.find_last_of("/\\");

    if (pathSepPos==strArgv0.npos)
        fileName = strArgv0;
    else
        fileName = strArgv0.substr(pathSepPos+1);

    auto extPos = fileName.find('.');

    return fileName.substr(0, extPos);
}

//----------------------------------------------------------------------------
inline
std::string getCurTestInputName(const char *argv0, const char *inputName = 0)
{
    std::string testName = getCurTestName(argv0);

    std::string inputNameStr;

    if (inputName)
        inputNameStr = inputName;

    if (!inputNameStr.empty())
    {
        testName += "_" + inputNameStr;
    }

    if (inputNameStr.find_first_of('.')==inputNameStr.npos)
        testName.append(".txt");

    return std::string(TESTS_INPUT_ROOT) + "/" + testName;
}






// Вызывается из main - должен иметь доступ к argc/argv
#define INIT_TEST_INPUT_FILE_EX(testName)                                                 \
                                                                                          \
            if (argc==0 || argv[0]==0)                                                    \
            {                                                                             \
                std::cerr << "Fatal error: argc or argv[0] is invalid" << std::endl;      \
                std::exit(1);                                                             \
            }                                                                             \
                                                                                          \
            std::string   testInputFileName;                                              \
            std::ifstream fileIn;                                                         \
            {                                                                             \
               auto inputName = getCurTestInputName(argv[0], testName);                   \
               testInputFileName = inputName;                                             \
               fileIn.open(inputName, std::ios_base::in);                                 \
               if (!fileIn)                                                               \
               {                                                                          \
                   std::cerr << "Failed to open input file: " << inputName << std::endl;  \
                   std::exit(1);                                                          \
               }                                                                          \
            }                                                                             \
                                                                                          \
            std::istream &in = fileIn


#define INIT_TEST_INPUT_FILE()                                                            \
            INIT_TEST_INPUT_FILE_EX("")

#define INIT_TEST_INPUT_CIN()                                                             \
            std::string   testInputFileName = "STDIN";                                    \
            std::istream &in = std::cin
            

// While running from the command line CWD must be in the subfolder of the root
#define INIT_TEST_INPUT_FILE_ARG()                                                        \
                                                                                          \
            if (argc<2)                                                                   \
            {                                                                             \
               std::cerr << "No input file name taken" << std::endl;                      \
               std::exit(1);                                                              \
            }                                                                             \
                                                                                          \
            INIT_TEST_INPUT_FILE_EX(argv[1])




