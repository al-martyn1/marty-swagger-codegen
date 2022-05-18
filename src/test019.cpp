/*! \file
    \brief Разбираюсь с анонимными пространствами имён
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

#include "jinja2cpp/template.h"
#include "jinja2cpp/template_env.h"
#include "jinja2cpp/reflected_value.h"
#include "jinja2cpp/user_callable.h"

#include "jinja2cpp_test_data.h"


enum class E1
{
    unknown = 0
};

enum class E2
{
    unknown = 0
};




namespace tmp{

namespace {

template<typename> inline
std::string getTypeName()
{
    return "GenericType";
}

} // namespace


#define DECLARE_GET_TYPE_NAME_SPECIALIZATION(typeName)  \
                                                        \
            namespace {                                 \
                                                        \
            template<> inline                           \
            std::string getTypeName<typeName>()         \
            {                                           \
                return #typeName;                       \
            }                                           \
                                                        \
            } /* namespace */

DECLARE_GET_TYPE_NAME_SPECIALIZATION(std::string)
DECLARE_GET_TYPE_NAME_SPECIALIZATION(int)
DECLARE_GET_TYPE_NAME_SPECIALIZATION(TestInnerStruct)
DECLARE_GET_TYPE_NAME_SPECIALIZATION(TestStruct)
DECLARE_GET_TYPE_NAME_SPECIALIZATION(E1)

} // namespace tmp



#define GET_TYPE_NAME(typeName) tmp::getTypeName<typeName>()

#define PRINT_TYPE_NAME(typeName) lout << "Name for '" << #typeName << "' is: " << GET_TYPE_NAME(typeName) << "\n"


 
using namespace std;



int main( int argc, char* argv[] )
{
    using umba::lout;
    using namespace umba::omanip;

    PRINT_TYPE_NAME( int );
    PRINT_TYPE_NAME( std::string  );
    PRINT_TYPE_NAME( std::wstring );
    PRINT_TYPE_NAME( std::int64_t );
    PRINT_TYPE_NAME( TestInnerStruct );
    PRINT_TYPE_NAME( E1 );
    PRINT_TYPE_NAME( E2 );



    return 0;
}



