#pragma once

#include <optional>
#include <string>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <typeinfo>
#include <variant>
#include <string>
#include <map>
#include <set>
#include <algorithm>


#if defined(_MSC_VER)

    #if !defined(_MSVC_LANG)
        #define _MSVC_LANG __cplusplus
    #endif

    #if _MSVC_LANG >= 201103L
        #define MARTY_SWAGGER_HAS_CXX_STD11
    #endif

#elif __cplusplus >= 201103L

    #define MARTY_SWAGGER_HAS_CXX_STD11

#endif



#if defined(MARTY_SWAGGER_HAS_CXX_STD11)
    #include <unordered_map>
    #include <unordered_set>
#endif


// marty::swagger::

namespace marty{
namespace swagger{




template< typename StringType
        #ifdef MARTY_SWAGGER_HAS_CXX_STD11
        , typename SetType = std::unordered_set<StringType>
        , typename MapType = std::unordered_map<StringType,SetType>
        #else
        , typename SetType = std::set<StringType>
        , typename MapType = std::map<StringType,SetType>
        #endif
        >
class DependencyFinder
{
    MapType m_depMap;


    static
    void addDependencyHelper( std::vector<StringType> &resVec, SetType &usedNames, const StringType &nameToAdd )
    {
        if (usedNames.find(nameToAdd)==usedNames.end()) // ещё не использовалось
        {
            usedNames.insert(nameToAdd);
            resVec.emplace_back(nameToAdd);
        }
    }

    static
    void addDependencyHelper( std::vector<StringType> &resVec, SetType &usedNames, const SetType &namesToAdd )
    {
        for( typename SetType::const_iterator it=namesToAdd.begin(); it!=namesToAdd.end(); ++it )
        {
            addDependencyHelper(resVec, usedNames, *it);
        }
    }


    std::vector<StringType> getAllDependenciesImpl(StringType name, SetType &usedNames) const
    {

        std::vector<StringType> res;
        {
            typename MapType::const_iterator it = m_depMap.find(name);
            if (it!=m_depMap.end())
                addDependencyHelper( res, usedNames, it->second );
        }

        typename std::vector<StringType>::size_type pos = 0;

        for(; pos!=res.size(); ++pos)
        {
            const StringType &name = res[pos];
            typename MapType::const_iterator it = m_depMap.find(name);

            if (it==m_depMap.end())
                continue;

            addDependencyHelper( res, usedNames, it->second );
        }

        return std::vector<StringType>( res.rbegin(), res.rend() );
    }


public:

    //------------------------------
    void addDependency( const StringType &name
                      , const StringType &dependency // name depend on dependency
                      )
    {
        if (name==dependency)
            return;

        m_depMap[name].insert(dependency);
    };

    //------------------------------
    std::vector<StringType> getAllDependencies(const StringType &name) const
    {
        SetType usedNames;

        usedNames.insert(name);

        return getAllDependenciesImpl(name, usedNames);
    }

    //------------------------------
    template<typename IteratorType, typename DereferencerType>
    std::vector<StringType> getAllDependencies( IteratorType namesBegin, IteratorType namesEnd
                                              , DereferencerType dereferencer
                                              , bool addNames = false
                                              ) const
    {
        SetType usedNames;
        std::vector<StringType> res;

        for(IteratorType it=namesBegin; it!=namesEnd; ++it)
        {
            auto name = dereferencer(it);
            if (!addNames)
                usedNames.insert(name);
            std::vector<StringType> tmp = getAllDependenciesImpl(name, usedNames);
            res.insert(res.end(), tmp.begin(), tmp.end());
        }

        if (addNames)
        {
            for(IteratorType it=namesBegin; it!=namesEnd; ++it)
            {
                addDependencyHelper( res, usedNames, dereferencer(it) );
            }
        }

        return res;
    }

    //------------------------------
    std::vector<StringType> getAllDependencies( const std::set<StringType> &names, bool addNames = false) const
    {
        return
        getAllDependencies( names.begin(), names.end()
                          , []( typename std::set<StringType>::const_iterator it )
                              { return *it; }
                          , addNames
                          );
    }

    #if defined(MARTY_SWAGGER_HAS_CXX_STD11)
    std::vector<StringType> getAllDependencies( const std::unordered_set<StringType> &names, bool addNames = false) const
    {
        return
        getAllDependencies( names.begin(), names.end()
                          , []( typename std::unordered_set<StringType>::const_iterator it )
                              { return *it; }
                          , addNames
                          );
    }
    #endif

    std::vector<StringType> getAllNames() const
    {
        return
        getAllDependencies( m_depMap.begin(), m_depMap.end()
                          , []( typename MapType::const_iterator it )
                              { return it->second; }
                          , true // addNames
                          );
    }


    // Упорядочиваем имена (уже в порядке по зависимостям относительно друг друга) по возрастанию количества зависимостей
    std::vector<StringType> getSortedByDependenciesCountAscending( const std::vector<StringType> &names
                                                                 , std::vector< std::vector<StringType> > *pDeps
                                                                 ) const
    {
        struct NameWithDependencies
        {
            StringType                name;
            std::vector<StringType>   deps;
        };

        std::vector< NameWithDependencies > namesWithDeps;
        namesWithDeps.reserve(names.size());

        // Собрали все зависимости для входных имён
        for( const auto &name : names )
        {
            // namesWithDeps.emplace_back( { name, getAllDependencies(name) } );
            namesWithDeps.emplace_back( name, getAllDependencies(name) );
        }

        std::stable_sort( namesWithDeps.begin(), namesWithDeps.end()
                        , []( const NameWithDependencies &nd1, const NameWithDependencies &nd2 )
                            {
                                if (nd1.deps.size()<nd2.deps.size())
                                    return true;
                                if (nd1.deps.size()>nd2.deps.size())
                                    return false;
                                return nd1.name<nd2.name;
                            }
                        );

        
        if (pDeps)
            pDeps->reserve(namesWithDeps.size());

        std::vector<StringType> resNames;
            resNames.reserve(namesWithDeps.size());

        for( const auto &nd : namesWithDeps )
        {
            if (pDeps)
                pDeps->emplace_back(nd.deps);

            resNames.emplace_back(nd.name);
        }
        
        return resNames;

    }



}; // class Dependencies


//----------------------------------------------------------------------------

} // namespace swagger
} // namespace marty

// marty::swagger::

