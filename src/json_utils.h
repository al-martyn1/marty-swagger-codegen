#pragma once


#include "yaml_utils.h"

#include <string>
#include <iostream>
#include <sstream>
#include <iterator>

#include "nlohmann/json.hpp"
#include "simple_formatter_json_inserter.h"


// marty::json_utils::

namespace marty{
namespace json_utils{


// 
nlohmann::json parseJsonOrYaml( const std::string &data, bool allowComments = true, std::string *pErrMsg = 0, std::string *pTmpJson = 0 )
{
    nlohmann::json jRes;

    try
    {
        jRes = nlohmann::json::parse( data
                                    , nullptr        // parser_callback_t
                                    , true           // allow_exceptions
                                    , allowComments  // ignore_comments
                                    );
    }
    catch(const std::exception &e)
    {
        if (pErrMsg)
           *pErrMsg = e.what();
    }

    if (!jRes.is_null())
        return jRes;

    try
    {
        YAML::Node yamlNode = YAML::Load(data);

        marty::yaml2json::FastSimpleStringStream fssm;
        marty::yaml2json::writeJson(fssm, yamlNode, -1);
        //marty::yaml2json::writeJson(fssm, yamlNode, 2);

        if (pTmpJson)
           *pTmpJson = fssm.str();
        
        jRes = nlohmann::json::parse( fssm.str()
                                    , nullptr        // parser_callback_t
                                    , true           // allow_exceptions
                                    , allowComments  // ignore_comments
                                    );

    }
    catch (const YAML::Exception& e)
    {
        if (pErrMsg)
           *pErrMsg = e.what();
    }
    catch (const std::exception& e)
    {
        if (pErrMsg)
           *pErrMsg = e.what();
    }
    catch (...)
    {
        if (pErrMsg)
           *pErrMsg = "unknown error";
    }

    return jRes;
    
}

// file_input_adapter
// input_stream_adapter
// iterator_input_adapter


nlohmann::json parseJsonOrYaml( std::istream &in, bool allowComments = true, std::string *pErrMsg = 0, std::string *pTmpJson = 0 )
{
    std::string data;
    char buffer[4096];
    while (in.read(buffer, sizeof(buffer)))
        data.append(buffer, sizeof(buffer));
    data.append(buffer, in.gcount());
    
    return parseJsonOrYaml( data, allowComments, pErrMsg, pTmpJson );

#if 0
    nlohmann::json jRes;

    try
    {
        // https://habr.com/ru/post/122283/
        // https://en.cppreference.com/w/cpp/iterator/istream_iterator
        jRes = nlohmann::json::parse( std::istream_iterator<char>(in)
                                    , std::istream_iterator<char>()
                                    , nullptr        // parser_callback_t
                                    , true           // allow_exceptions
                                    , allowComments  // ignore_comments
                                    );
    }
    catch(const std::exception &e)
    {
        if (pErrMsg)
           *pErrMsg = e.what();
    }

    if (!jRes.is_null())
        return jRes;

    try
    {
        YAML::Node yamlNode = YAML::Load(in);
        auto nodeType = yamlNode.Type();
        if (nodeType==YAML::NodeType::value::Undefined || nodeType==YAML::NodeType::value::Null)
            return jRes;

        marty::yaml2json::FastSimpleStringStream fssm;
        marty::yaml2json::writeJson(fssm, yamlNode, -1);
        
        if (pTmpJson)
           *pTmpJson = fssm.str();

        jRes = nlohmann::json::parse( fssm.str()
                                    , nullptr        // parser_callback_t
                                    , true           // allow_exceptions
                                    , allowComments  // ignore_comments
                                    );

    }
    catch (const YAML::Exception& e)
    {
        if (pErrMsg)
           *pErrMsg = e.what();
    }
    catch (const std::exception& e)
    {
        if (pErrMsg)
           *pErrMsg = e.what();
    }
    catch (...)
    {
        if (pErrMsg)
           *pErrMsg = "unknown error";
    }

    return jRes;
#endif    
}


/*
    static basic_json parse(IteratorType first,
                            IteratorType last,
                            const parser_callback_t cb = nullptr,
                            const bool allow_exceptions = true,
                            const bool ignore_comments = false)

*/

// auto j4 = json::parse(R"({"happy": true, "pi": 3.141})");





} // namespace json_utils
} // namespace marty

// marty::json_utils::
