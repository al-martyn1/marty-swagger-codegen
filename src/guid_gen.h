#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
 
int main()
{
   boost::uuids::random_generator uuid_gen;
   boost::uuids::uuid u = uuid_gen();
   std::cout << u << std::endl;
}



#include <windows.h>
int main( void )
{
    GUID MyGuid;
    HRESULT hr = CoCreateGuid( &MyGuid );
    return 0;
}


#include <random>
#include <iostream>
 
int main() {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(1, 10);
 
    for (int i=0; i<16; ++i)
        std::cout << dist(mt) << "\n";
}
