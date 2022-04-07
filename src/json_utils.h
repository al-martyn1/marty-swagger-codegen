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

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
enum class JsonNodeType
{
    unknown        ,

    // scalar node types
    null           ,
    boolean        ,
    numberUnsigned ,
    numberInteger  ,
    numberFloat    ,
    string         ,

    array          ,

    object

}; // enum class JsonNodeType

//------------------------------
inline
JsonNodeType nodeType(const nlohmann::json &jNode)
{
    if (jNode.is_null())
        return JsonNodeType::null;

    else if (jNode.is_boolean())
        return JsonNodeType::boolean;

    else if (jNode.is_number_integer() && jNode.is_number_unsigned())
        return JsonNodeType::numberUnsigned;

    else if (jNode.is_number_integer())
        return JsonNodeType::numberInteger;

    else if (jNode.is_number_float())
        return JsonNodeType::numberFloat;

    else if (jNode.is_string())
        return JsonNodeType::string;

    else if (jNode.is_array())
        return JsonNodeType::array;

    else if (jNode.is_object())
        return JsonNodeType::object;

    else 
        return JsonNodeType::unknown;

}

//------------------------------
inline
std::string nodeTypeName( JsonNodeType nt )
{
    switch(nt)
    {
        case JsonNodeType::null           : return "null"    ;
        case JsonNodeType::boolean        : return "boolean" ;
        case JsonNodeType::numberUnsigned : return "unsigned";
        case JsonNodeType::numberInteger  : return "integer" ;
        case JsonNodeType::numberFloat    : return "float"   ;
        case JsonNodeType::string         : return "string"  ;
        case JsonNodeType::array          : return "array"   ;
        case JsonNodeType::object         : return "object"  ;
        default                           : return "unknown" ;
    }
}

//------------------------------
inline
bool isScalarNode( JsonNodeType nodeType )
{
    if (nodeType==JsonNodeType::array || nodeType==JsonNodeType::object)
        return false;
    return true;
}

//------------------------------
inline
bool isArrayNode( JsonNodeType nodeType )
{
    if (nodeType==JsonNodeType::array)
        return true;
    return false;
}

//------------------------------
inline
bool isObjectNode( JsonNodeType nodeType )
{
    if (nodeType==JsonNodeType::object)
        return true;
    return false;
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
enum class FileFormat
{
    unknown = 0,
    json,
    yaml
};

// 
nlohmann::json parseJsonOrYaml( const std::string &data
                              , bool allowComments = true
                              , std::string *pErrMsg = 0
                              , std::string *pTmpJson = 0
                              , FileFormat *pDetectedFormat = 0
                              )
{
    nlohmann::json jRes;

    FileFormat detectedFormat = FileFormat::unknown;

    if (pDetectedFormat)
        *pDetectedFormat = detectedFormat;

    bool jsonStartFound = false;
    std::string::size_type pos = 0;
    for(; pos!=data.size(); ++pos)
    {
        if (data[pos]==' ')
            continue;

        if (data[pos]=='{')
            jsonStartFound = true;
            
        break;
    }

    if (jsonStartFound)
    {
        try
        {
            jRes = nlohmann::json::parse( data
                                        , nullptr        // parser_callback_t
                                        , true           // allow_exceptions
                                        , allowComments  // ignore_comments
                                        );
            detectedFormat = FileFormat::json;
        }
        catch(const std::exception &e)
        {
            if (pErrMsg)
               *pErrMsg = e.what();
        }
    
        if (pDetectedFormat)
            *pDetectedFormat = detectedFormat;
        return jRes;
    }



    try
    {
        YAML::Node yamlNode = YAML::Load(data);

        marty::yaml2json::FastSimpleStringStream fssm;
        //marty::yaml2json::writeJson(fssm, yamlNode, -1);
        marty::yaml2json::writeJson(fssm, yamlNode, 2);

        if (pTmpJson)
           *pTmpJson = fssm.str();
        
        jRes = nlohmann::json::parse( fssm.str()
                                    , nullptr        // parser_callback_t
                                    , true           // allow_exceptions
                                    , allowComments  // ignore_comments
                                    );
        if (pDetectedFormat)
            *pDetectedFormat = FileFormat::yaml;


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

//------------------------------
// file_input_adapter
// input_stream_adapter
// iterator_input_adapter

nlohmann::json parseJsonOrYaml( std::istream &in
                              , bool allowComments = true
                              , std::string *pErrMsg = 0
                              , std::string *pTmpJson = 0
                              , FileFormat *pDetectedFormat = 0
                              )
{
    std::string data;
    char buffer[4096];
    while (in.read(buffer, sizeof(buffer)))
        data.append(buffer, sizeof(buffer));
    data.append(buffer, in.gcount());
    
    return parseJsonOrYaml( data, allowComments, pErrMsg, pTmpJson, pDetectedFormat );

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
#endif    
}

//----------------------------------------------------------------------------
inline
std::string makeIndentStr( int indent )
{
    if (indent>=0)
        return std::string( (std::string::size_type)indent, ' ' );

    //return std::string(" ");
    return std::string();
}

//----------------------------------------------------------------------------
inline
bool isScalar( nlohmann::json &j )
{
    if (j.is_null() || j.is_boolean() || j.is_number() || j.is_string())
        return true;
    return false;
}

//----------------------------------------------------------------------------
template<typename StreamType> inline
bool writeScalar( StreamType &s, nlohmann::json &j )
{
    if (j.is_null())
    {
        s << "null";
    }
    else if (j.is_boolean())
    {
        auto val = j.get<bool>();
        s << (val?"true":"false");
    }
    else if (j.is_number_integer() && j.is_number_unsigned())
    {
        auto val = j.get<std::uint64_t>();
        s << val;
    }
    else if (j.is_number_integer())
    {
        auto val = j.get<std::int64_t>();
        s << val;
    }
    else if (j.is_number_float())
    {
        auto val = j.get<double>();
        s << val;
    }
    else if (j.is_string())
    {
        auto val = j.get<std::string>();
        if (val.empty() || val=="null")
            s << '\'' << val << '\'';
        else
            s << val;
    }
    else // j.is_object() || j.is_array()
    {
        return false;;
    }

    return true;
}

//----------------------------------------------------------------------------
template<typename StreamType>
void writeNodeImpl( StreamType &s, nlohmann::json &j // j - не меняется, просто нет константной версии begin/end
                  , int indent, int indentInc, bool noFirstIndent = false
                  ) 
{
    if (indent<0)
        indent = 0;

    if (indentInc<1)
        indentInc = 1;

    if (writeScalar( s, j ))
    {
        return;
    }
    else if (j.is_object())
    {
        bool bFirst = true;
        for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it)
        {
            if (!(bFirst && noFirstIndent))
            {
                s << makeIndentStr(indent);
            }

            s << it.key() << ":";
            auto val = it.value();
            if (isScalar(val))
            {
                s << " "; 
                writeScalar(s,val);
                s << "\n";
            }
            else
            {
                s << "\n";
                writeNodeImpl( s, val, indent+indentInc, indentInc );
            }

            bFirst = false;
        }

    }
    else if (j.is_array())
    {
        bool bFirst = true;
        for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it)
        {
            s << makeIndentStr(indent) << "- ";
            if (isScalar(*it))
            {
                writeScalar(s,*it);
                s << "\n";
            }
            else
            {
                writeNodeImpl( s, *it, indent+indentInc, indentInc, true /* noFirstIndent */  );
            }

            bFirst = false;
        }
    }

}

//----------------------------------------------------------------------------
template<typename StreamType> inline
void writeYaml( StreamType &s, nlohmann::json &j // j - не меняется, просто нет константной версии begin/end
              )
{
    writeNodeImpl( s, j, 0, 2, false );
}





} // namespace json_utils
} // namespace marty

// marty::json_utils::

