#pragma once

#include "yaml-cpp/eventhandler.h"
#include "yaml-cpp/yaml.h"  // IWYU pragma: keep

#include <algorithm>
#include <iterator>


// marty::yaml2json::

namespace marty{
namespace yaml2json{



inline
std::string toLowerCopy( const std::string &str )
{
    std::string res; res.reserve(str.size());

    std::transform( str.begin(), str.end(), std::back_inserter(res)
                  , [](char ch) -> char
                    {
                        if (ch>='A' && ch<='Z')
                            return ch - 'A' + 'a';
                        return ch;
                    }
                  );
    return res;
}



enum class DetectedValueType
{
    literal   , // false, true, null
    number    ,
    string
};


inline
DetectedValueType detectValueType( const std::string &str )
{
    if (str.size()>48)
        return DetectedValueType::string; // таких длинных чисел не бывает

    auto lowerStr = toLowerCopy(str);
    if (lowerStr=="false" || lowerStr=="true" || lowerStr=="null")
       return DetectedValueType::literal; // отловили литералы


    // теперь у нас только строки и числа остались
    const int idxX         = 0;
    const int idxPlus      = 1;
    const int idxMinus     = 2;
    const int idxDigit     = 3;
    const int idxDigitX    = 4;
    const int idxDot       = 5;
    const int idxOthers    = 6;
    // const int idxE         = 7;
    const int idxTotal     = 7;

    int counters[idxTotal] = { 0 };

    auto isDecDigit = [](char ch) -> bool
    {
        return ch>='0' && ch<='9';
    };

    for( char ch : lowerStr)
    {
        switch(ch)
        {
            // case '0': case '1': case '2': case '3': case '4':
            // case '5': case '6': case '7': case '8': case '9':
            //     counters[idxDigit]++;
            //     break;
            //  
            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                counters[idxDigitX]++;
                break;

            case 'x': // поддержка 16-ных чисел
                counters[idxX]++;
                break;

            // Обойдёмся без экспоненциальной формы
            // case 'e':
            //     counters[idxE]++;
            //     break;

            case '+':
                counters[idxPlus]++;
                break;

            case '-':
                counters[idxMinus]++;
                break;

            case '.':
                counters[idxDot]++;
                break;

            default:
                if (isDecDigit(ch))
                   counters[idxDigit]++;
                else
                   counters[idxOthers]++;
                break;
        }
    
    }

    // если что-то левое - явно строка
    if (counters[idxOthers]>0)
        return DetectedValueType::string;

    int decDigits = counters[idxDigit ];
    int hexDigits = counters[idxDigitX]; // only hex digits
    int allDigits = decDigits + hexDigits;
    int allSigns  = counters[idxPlus]+counters[idxMinus];


    //auto removeLeadingPlus()

    // знаков и точек не должно быть больше 1, нет hex цифр и нет hex-маркера и хоть одна десятичная цифра
    // Плюсик - недопустим в нормальном JSON
    if ( /* allSigns */counters[idxMinus] <2 && counters[idxDot]<2 && counters[idxX]==0 && hexDigits==0 && decDigits>0)
    {
        auto pos = 0u;
        while(pos!=str.size() && !isDecDigit(str[pos])) ++pos; // пропускаем все нецифровые символы
        auto lzCounter = 0u;
        while(pos!=str.size() && str[pos]=='0') { ++pos; ++lzCounter; } // считаем ведущие нолики
        auto tailLen = str.size()-pos;
        if (tailLen==0 && lzCounter>0)
            return DetectedValueType::number;
        if (tailLen>0 && lzCounter==0)
            return DetectedValueType::number;
    }

    // hex - недопустим в нормальном JSON
    /*
    // hex - без знака, без точек, хотя бы одна цифра, только один 'x', размер>2 и префикс==0x
    if (allSigns==0 && counters[idxDot]==0 && allDigits>0 && counters[idxX]==1 && str.size()>2 && lowerStr[0]=='0' && lowerStr[1]=='x')
        return DetectedValueType::number;
    */

    if (( /* counters[idxPlus]+ */ counters[idxMinus])<2 && counters[idxDot]==0 && counters[idxDigitX]==0 && counters[idxDigit]>0)
    {
        auto pos = 0u;
        while(pos!=str.size() && !isDecDigit(str[pos])) ++pos; // пропускаем все нецифровые символы
        auto lzCounter = 0u;
        while(pos!=str.size() && str[pos]=='0') { ++pos; ++lzCounter; } // считаем ведущие нолики
        auto tailLen = str.size()-pos;
        if (tailLen==0 && lzCounter>0)
            return DetectedValueType::number;
        if (tailLen>0 && lzCounter==0)
            return DetectedValueType::number;
    }

    return DetectedValueType::string;
}


class FastSimpleStringStream
{
    std::string m_str;

public:

    FastSimpleStringStream() : m_str() {} // = delete;
    //FastSimpleStringStream( std::string &s ) : m_str(s) {}
    FastSimpleStringStream( const FastSimpleStringStream &ss ) : m_str(ss.m_str) {}

    FastSimpleStringStream( FastSimpleStringStream &&) = delete;
    FastSimpleStringStream& operator=( const FastSimpleStringStream &ss ) = delete;

    const std::string& str() const
    {
        return m_str;
    }

    FastSimpleStringStream& operator<<( const char *str )
    {
        m_str.append(str);
        return *this;
    }

    FastSimpleStringStream& operator<<( char ch )
    {
        m_str.append(1, ch);
        return *this;
    }

    FastSimpleStringStream& operator<<( const std::string &str )
    {
        m_str.append(str);
        return *this;
    }

};



// поддерживаются варианты \\, \/,

inline
std::string makeEscapedString( const std::string & str )
{
    std::string res; res.reserve(str.size());

    for( auto ch : str )
    {
        switch(ch)
        {
            case 0x00:   res.append("\\0000"); break;
            case 0x01:   res.append("\\0001"); break;
            case 0x02:   res.append("\\0002"); break;
            case 0x03:   res.append("\\0003"); break;
            case 0x04:   res.append("\\0004"); break;
            case 0x05:   res.append("\\0005"); break;
            case 0x06:   res.append("\\0006"); break;
            case 0x07:   res.append("\\0007"); break;
            case '\b':   res.append("\\b")   ; break; // 0x08
            case '\t':   res.append("\\t")   ; break; // 0x09
            case '\n':   res.append("\\n")   ; break; // 0x0A
            case 0x0B:   res.append("\\000B"); break;
            case '\f':   res.append("\\f")   ; break; // 0x0C
            case '\r':   res.append("\\r")   ; break; // 0x0D
            case 0x0E:   res.append("\\000E"); break;
            case 0x0F:   res.append("\\000F"); break;

            case 0x10:   res.append("\\0010"); break;
            case 0x11:   res.append("\\0011"); break;
            case 0x12:   res.append("\\0012"); break;
            case 0x13:   res.append("\\0013"); break;
            case 0x14:   res.append("\\0014"); break;
            case 0x15:   res.append("\\0015"); break;
            case 0x16:   res.append("\\0016"); break;
            case 0x17:   res.append("\\0017"); break;
            case 0x18:   res.append("\\0018"); break;
            case 0x19:   res.append("\\0019"); break;
            case 0x1A:   res.append("\\001A"); break;
            case 0x1B:   res.append("\\001B"); break;
            case 0x1C:   res.append("\\001C"); break;
            case 0x1D:   res.append("\\001D"); break;
            case 0x1E:   res.append("\\001E"); break;
            case 0x1F:   res.append("\\001F"); break;

            case '\"':   res.append("\\\"")  ; break;
            case '\\':   res.append("\\\\")  ; break;
            // case '/' :   res.append("\\/")  ; break; // ???

            default:     res.append(1, ch);

        }
    }

    return res;
}



inline
bool isNodeScalarSequence( const YAML::Node &node )
{
    if (node.Type()!=YAML::NodeType::value::Sequence)
        return false;

    YAML::Node::const_iterator it = node.begin();
    for (; it!=node.end(); ++it)
    {
        switch( it->Type() )
        {
            case YAML::NodeType::value::Scalar   : 
                 {
                     std::string value  = it->as<std::string>();
                     if (value.size()>24)
                         return false;
                 }
                 continue;

            case YAML::NodeType::value::Undefined: continue;
            case YAML::NodeType::value::Null     : continue;

            case YAML::NodeType::value::Sequence : return false;
            case YAML::NodeType::value::Map      : return false;
        }
    }

    return true;

}

inline
std::string makeQuoted( const std::string & str, bool forceQuoted = false )
{
    if (!forceQuoted)
    {
        auto detectedValueType = detectValueType( str );
        if (detectedValueType==DetectedValueType::string)
            forceQuoted = true;
    }

    if (!forceQuoted)
        return str;

    std::string res; res.reserve(str.size()+8);
    res.append(1,'\"');
    res.append(makeEscapedString(str));
    res.append(1,'\"');
    return res;
}


inline
std::string getNodeTypeName( YAML::NodeType::value type )
{
    switch( type )
    {
        case YAML::NodeType::value::Undefined: return "Undefined";
        case YAML::NodeType::value::Null     : return "Null";
        case YAML::NodeType::value::Scalar   : return "Scalar";
        case YAML::NodeType::value::Sequence : return "Sequence";
        case YAML::NodeType::value::Map      : return "Map";
        default: return "<UNKNOWN>";
    }
}

inline
std::string makeIndentStr( int indent )
{
    if (indent>=0)
        return std::string( (std::string::size_type)indent, ' ' );

    //return std::string(" ");
    return std::string();
}

inline
int calcJsonIndent( int prevIndent, int indentIncrement )
{
    if (prevIndent<0 || indentIncrement<0)
        return -1;
    return prevIndent + indentIncrement;
}

inline 
const char* makeJsonLf(int indent)
{
    static const char* lf   = "\n";
    static const char* nolf = " ";

    return (indent<0) ? nolf : lf;
}


template< typename StreamType > inline
void printScalar( StreamType &s, int indent, const YAML::Node &n, bool isFirst, bool isLast, std::string name = "" )
{
    std::string value = "null";
    if (n.Type()==YAML::NodeType::value::Scalar)
    {
        std::string scalarValue = n.as<std::string>();
        value = makeQuoted(scalarValue);
    }

    s << makeIndentStr(indent);

    if (!name.empty())
    {
        s << makeQuoted(name,true) << ": ";
    }

    s << value;

    if (!isLast)
        s << ",";
    s << makeJsonLf(indent);

}


template< typename StreamType > inline
void writeJsonImpl( StreamType &s, const YAML::Node &node, int indentIncrement, int indent = 0 )
{
    auto nodeType = node.Type();



    switch( nodeType )
    {
        case YAML::NodeType::value::Undefined:
             s << makeIndentStr(indent) << "null" << makeJsonLf(indent);
             return;

        case YAML::NodeType::value::Null     :
             s << makeIndentStr(indent) << "null" << makeJsonLf(indent);
             return;

        case YAML::NodeType::value::Scalar   :
             {
                 std::string value  = node.as<std::string>();
                 s << makeIndentStr(indent) << makeQuoted(value) << makeJsonLf(indent);
             }
             return;

        case YAML::NodeType::value::Sequence :
             {
                 YAML::Node::const_iterator it = node.begin();
                 for (; it!=node.end(); ++it)
                 {
                     bool isFirst = it==node.begin();
                     auto nextIt  = it; ++nextIt;
                     bool isLast  = nextIt==node.end();

                     switch( it->Type() )
                     {
                         case YAML::NodeType::value::Undefined:
                         case YAML::NodeType::value::Null     :
                         case YAML::NodeType::value::Scalar   :
                              printScalar( s, indent, *it, isFirst, isLast);
                              break;

                         case YAML::NodeType::value::Sequence :
                              {
                                  int seqIndent = indent;

                                  s << makeIndentStr(seqIndent);

                                  if (isNodeScalarSequence(*it))
                                     seqIndent = -1;

                                  s << "[" << makeJsonLf(seqIndent);
                                  writeJsonImpl( s, *it, indentIncrement, calcJsonIndent(seqIndent,indentIncrement) );
                                  s << makeIndentStr(seqIndent) << "]";
                                  if (!isLast)
                                      s << ",";
                                  s << makeJsonLf(indent);
                              }
                              break;

                         case YAML::NodeType::value::Map      :
                              {
                                  s << makeIndentStr(indent);
                                  s << "{" << makeJsonLf(indent);
                                  writeJsonImpl( s, *it, indentIncrement, calcJsonIndent(indent,indentIncrement) );
                                  s << makeIndentStr(indent) << "}";
                                  if (!isLast)
                                      s << ",";
                                  s << makeJsonLf(indent);
                              }
                              break;
                     }
                 }

             }
             return;

        case YAML::NodeType::value::Map      :
             {
                 YAML::Node::const_iterator it = node.begin();
                 for (; it!=node.end(); ++it)
                 {
                     bool isFirst = it==node.begin();
                     auto nextIt  = it; ++nextIt;
                     bool isLast  = nextIt==node.end();

                     std::string name  = it->first .as<std::string>();
                     switch( it->second.Type() )
                     {
                         case YAML::NodeType::value::Undefined:
                         case YAML::NodeType::value::Null     :
                         case YAML::NodeType::value::Scalar   :
                              printScalar( s, indent, it->second, isFirst, isLast, name);
                              break;

                         case YAML::NodeType::value::Sequence :
                              {
                                  int seqIndent = indent;

                                  s << makeIndentStr(seqIndent);

                                  if (isNodeScalarSequence(it->second))
                                     seqIndent = -1;

                                  s << makeQuoted(name,true) << ": [" << makeJsonLf(seqIndent);
                                  writeJsonImpl( s, it->second, indentIncrement, calcJsonIndent(seqIndent,indentIncrement) );
                                  s << makeIndentStr(seqIndent) << "]";
                                  if (!isLast)
                                      s << ",";
                                  s << makeJsonLf(indent);
                              }
                              break;

                         case YAML::NodeType::value::Map      :
                              {
                                  s << makeIndentStr(indent);
                                  s << makeQuoted(name,true) << ": {" << makeJsonLf(indent);
                                  writeJsonImpl( s, it->second, indentIncrement, calcJsonIndent(indent,indentIncrement) );
                                  s << makeIndentStr(indent) << "}";
                                  if (!isLast)
                                      s << ",";
                                  s << makeJsonLf(indent);
                              }
                              break;
                     }
                 }
             }
             return;

        default:
             {
             }
             return;

    }
}

template< typename StreamType > inline
void writeJson( StreamType &s, const YAML::Node &node, int indentIncrement, int indent = 0 )
{
    if (indentIncrement<0)
        indent = -1;

    auto nodeType = node.Type();

    //int calcJsonIndent( int prevIndent, int indentIncrement )
    switch( nodeType )
    {
        case YAML::NodeType::value::Undefined:
        case YAML::NodeType::value::Null     :
        case YAML::NodeType::value::Scalar   :
             printScalar( s, indent, node, true, true );
             break;

        case YAML::NodeType::value::Sequence :
             {
                 s << makeIndentStr(indent) << "[" << makeJsonLf(indent);
                 writeJsonImpl( s, node, indentIncrement, calcJsonIndent(indent,indentIncrement) );
                 s << makeIndentStr(indent) << "]" << makeJsonLf(indent);
             }
             break;

        case YAML::NodeType::value::Map      :
             {
                 s << makeIndentStr(indent) << "{" << makeJsonLf(indent);
                 writeJsonImpl( s, node, indentIncrement, calcJsonIndent(indent,indentIncrement) );
                 s << makeIndentStr(indent) << "}" << makeJsonLf(indent);
             }
             break;

        default:
             {
                 writeJsonImpl( s, node, indentIncrement, indent );
             }
             return;

    }


}






} // namespace yaml2json
} // marty

// marty::yaml2json::


