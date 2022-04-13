#pragma once


/*
    x-waiters - F:\_github\__3dp\APIs_guru_openapi_directory\APIs\amazonaws.com\appstream\2016-12-01\openapi.yaml


 */


#include "yaml_utils.h"
#include "json_utils.h"

#include <optional>
#include <string>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <typeinfo>
#include <variant>
#include <string>


#include "dependency_finder.h"

// marty::swagger::

namespace marty{
namespace swagger{



/* std::optional<> - https://habr.com/ru/post/372103/
                     https://en.cppreference.com/w/cpp/utility/optional

   using (type alias) - https://en.cppreference.com/w/cpp/language/type_alias
 */

//----------------------------------------------------------------------------
template < typename T >
using Opt = std::optional<T>;

/*

std::optional<...> opt;
std::optional<...> opt = {};
std::optional<...> opt = std::nullopt;

auto oDouble = std::make_optional(3.0);
std::optional<...> o = {std::in_place, 3.0};


Получить сохранённое значение.
Для этого есть несколько вариантов:

Использовать operator*() и operator->() так же, как в итераторах. Если объект не содержит реального значения, то поведение не определено!
value() — возвращает значение или бросает исключение std::bad_optional_access.
value_or(default) — возвращает значение, если доступно, или же возвращает default.

Чтобы проверить, есть ли реальное значение в объекте, вы можете использовать метод has_value() 
или просто проверить объект с помощью if (optional) {...}
 */

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
struct ArrayText
{
    std::string value;

    ArrayText() : value() {}
    ArrayText( const ArrayText &a) : value(a.value) {}
    ArrayText& operator=( const ArrayText &a) { value = a.value; return *this; }
    ArrayText(const std::string &s) : value(s) {}
    operator std::string() const { return value; }
};

struct ObjectText
{
    std::string value;

    ObjectText() : value() {}
    ObjectText( const ObjectText &a) : value(a.value) {}
    ObjectText& operator=( const ObjectText &a) { value = a.value; return *this; }
    ObjectText(const std::string &s) : value(s) {}
    operator std::string() const { return value; }
};

//----------------------------------------------------------------------------
// https://json.nlohmann.me/features/types/number_handling/#number-conversion
// typedef 
using json_scalar = 
std::variant< bool             // std::get<0>(...)
            , std::int64_t     // std::get<1>(...)
            , std::uint64_t    // std::get<2>(...)
            , double           // std::get<3>(...)
            , std::string      // std::get<4>(...)
            , ArrayText        // std::get<5>(...)
            , ObjectText       // std::get<6>(...)
            >
            ; // json_scalar;

// typedef 
using json_number = 
std::variant< std::int64_t     // std::get<0>(...)
            , std::uint64_t    // std::get<1>(...)
            , double           // std::get<2>(...)
            >
            ; // json_number;

//----------------------------------------------------------------------------
void to_json( nlohmann::json& jNode, const json_scalar& scalarVariant);
void from_json( const nlohmann::json& jNode, json_scalar& scalarVariant);
void to_json( nlohmann::json& jNode, const json_number& num);
void from_json(const nlohmann::json& jNode, json_number &num);

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
namespace util
{




//----------------------------------------------------------------------------
inline
json_scalar getScalarFromJson(const nlohmann::json& jScalarNode)
{
    if (jScalarNode.is_boolean())
    {
        return json_scalar(jScalarNode.get<bool>());
    }
    else if (jScalarNode.is_number_integer() && jScalarNode.is_number_unsigned())
    {
        return json_scalar(jScalarNode.get<std::uint64_t>());
    }
    else if (jScalarNode.is_number_integer())
    {
        return json_scalar(jScalarNode.get<std::int64_t>());
    }
    else if (jScalarNode.is_number()) // is_number_float
    {
        return json_scalar(jScalarNode.get<double>());
    }
    else if (jScalarNode.is_array())
    {
        return json_scalar(ArrayText(std::string(jScalarNode)));
    }
    else if (jScalarNode.is_object())
    {
        return json_scalar(ObjectText(std::string(jScalarNode)));
    }
    else
    {
        return jScalarNode.get<std::string>(); // если что-то пойдёт не так, то тут вылетит птичка
    }
}

//----------------------------------------------------------------------------
inline
std::string makeStringFromScalarVariant( const json_scalar  &scalarVariant )
{
    if (std::holds_alternative<bool>(scalarVariant))
    {
        return std::get<0>(scalarVariant) ? "true" : "false";
    }
    else if (std::holds_alternative<std::int64_t>(scalarVariant))
    {
        return std::to_string(std::get<1>(scalarVariant));
    }
    else if (std::holds_alternative<std::uint64_t>(scalarVariant))
    {
        return std::to_string(std::get<2>(scalarVariant));
    }
    else if (std::holds_alternative<double>(scalarVariant))
    {
        std::ostringstream oss;
        oss << std::get<3>(scalarVariant);
        return oss.str();
    }
    else if (std::holds_alternative<std::string>(scalarVariant))
    {
        return std::get<4>(scalarVariant);
    }
    else if (std::holds_alternative<ArrayText>(scalarVariant))
    {
        return std::string(std::get<5>(scalarVariant));
    }
    else if (std::holds_alternative<ObjectText>(scalarVariant))
    {
        return std::string(std::get<6>(scalarVariant));
    }
    else
    {
        throw std::runtime_error("getScalarStringFromJson: bad alternative");
    }

    return std::string();
}

//----------------------------------------------------------------------------
inline
std::string getScalarStringFromJson(const nlohmann::json& jNode)
{
    if (jNode.is_object() || jNode.is_array())
    {
        return jNode.dump();
    }

    json_scalar scalarVariant = getScalarFromJson(jNode);

    return makeStringFromScalarVariant(scalarVariant);
}

//----------------------------------------------------------------------------
template<typename T> inline
void jsonGetOptVal( const nlohmann::json& jNode, const std::string &name, Opt<T> &extractTo )
{
    auto iter = jNode.find(name);

    if (iter != jNode.end())
    {
        const nlohmann::json& jFound = *iter;
        extractTo = jFound.get<T>(); // std::make_optional(jNode[name].get<T>());
        //extractTo = jNode[name].get<T>(); // std::make_optional(jNode[name].get<T>());
    }
    else
    {
        extractTo = {}; // std::nullopt
    }
}

//----------------------------------------------------------------------------
template<> inline
void jsonGetOptVal<std::string>( const nlohmann::json& jNode, const std::string &name, Opt<std::string> &extractTo )
{
    auto iter = jNode.find(name);

    if (iter!=jNode.end() && !iter->is_null())
    {
        const nlohmann::json& jFound = *iter;
        extractTo = getScalarStringFromJson(jFound);
    }
    else
    {
        extractTo = {}; // std::nullopt
    }
}

//----------------------------------------------------------------------------
inline
void jsonGetOptVal( const nlohmann::json& jNode, const std::string &name, Opt< std::vector<std::string> > &extractTo )
{
    auto iter = jNode.find(name);

    if (iter!=jNode.end() && !iter->is_null())
    {
        const nlohmann::json& jFound = *iter;

        if (jFound.is_array())
        {
            std::vector<std::string> vec;
            for (nlohmann::json::const_iterator it = jFound.begin(); it != jFound.end(); ++it)
            {
                const auto &jItem = *it;
                vec.emplace_back(getScalarStringFromJson(jItem));
            }

            extractTo = vec;
        }
        else
        {
            //extractTo = getScalarStringFromJson(jFound);
        }
    }
    else
    {
        extractTo = {}; // std::nullopt
    }
}

//----------------------------------------------------------------------------
template<> inline
void jsonGetOptVal<json_scalar>( const nlohmann::json& jNode, const std::string &name, Opt<json_scalar> &extractTo )
{
    auto iter = jNode.find(name);

    if (iter!=jNode.end() && !iter->is_null())
    {
        const nlohmann::json& jFound = *iter;
        json_scalar tmp;
        from_json(jFound,tmp);
        extractTo = tmp;
    }
    else
    {
        extractTo = {}; // std::nullopt
    }
}

//----------------------------------------------------------------------------
template<> inline
void jsonGetOptVal<json_number>( const nlohmann::json& jNode, const std::string &name, Opt<json_number> &extractTo )
{
    auto iter = jNode.find(name);

    if (iter!=jNode.end() && !iter->is_null())
    {
        const nlohmann::json& jFound = *iter;
        json_number tmp;
        from_json(jFound,tmp);
        extractTo = tmp;
    }
    else
    {
        extractTo = {}; // std::nullopt
    }
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template<typename T> inline
void extractFromJsonTo( const nlohmann::json& jNode, const std::string &name, Opt<T> &extractTo )
{
    try
    {
        if (jNode.contains(name))
        {
            jsonGetOptVal( jNode, name, extractTo );
        }
        else
        {
            extractTo = {}; // std::nullopt
        }
    }
    catch( const nlohmann::detail::type_error &e )
    {
        std::stringstream ss;
        ss << e.what() << "\n";

        // https://stackoverflow.com/questions/4484982/how-to-convert-typename-t-to-string-in-c
        ss << "Details: name - " << name << ", type - " << typeid(T).name() << "\n";
        ss << "Node: " << jNode.dump(2); // << "\n";
        throw std::runtime_error(ss.str());
    }
}

//----------------------------------------------------------------------------
template<typename T> inline
void convertToJsonFrom( nlohmann::json& jNode, const std::string &name, const Opt<T> &convertFrom )
{
    if (convertFrom)
        jNode[name] = *convertFrom;
}

//----------------------------------------------------------------------------
void convertToJsonFrom( nlohmann::json& jNode, const std::string &name, const Opt< json_scalar > &convertFrom )
{
    if (convertFrom)
    {
        to_json(jNode[name], *convertFrom);
        #if 0
        const json_scalar &scalarVariant = *convertFrom;

        if (std::holds_alternative<bool>(scalarVariant))
        {
            jNode[name] = std::get<0>(scalarVariant);
        }
        else if (std::holds_alternative<std::int64_t>(scalarVariant))
        {
            jNode[name] = std::get<1>(scalarVariant);
        }
        else if (std::holds_alternative<std::uint64_t>(scalarVariant))
        {
            jNode[name] = std::get<2>(scalarVariant);
        }
        else if (std::holds_alternative<double>(scalarVariant))
        {
            jNode[name] = std::get<3>(scalarVariant);
        }
        else if (std::holds_alternative<std::string>(scalarVariant))
        {
            jNode[name] = std::get<4>(scalarVariant);
        }
        else if (std::holds_alternative<ArrayText>(scalarVariant))
        {
            jNode[name] = std::string(std::get<5>(scalarVariant));
        }
        else if (std::holds_alternative<ObjectText>(scalarVariant))
        {
            jNode[name] = std::string(std::get<6>(scalarVariant));
        }
        else
        {
            throw std::runtime_error("convertToJsonFrom: bad alternative");
        }
        #endif
    }
}

//----------------------------------------------------------------------------
void convertToJsonFrom( nlohmann::json& jNode, const std::string &name, const Opt< json_number > &convertFrom )
{
    if (convertFrom)
    {
        to_json(jNode[name], *convertFrom);
        #if 0
        const json_number &numberVariant = *convertFrom;

        if (std::holds_alternative<std::int64_t>(numberVariant))
        {
            jNode[name] = std::get<0>(numberVariant);
        }
        else if (std::holds_alternative<std::uint64_t>(numberVariant))
        {
            jNode[name] = std::get<1>(numberVariant);
        }
        else if (std::holds_alternative<double>(numberVariant))
        {
            jNode[name] = std::get<2>(numberVariant);
        }
        else
        {
            throw std::runtime_error("convertToJsonFrom: bad alternative");
        }
        #endif
    }
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------

} // namespace util

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
inline
void to_json( nlohmann::json& jNode, const json_scalar& scalarVariant)
{
    if (std::holds_alternative<bool>(scalarVariant))
    {
        jNode = std::get<0>(scalarVariant);
    }
    else if (std::holds_alternative<std::int64_t>(scalarVariant))
    {
        jNode = std::get<1>(scalarVariant);
    }
    else if (std::holds_alternative<std::uint64_t>(scalarVariant))
    {
        jNode = std::get<2>(scalarVariant);
    }
    else if (std::holds_alternative<double>(scalarVariant))
    {
        jNode = std::get<3>(scalarVariant);
    }
    else if (std::holds_alternative<std::string>(scalarVariant))
    {
        jNode = std::get<4>(scalarVariant);
    }
    else if (std::holds_alternative<ArrayText>(scalarVariant))
    {
        jNode = std::string(std::get<5>(scalarVariant));
    }
    else if (std::holds_alternative<ObjectText>(scalarVariant))
    {
        jNode = std::string(std::get<6>(scalarVariant));
    }
    else
    {
        throw std::runtime_error("void to_json( nlohmann::json& jNode, const json_scalar& scalarVariant): bad alternative");
    }
}

//------------------------------
inline
void from_json( const nlohmann::json& jNode, json_scalar& scalarVariant)
{
    scalarVariant = util::getScalarFromJson(jNode);
}

//------------------------------
inline
void to_json( nlohmann::json& jNode, const json_number& num)
{
    if (std::holds_alternative<std::int64_t>(num))
    {
        jNode = std::get<0>(num);
    }
    else if (std::holds_alternative<std::uint64_t>(num))
    {
        jNode = std::get<1>(num);
    }
    else if (std::holds_alternative<double>(num))
    {
        jNode = std::get<2>(num);
    }
    else
    {
        throw std::runtime_error("void to_json( nlohmann::json& jNode, const json_number& num): bad alternative");
    }
}

//------------------------------
inline
void from_json(const nlohmann::json& jNode, json_number &num)
{
    static const std::string errBase = "void from_json(const nlohmann::json& jNode, json_number &num): bad alternative - ";

    if (jNode.is_number_integer() && jNode.is_number_unsigned())
    {
        num = jNode.get<std::uint64_t>();
    }
    else if (jNode.is_number_integer())
    {
        num = jNode.get<std::int64_t>();
    }
    else if (jNode.is_number()) // is_number_float
    {
        num = jNode.get<double>();
    }
    else if (jNode.is_boolean())
    {
        throw std::runtime_error(errBase + "bool" + ", JSON: " + jNode.dump());
    }
    else if (jNode.is_string())
    {
        throw std::runtime_error(errBase + "string" + ", JSON: " + jNode.dump());
    }
    else if (jNode.is_array())
    {
        throw std::runtime_error(errBase + "array" + ", JSON: " + jNode.dump());
    }
    else if (jNode.is_object())
    {
        throw std::runtime_error(errBase + "object" + ", JSON: " + jNode.dump());
    }
    else if (jNode.is_null())
    {
        throw std::runtime_error(errBase + "null" + ", JSON: " + jNode.dump());
    }
    else
    {
        throw std::runtime_error(errBase + "unknown" + ", JSON: " + jNode.dump());
    }
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------

#define    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, member )    marty::swagger::util::extractFromJsonTo(jNode, #member, o.member)
#define    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, member )      marty::swagger::util::convertToJsonFrom(jNode, #member, o.member )

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
#define MARTY_SWAGGER_TO_JSON_IMPL_BEGIN(ObjectType)                  \
            inline                                                    \
            void to_json( nlohmann::json& jNode, const ObjectType& o) \
            {

#define MARTY_SWAGGER_TO_JSON_IMPL_END(ObjectType)                    \
            }


#define MARTY_SWAGGER_FROM_JSON_IMPL_BEGIN(ObjectType)                  \
            inline                                                      \
            void from_json( const nlohmann::json& jNode, ObjectType& o) \
            {                                                           \
                try                                                     \
                {

#define MARTY_SWAGGER_FROM_JSON_IMPL_END(ObjectType)                    \
            }                                                           \
            catch(const std::exception *e)                              \
            {                                                           \
                std::stringstream ss;                                   \
                ss << e.what() << "\n";                                 \
                ss << "While converting  " << typeid(o).name() << " from JSON"; \
                throw std::runtime_error(ss.str());                     \
            }                                                           \
        }



//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#external-documentation-object
struct ExternalDocumentationObject
{
    Opt< std::string    >                              description  ;
    Opt< std::string    >                              url          ; // REQUIRED

}; // struct ExternalDocumentationObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const ExternalDocumentationObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, url         );
}

inline
void from_json( const nlohmann::json& jNode, ExternalDocumentationObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, url         );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#oauth-flow-object
struct OAuthFlowObject 
{
    Opt< std::string    >                              authorizationUrl	; // REQUIRED
    Opt< std::string    >                              tokenUrl         ; // REQUIRED
    Opt< std::string    >                              refreshUrl       ;
    Opt< std::map<std::string, std::string> >          scopes           ; // REQUIRED

}; // struct OAuthFlowObject 

//------------------------------
inline
void to_json( nlohmann::json& jNode, const OAuthFlowObject & o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, authorizationUrl );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, tokenUrl         );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, refreshUrl       );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, scopes           );
}

inline
void from_json( const nlohmann::json& jNode, OAuthFlowObject & o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, authorizationUrl );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, tokenUrl         );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, refreshUrl       );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, scopes           );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#oauth-flows-object
struct OAuthFlowsObject 
{
    Opt< OAuthFlowObject >                              implicit          ;
    Opt< OAuthFlowObject >                              password          ;
    Opt< OAuthFlowObject >                              clientCredentials ;
    Opt< OAuthFlowObject >                              authorizationCode ;

}; // struct OAuthFlowsObject 

//------------------------------
inline
void to_json( nlohmann::json& jNode, const OAuthFlowsObject & o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, implicit          );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, password          );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, clientCredentials );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, authorizationCode );
}

inline
void from_json( const nlohmann::json& jNode, OAuthFlowsObject & o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, implicit          );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, password          );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, clientCredentials );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, authorizationCode );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#security-scheme-object
struct SecuritySchemeObject 
{
    Opt< std::string    >                              type            ; // REQUIRED
    Opt< std::string    >                              description     ;
    Opt< std::string    >                              name            ; // REQUIRED
    Opt< std::string    >                              in              ; // REQUIRED
    Opt< std::string    >                              scheme          ; // REQUIRED
    Opt< std::string    >                              bearerFormat    ;
    Opt< OAuthFlowsObject >                            flows           ; // REQUIRED
    Opt< std::string    >                              openIdConnectUrl; // REQUIRED

    Opt< std::string    >                              x_amazon_apigateway_authtype;

}; // struct SecuritySchemeObject 

//------------------------------
inline
void to_json( nlohmann::json& jNode, const SecuritySchemeObject & o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, type             );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description      );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, name             );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, in               );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, scheme           );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, bearerFormat     );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, flows            );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, openIdConnectUrl );
    util::convertToJsonFrom(jNode, "x-amazon-apigateway-authtype", o.x_amazon_apigateway_authtype );
}

inline
void from_json( const nlohmann::json& jNode, SecuritySchemeObject & o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, type             );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description      );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, name             );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, in               );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, scheme           );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, bearerFormat     );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, flows            );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, openIdConnectUrl );
    util::extractFromJsonTo(jNode, "x-amazon-apigateway-authtype", o.x_amazon_apigateway_authtype );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#tagObject
struct TagObject
{
    Opt< std::string    >                 name              ;
    Opt< std::string    >                 description       ;
    Opt< ExternalDocumentationObject >    externalDocs      ;
    

}; // struct TagObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const TagObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, name         );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description  );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, externalDocs );
}

inline
void from_json( const nlohmann::json& jNode, TagObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, name         );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description  );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, externalDocs );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#contact-object
struct ContactObject
{
    Opt< std::string    >   name              ;
    Opt< std::string    >   url               ;
    Opt< std::string    >   email             ;

}; // struct ContactObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const ContactObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, name  );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, url   );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, email );
}

inline
void from_json( const nlohmann::json& jNode, ContactObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, name  );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, url   );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, email );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#licenseObject
struct LicenseObject
{
    Opt< std::string    >   name              ; // REQUIRED
    Opt< std::string    >   identifier        ;
    Opt< std::string    >   url               ;

}; // struct LicenseObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const LicenseObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, name       );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, identifier );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, url        );
}

inline
void from_json( const nlohmann::json& jNode, LicenseObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, name       );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, identifier );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, url        );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
/*
      converter:
        url: 'https://github.com/mermade/aws2openapi'
        version: 1.0.0
 */
struct XOriginItemConverterObject
{
    Opt< std::string    >   url               ;
    Opt< std::string    >   version           ;
}; // struct XOriginItemConverterObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const XOriginItemConverterObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, url     );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, version );
}

inline
void from_json( const nlohmann::json& jNode, XOriginItemConverterObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, url     );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, version );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
/*
  x-origin:
    - format: swagger
      url: 'https://raw.githubusercontent.com/AuthentiqID/authentiq-docs/master/docs/swagger/issuer.yaml'
      version: '2.0'

  x-origin:
    - contentType: application/json
      url: 'https://raw.githubusercontent.com/aws/aws-sdk-js/master/apis/acm-2015-12-08.normal.json'
      converter:
        url: 'https://github.com/mermade/aws2openapi'
        version: 1.0.0
      x-apisguru-direct: true

 */
struct XOriginItemObject
{
    Opt< std::string    >   format             ;
    Opt< std::string    >   url                ;
    Opt< std::string    >   version            ;
    Opt< std::string    >   contentType        ;
    Opt< bool           >   x_apisguru_direct  ;
    Opt< XOriginItemConverterObject > converter;

}; // struct XOriginItemObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const XOriginItemObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, format  );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, url     );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, version );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, contentType );
    util::convertToJsonFrom(jNode, "x-apisguru-direct", o.x_apisguru_direct );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, converter );
}

inline
void from_json( const nlohmann::json& jNode, XOriginItemObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, format  );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, url     );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, version );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, contentType );
    util::extractFromJsonTo(jNode, "x-apisguru-direct", o.x_apisguru_direct );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, converter );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
/*
  x-logo:
    backgroundColor: '#F26641'
    url: 'https://www.authentiq.com/theme/images/authentiq-logo-a-inverse.svg'
 */
struct XLogoObject
{
    Opt< std::string    >   url              ;
    Opt< std::string    >   backgroundColor  ;

}; // struct XLogoObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const XLogoObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, url             );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, backgroundColor );
}

inline
void from_json( const nlohmann::json& jNode, XLogoObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, url );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, backgroundColor );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
/*
  x-apiClientRegistration:
    url: 'https://portal.aws.amazon.com/gp/aws/developer/registration/index.html?nc2=h_ct'
 */
struct XApiClientRegistrationObject
{
    Opt< std::string    >   url              ;

}; // struct XApiClientRegistrationObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const XApiClientRegistrationObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, url             );
}

inline
void from_json( const nlohmann::json& jNode, XApiClientRegistrationObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, url );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#infoObject
struct InfoObject
{
    Opt< std::string    >                   title          ; // REQUIRED
    Opt< std::string    >                   summary        ;
    Opt< std::string    >                   description    ;
    Opt< std::string    >                   termsOfService ;
    Opt< ContactObject  >                   contact        ;
    Opt< LicenseObject  >                   license        ;
    Opt< std::string    >                   version        ;

    Opt< std::string    >                   x_providerName ;
    Opt< std::string    >                   x_serviceName  ;
    Opt< std::string    >                   x_release      ;
    Opt< bool           >                   x_preferred    ;
    Opt< std::vector<std::string> >         x_tags         ;
    Opt< bool           >                   x_unofficialSpec;
    Opt< std::string    >                   x_description_language;

/*
  x-origin:
    - format: swagger
      url: 'http://1forge.com/openapi.json'
      version: '2.0'
 */
    Opt< std::vector<XOriginItemObject> >   x_origin       ;

/*
  x-apisguru-categories:
    - financial
*/
    Opt< std::vector<std::string> >         x_apisguru_categories;

/*
  x-logo:
    backgroundColor: '#F26641'
    url: 'https://www.authentiq.com/theme/images/authentiq-logo-a-inverse.svg'
 */
    Opt< XLogoObject    >                   x_logo         ;

/*
  x-apiClientRegistration:
    url: 'https://portal.aws.amazon.com/gp/aws/developer/registration/index.html?nc2=h_ct'
*/
    Opt< XApiClientRegistrationObject >     x_apiClientRegistration ;


/*
  x-preferred: true

*/

}; // struct InfoObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const InfoObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, title            );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, summary          );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description      );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, termsOfService   );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, contact          );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, license          );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, version          );
    util::convertToJsonFrom(jNode, "x-providerName", o.x_providerName );
    util::convertToJsonFrom(jNode, "x-serviceName", o.x_serviceName   );
    util::convertToJsonFrom(jNode, "x-release"  , o.x_release         );
    util::convertToJsonFrom(jNode, "x-preferred", o.x_preferred       );
    util::convertToJsonFrom(jNode, "x-tags"     , o .x_tags      );
    util::convertToJsonFrom(jNode, "x-unofficialSpec", o .x_unofficialSpec);
    util::convertToJsonFrom(jNode, "x-origin"   , o.x_origin          );
    util::convertToJsonFrom(jNode, "x-apisguru-categories", o.x_apisguru_categories );
    util::convertToJsonFrom(jNode, "x-logo"     , o.x_logo            );
    util::convertToJsonFrom(jNode, "x-apiClientRegistration", o.x_apiClientRegistration );
    util::convertToJsonFrom(jNode, "x-description-language", o.x_description_language );
}

inline
void from_json( const nlohmann::json& jNode, InfoObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, title          );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, summary        );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description    );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, termsOfService );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, contact        );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, license        );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, version        );
    util::extractFromJsonTo(jNode, "x-providerName",o.x_providerName  );
    util::extractFromJsonTo(jNode, "x-serviceName", o.x_serviceName   );
    util::extractFromJsonTo(jNode, "x-release"    , o.x_release       );
    util::extractFromJsonTo(jNode, "x-preferred"  , o.x_preferred     );
    util::extractFromJsonTo(jNode, "x-tags"       , o. x_tags         );
    util::extractFromJsonTo(jNode, "x-unofficialSpec", o. x_unofficialSpec);
    util::extractFromJsonTo(jNode, "x-origin"     , o.x_origin        );
    util::extractFromJsonTo(jNode, "x-apisguru-categories", o.x_apisguru_categories );
    util::extractFromJsonTo(jNode, "x-logo"       , o.x_logo          );
    util::extractFromJsonTo(jNode, "x-apiClientRegistration", o.x_apiClientRegistration );
    util::extractFromJsonTo(jNode, "x-description-language", o.x_description_language );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#serverVariableObject
struct ServerVariableObject
{
    Opt< std::vector<json_scalar> >   _enum   ; // REQUIRED, NOT_EMPTY
    Opt< json_scalar >                _default   ; // REQUIRED
    Opt< std::string    >              description   ;

}; // struct ServerVariableObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const ServerVariableObject& o)
{
    util::convertToJsonFrom(jNode, "enum"       , o._enum    );
    util::convertToJsonFrom(jNode, "default"    , o._default );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description );
}

inline
void from_json( const nlohmann::json& jNode, ServerVariableObject& o)
{
    util::extractFromJsonTo(jNode, "enum"         , o._enum    );
    util::extractFromJsonTo(jNode, "default"      , o._default );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#discriminator-object
struct DiscriminatorObject
{
    Opt< std::string >                                 propertyName ;
    Opt< std::map<std::string,std::string> >           mapping      ;

}; // struct DiscriminatorObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const DiscriminatorObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, propertyName );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, mapping      );
}

inline
void from_json( const nlohmann::json& jNode, DiscriminatorObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, propertyName );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, mapping      );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#xml-object
struct XmlObject
{
    Opt< std::string >                                 name         ;
    Opt< std::string >                                 _namespace   ;
    Opt< std::string >                                 prefix       ;
    Opt< std::string >                                 attribute    ;
    Opt< std::string >                                 wrapped      ;

}; // struct XmlObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const XmlObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, name         );
    util::convertToJsonFrom(jNode, "namespace"  , o._namespace    );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, prefix       );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, attribute    );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, wrapped      );
}

inline
void from_json( const nlohmann::json& jNode, XmlObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, name         );
    util::extractFromJsonTo(jNode, "namespace"    , o._namespace    );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, prefix       );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, attribute    );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, wrapped      );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#reference-object
struct ReferenceObject
{
    Opt< std::string    >                              _ref         ; // REQUIRED
    Opt< std::string    >                              summary      ;
    Opt< std::string    >                              description  ;

}; // struct ReferenceObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const ReferenceObject& o)
{
    util::convertToJsonFrom(jNode, "$ref"       , o._ref         );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, summary     );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description );
}

inline
void from_json( const nlohmann::json& jNode, ReferenceObject& o)
{
    util::extractFromJsonTo(jNode, "$ref"         , o._ref         );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, summary     );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description );
}
 
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
struct SchemaObject;
struct SchemaOrReferenceObject;
struct PropertyObject;
void to_json( nlohmann::json& jNode, const PropertyObject& o);
void from_json( const nlohmann::json& jNode, PropertyObject& o);

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
struct PropertyItemObject
{
    Opt< std::string >                                 _ref         ;
    Opt< std::string >                                 type, format, description, summary;
    Opt< std::vector<json_scalar> >                    _enum        ;
    Opt< json_scalar    >                              _default     ;
    Opt< std::string >                                 example      ; // !!! В новых доках - examples, Type: Any - хз как парсить, да оно и не надо
    Opt< std::map<std::string, PropertyObject> >       properties   ;
    Opt< int >                                         minLength, maxLength;
    Opt< json_number >                                 minimum, maximum;

}; // struct PropertyItemObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const PropertyItemObject& o)
{
    util::convertToJsonFrom(jNode, "$ref"       , o._ref          );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, type         );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, format       );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description  );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, summary      );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, example      );
    util::convertToJsonFrom(jNode, "enum"       , o._enum         );
    util::convertToJsonFrom(jNode, "default"    , o._default   );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, properties   );
}

inline
void from_json( const nlohmann::json& jNode, PropertyItemObject& o )
{
    util::extractFromJsonTo(jNode, "$ref"         , o._ref         );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, type        );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, format      );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, summary     );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, example     );
    util::extractFromJsonTo(jNode, "enum"         , o._enum        );
    util::extractFromJsonTo(jNode, "default"      , o._default   );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, properties  );
}
 
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
struct PropertyObject
{
    Opt< std::string    >                              _ref         ;
    Opt< std::string    >                              type, format, description, summary;
    Opt< std::string    >                              example      ;
    Opt< std::vector<json_scalar> >                    _enum        ;
    Opt< json_scalar    >                              _default     ;
    Opt< PropertyItemObject >                          items        ; // те же свойства, что и у PropertyObject, 
    // но структура не может сама себя включать по значению, поэтому ввёл доп тип. Данное поле описывает повторяющиеся одинаковые элементы
    Opt< std::map<std::string, PropertyObject> >       properties   ; // Описывает одиночные именованные свойства, которые, 
    // тем не менее, могут быть типа array (или нет? надо проверить, может ли быть type: array и $ref на какой-то тип? )
    Opt< int >                                         minLength, maxLength;
    Opt< json_number >                                 minimum, maximum;

    void fromPropertyItemObject(const PropertyItemObject &pi)
    {
        _ref           = pi._ref       ;
        type           = pi.type       ;
        format         = pi.format     ;
        description    = pi.description;
        summary        = pi.summary    ;
        example        = pi.example    ;
        _enum          = pi._enum      ;
        _default       = pi._default   ;
        //items          = pi.;
        properties     = pi.properties ;
        minLength      = pi.minLength  ;
        maxLength      = pi.maxLength  ;
        minimum        = pi.minimum    ;
        maximum        = pi.maximum    ;
    }

    static 
    PropertyObject makeFromPropertyItemObject(const PropertyItemObject &pi)
    {
        PropertyObject po;
        po.fromPropertyItemObject(pi);
        return po;
    }

}; // struct PropertyObject

//----------------------------------------------------------------------------
// struct PropertyObject
inline
void to_json( nlohmann::json& jNode, const PropertyObject& o)
{
    util::convertToJsonFrom(jNode, "$ref"       , o._ref          );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, type         );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, format       );
    util::convertToJsonFrom(jNode, "enum"       , o._enum         );
    util::convertToJsonFrom(jNode, "default"    , o._default   );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, items        ); // if type==array
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description  );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, summary      );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, example      );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, properties   );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, minLength    );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, maxLength    );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, minimum      );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, maximum      );
}

inline
void from_json( const nlohmann::json& jNode, PropertyObject& o)
{
    util::extractFromJsonTo(jNode, "$ref"         , o._ref          );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, type         );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, format       );
    util::extractFromJsonTo(jNode, "enum"         , o._enum         );
    util::extractFromJsonTo(jNode, "default"      , o._default   );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, items        );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description  );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, summary     );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, example      );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, properties   );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, minLength    );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, maxLength    );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, minimum      );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, maximum      );
}
 
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#schema-object
struct SchemaObject
{
    Opt< DiscriminatorObject >                         discriminator;
    Opt< XmlObject >                                   xml          ;
    Opt< ExternalDocumentationObject >                 externalDocs ;

    Opt< std::string >                                 title        ;
    Opt< std::string >                                 type         ;
    Opt< std::string >                                 format       ;
    Opt< PropertyItemObject >                          items        ;
    Opt< int >                                         minItems     ;
    Opt< int >                                         maxItems     ;

    //Opt< std::string >                                 _default     ;
    Opt< json_scalar >                                 _default     ;
    Opt< std::string >                                 pattern      ;
    Opt< std::string >                                 x_pattern    ;
    Opt< int >                                         minLength    ;
    Opt< int >                                         maxLength    ;
    
    Opt< std::string >                                 description  ;

    Opt< std::vector<std::string> >                    required     ; // list of required fields

    Opt< std::vector<json_scalar> >                    _enum        ;

    Opt< std::map<std::string, PropertyObject> >       properties   ;

    Opt< ReferenceObject >                             additionalProperties;

    Opt< std::string >                                 example      ; // !!! В новых доках - examples, Type: Any - хз как парсить, да оно и не надо
//Opt< std::map<std::string, SchemaObject> >                  schemas      ;

    Opt< json_number >                                 minimum;
    Opt< json_number >                                 maximum;


}; // struct SchemaObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const SchemaObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, discriminator );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, xml           );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, externalDocs  );

    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, title         );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, type          );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, format        );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, items         ); // if type==array
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, minItems      ); // if type==array
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, maxItems      ); // if type==array
    util::convertToJsonFrom(jNode, "default"       , o._default    );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, pattern       );
    util::convertToJsonFrom(jNode, "x-pattern"  , o. x_pattern     );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, minLength     );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, maxLength     );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description   );

    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, required      );
    util::convertToJsonFrom(jNode, "enum"       , o._enum          );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, properties    );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, additionalProperties);

    //!!!
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, example       );

    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, minimum       );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, maximum       );

}

inline
void from_json( const nlohmann::json& jNode, SchemaObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, discriminator );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, xml           );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, externalDocs  );
    // MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, example       );
    // if (!o.examples) // элемента 'examples' нет, возможно, что есть 'example'
    // {
    //     if (jNode.contains("example"))
    //         util::extractFromJsonTo( jNode, "example", o.examples );
    // }
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, title         );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, type          );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, format        );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, items         );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, minItems      );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, maxItems      );
    util::extractFromJsonTo(jNode, "default"         , o._default    );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, pattern       );
    util::extractFromJsonTo(jNode, "x-pattern"    , o. x_pattern     );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, minLength     );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, maxLength     );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description   );

    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, required      );
    util::extractFromJsonTo(jNode, "enum"         , o._enum          );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, properties    );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, additionalProperties);

    //!!!
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, example       );

    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, minimum       );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, maximum       );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
struct SchemaOrReferenceObject
{
    Opt< ReferenceObject    >                          reference ;
    Opt< SchemaObject       >                          schema    ;

}; // struct SchemaOrReferenceObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const SchemaOrReferenceObject& o)
{
    if (o.reference)
        to_json(jNode, *o.reference);
    else if (o.schema)
        to_json(jNode, *o.schema);
}

inline
void from_json( const nlohmann::json& jNode, SchemaOrReferenceObject& o)
{
    o.reference = std::nullopt; // {}
    o.schema    = std::nullopt; // {}

    if (jNode.contains("$ref"))
    {
        ReferenceObject tmp;
        from_json(jNode, tmp);
        o.reference = tmp;
    }
    else if (jNode.contains("type"))
    {
        SchemaObject tmp;
        from_json(jNode, tmp);
        o.schema = tmp;
    }
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#parameter-object
struct ParameterObject
{
    Opt< std::string    >                              name         ; // REQUIRED
    Opt< std::string    >                              in           ; // REQUIRED. The location of the parameter
    Opt< std::string    >                              description  ;
    Opt< std::string    >                              style        ; // ХЗ чо
    Opt< bool           >                              required     ; // If the parameter location is "path", this property is REQUIRED and its value MUST be true. Otherwise, the property MAY be included and its default value is false.
    Opt< bool           >                              deprecated   ;
    Opt< bool           >                              explode      ; // ХЗ чо
    Opt< bool           >                              allowEmptyValue;
    Opt< SchemaOrReferenceObject >                     schema       ;
    

}; // struct ParameterObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const ParameterObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, name        );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, in          );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, style       );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, required    );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, deprecated  );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, explode     );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, allowEmptyValue );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, schema      );
}

inline
void from_json( const nlohmann::json& jNode, ParameterObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, name        );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, in          );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, style       );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, required    );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, deprecated  );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, explode     );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, allowEmptyValue );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, schema      );
}
 
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
/*
    // https://spec.openapis.org/oas/v3.1.0#parameter-object
    name            - REQUIRED

    // https://spec.openapis.org/oas/v3.1.0#reference-object

    $ref         - REQUIRED

 */

struct ParameterOrReferenceObject
{
    Opt< ReferenceObject    >                          reference ;
    Opt< ParameterObject    >                          parameter ;

}; // struct ParameterOrReferenceObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const ParameterOrReferenceObject& o)
{
    if (o.reference)
        to_json(jNode, *o.reference);
    else if (o.parameter)
        to_json(jNode, *o.parameter);
}

inline
void from_json( const nlohmann::json& jNode, ParameterOrReferenceObject& o)
{
    o.reference = std::nullopt; // {}
    o.parameter = std::nullopt; // {}

    if (jNode.contains("$ref"))
    {
        ReferenceObject tmp;
        from_json(jNode, tmp);
        o.reference = tmp;
    }
    else if (jNode.contains("name"))
    {
        ParameterObject tmp;
        from_json(jNode, tmp);
        o.parameter = tmp;
    }
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#server-object
struct ServerObject
{
    Opt< std::string    >                              url          ; // REQUIRED
    Opt< std::string    >                              description  ;
    Opt< std::map<std::string, ServerVariableObject> > variables    ;

}; // struct ServerObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const ServerObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, url         );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, variables   );
}

inline
void from_json( const nlohmann::json& jNode, ServerObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, url         );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, variables   );
}
 
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#media-type-object
struct MediaTypeObject
{
    Opt< SchemaOrReferenceObject >                     schema       ;
    
    // example
    // examples
    // encoding


}; // struct MediaTypeObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const MediaTypeObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, schema      );
}

inline
void from_json( const nlohmann::json& jNode, MediaTypeObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, schema      );
}
 
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#response-object
struct ResponseObject
{
    Opt< std::string    >                              description  ;
    Opt< std::map<std::string, MediaTypeObject > >     content      ;

}; // struct ResponseObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const ResponseObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, content     );
}

inline
void from_json( const nlohmann::json& jNode, ResponseObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, content     );
}
 
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
struct ResponseOrReferenceObject
{
    Opt< ReferenceObject    >                          reference  ;
    Opt< ResponseObject  >                             response   ;

}; // struct ResponseOrReferenceObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const ResponseOrReferenceObject& o)
{
    if (o.reference)
        to_json(jNode, *o.reference);
    else if (o.response)
        to_json(jNode, *o.response);
}

inline
void from_json( const nlohmann::json& jNode, ResponseOrReferenceObject& o)
{
    o.reference   = std::nullopt; // {}
    o.response    = std::nullopt; // {}

    if (jNode.contains("$ref"))
    {
        ReferenceObject tmp;
        from_json(jNode, tmp);
        o.reference = tmp;
    }
    else if (jNode.contains("content") || jNode.contains("description"))
    {
        ResponseObject tmp;
        from_json(jNode, tmp);
        o.response = tmp;
    }
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
#if 0
// https://spec.openapis.org/oas/v3.1.0#responses-object
struct ResponsesObject
{
    Opt< std::string    >                              description  ;
    Opt< std::string    >                              url          ; // REQUIRED

}; // struct ResponsesObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const ResponsesObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, url         );
}

inline
void from_json( const nlohmann::json& jNode, ResponsesObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, url         );
}
#endif 
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#security-requirement-object
struct SecurityRequirementObject
{
    //Opt< std::map<std::string, std::string > >         content      ;

}; // struct SecurityRequirementObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const SecurityRequirementObject& o)
{
    // MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description );
    // MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, content     );
}

inline
void from_json( const nlohmann::json& jNode, SecurityRequirementObject& o)
{
    // MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description );
    // MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, content     );
}
 
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#request-body-object
struct RequestBodyObject
{
    Opt< std::string    >                              description  ;
    Opt< std::map<std::string, MediaTypeObject > >     content      ; // REQUIRED
    Opt< bool           >                              required     ;

}; // struct RequestBodyObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const RequestBodyObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, content     );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, required    );
}

inline
void from_json( const nlohmann::json& jNode, RequestBodyObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, content     );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, required    );
}
 
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
struct RequestBodyOrReferenceObject
{
    Opt< ReferenceObject    >                          reference  ;
    Opt< RequestBodyObject  >                          requestBody;

}; // struct RequestBodyOrReferenceObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const RequestBodyOrReferenceObject& o)
{
    if (o.reference)
        to_json(jNode, *o.reference);
    else if (o.requestBody)
        to_json(jNode, *o.requestBody);
}

inline
void from_json( const nlohmann::json& jNode, RequestBodyOrReferenceObject& o)
{
    o.reference   = std::nullopt; // {}
    o.requestBody = std::nullopt; // {}

    if (jNode.contains("$ref"))
    {
        ReferenceObject tmp;
        from_json(jNode, tmp);
        o.reference = tmp;
    }
    else if (jNode.contains("content"))
    {
        RequestBodyObject tmp;
        from_json(jNode, tmp);
        o.requestBody = tmp;
    }
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#operation-object
struct OperationObject
{
    Opt< std::vector<std::string> >                    tags         ;
    Opt< std::vector<std::string> >                    x_tags       ;
    Opt< std::string    >                              summary      ;
    Opt< std::string    >                              description  ;
    Opt< ExternalDocumentationObject >                 externalDocs ;
    Opt< std::string >                                 operationId  ;
    Opt< std::vector<ParameterOrReferenceObject> >     parameters   ;
    Opt< std::map<std::string, ResponseOrReferenceObject > > responses; //!!!
    //Opt< std::vector<SecurityRequirementObject> >      security     ;
    Opt< std::vector< std::map<std::string, std::vector<std::string> > > >      security     ;
    Opt< RequestBodyOrReferenceObject        >         requestBody  ;


    //Opt< std::map<std::string, std::vector<std::string> > >         content      ;

    // SecurityRequirementObject

    //TODO: 'callbacks'
    //TODO: 'deprecated'
    //TODO: 'security'
    //TODO: 'servers'

    

}; // struct OperationObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const OperationObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, tags        );
    util::convertToJsonFrom(jNode, "x-tags"     , o .x_tags      );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, summary     );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, externalDocs);
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, operationId );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, parameters  );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, responses   );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, security    );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, requestBody );
}

inline
void from_json( const nlohmann::json& jNode, OperationObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, tags        );
    util::extractFromJsonTo(jNode, "x-tags"       , o. x_tags      );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, summary     );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, externalDocs);
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, operationId );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, parameters  );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, responses   );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, security    );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, requestBody );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#path-item-object
struct PathItemObject
{
    Opt< std::string    >                              _ref         ; // REQUIRED
    Opt< std::string    >                              summary      ;
    Opt< std::string    >                              description  ;
    Opt< OperationObject>                              get          ;
    Opt< OperationObject>                              put          ;
    Opt< OperationObject>                              post         ;
    Opt< OperationObject>                              _delete      ;
    Opt< OperationObject>                              options      ;
    Opt< OperationObject>                              head         ;
    Opt< OperationObject>                              patch        ;
    Opt< OperationObject>                              trace        ;
    Opt< std::vector<ServerObject> >                   servers      ;
    Opt< std::vector<ParameterOrReferenceObject> >     parameters   ;

}; // struct PathItemObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const PathItemObject& o)
{
    util::convertToJsonFrom(jNode, "$ref"       , o._ref         );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, summary     );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, get         );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, put         );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, post        );
    util::convertToJsonFrom(jNode, "delete"     , o._delete      );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, options     );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, head        );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, patch       );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, trace       );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, servers     );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, parameters  );
}

inline
void from_json( const nlohmann::json& jNode, PathItemObject& o)
{
    util::extractFromJsonTo(jNode, "$ref"         , o._ref         );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, summary     );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, get         );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, put         );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, post        );
    util::extractFromJsonTo(jNode, "delete"       , o._delete      );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, options     );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, head        );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, patch       );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, trace       );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, servers     );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, parameters  );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#components-object
struct ComponentsObject
{
    Opt< std::map<std::string, SchemaObject> >                  schemas        ;
    Opt< std::map<std::string, ResponseObject> >                responses      ;
    Opt< std::map<std::string, ParameterOrReferenceObject> >    parameters     ;
    Opt< std::map<std::string, RequestBodyOrReferenceObject> >  requestBodies  ;
    Opt< std::map<std::string, SecuritySchemeObject > >         securitySchemes;

}; // struct ComponentsObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const ComponentsObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, schemas         );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, responses       );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, parameters      );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, requestBodies   );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, securitySchemes );
}

inline
void from_json( const nlohmann::json& jNode, ComponentsObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, schemas         );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, responses       );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, parameters      );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, requestBodies   );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, securitySchemes );
}

//----------------------------------------------------------------------------



#if 0
//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#components-object
struct SecurityRequirementObject
{
    Opt< std::map<std::string, std::vector<std::string> > >       schemas        ;

}; // struct SecurityRequirementObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const SecurityRequirementObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, schemas         );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, responses       );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, parameters      );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, requestBodies   );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, securitySchemes );
}

inline
void from_json( const nlohmann::json& jNode, SecurityRequirementObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, schemas         );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, responses       );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, parameters      );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, requestBodies   );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, securitySchemes );
}

//----------------------------------------------------------------------------
#endif



//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#openapi-object
struct OpenApiSpecObject
{
    Opt< std::string    >                           openapi           ; // REQUIRED
    Opt< InfoObject     >                           info              ; // REQUIRED
    Opt< std::string    >                           jsonSchemaDialect ;
    Opt< std::vector<ServerObject> >                servers           ;
    Opt< std::map<std::string, PathItemObject> >    paths             ; // paths field - в доке описано не как мапа
    //TODO: !!! Add 'webhooks' field
    Opt< ComponentsObject >                         components        ;
    
    // https://spec.openapis.org/oas/v3.1.0#security-requirement-object
    // security:
    //   - apikey: []
    // security:
    //   - hmac: []
    Opt< std::vector< std::map<std::string, std::vector<std::string> > > > security ;

    Opt< std::vector<TagObject> >                   tags              ;

    Opt< ExternalDocumentationObject >              externalDocs      ;

    Opt< bool >                                     x_hasEquivalentPaths;

}; // struct OpenApiSpecObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const OpenApiSpecObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, openapi      );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, info         );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, jsonSchemaDialect );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, servers      );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, paths        );

    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, components   );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, security     );

    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, tags         );

    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, externalDocs );

    util::convertToJsonFrom(jNode, "x-hasEquivalentPaths", o.x_hasEquivalentPaths );
}

inline
void from_json( const nlohmann::json& jNode, OpenApiSpecObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, openapi      );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, info         );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, jsonSchemaDialect );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, servers      );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, paths        );

    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, components   );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, security     );

    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, tags         );

    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, externalDocs );

    util::extractFromJsonTo(jNode, "x-hasEquivalentPaths", o.x_hasEquivalentPaths );
}

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#openapi-object
struct OpenApiSpecDetectStubObject
{
    Opt< std::string    >                           openapi           ; // REQUIRED

}; // struct OpenApiSpecDetectStubObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const OpenApiSpecDetectStubObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, openapi      );
}

inline
void from_json( const nlohmann::json& jNode, OpenApiSpecDetectStubObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, openapi      );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// marty::swagger::util::
namespace util
{


//------------------------------
inline
std::string unquoteSimpleQuoted( const std::string &str )
{
    if (str.size()<2)
        return str;

    const typename std::string::value_type quotChar = str.front();

    if (str.front()!=quotChar || str.back()!=quotChar)
        return str;

    std::string::size_type pos = 1, endPos = str.size()-1;

    std::string res; res.reserve(endPos-pos);

    bool prevQuot = false;

    for( ; pos!=endPos; ++pos )
    {
        auto ch = str[pos];

        if (prevQuot)
        {
            if (ch==quotChar)
            {
                res.append(1,quotChar); // remove diplicated quots
            }
            else
            {
                res.append(1,quotChar);
                res.append(1,ch); // не знаю, что это, просто игнорим
            }
            prevQuot = false;
        }
        else
        {
            res.append(1,ch);
        }
    }

    if (prevQuot)
    {
        res.append(1,quotChar);
    }

    return res;
}

//------------------------------
inline
std::string removeLeadingHashCopy( const std::string &str )
{
    if (str.empty() || str[0]!='#')
        return str;

    return std::string(str, 1);
}

//------------------------------
inline
std::string getRefPrefix( const std::string &ref, std::string::size_type pos = std::string::npos )
{
    if (pos==std::string::npos)
        pos = ref.rfind('/');
    return removeLeadingHashCopy(std::string(ref, 0, pos));
}

//------------------------------
//! Возвращает значение ссылки (последний элемент пути, разделённого символами '/')
/*!
    ref             : '#/components/schemas/InstrumentType'
    *pRefPrefixPath : '/components/schemas' ('#' at front removed if present)
    returns         : 'InstrumentType'

 */
inline
std::string getRefValue( const std::string &ref, std::string *pRefPrefixPath = 0 )
{
    std::string::size_type pos = ref.rfind('/');

    if (pRefPrefixPath)
    {
        *pRefPrefixPath = getRefPrefix(ref, pos);
    }

    if (pos==ref.npos)
    {
        return std::string();
    }

    return std::string(ref, pos+1);

}

//-----------------------------------------------------------------------------
inline
std::string getFirstPathItem( const std::string &path )
{
    std::string::size_type pos = path.find('/');

    if (pos==path.npos)
    {
        return std::string();
    }

    return std::string(path, 0, pos);

}

//-----------------------------------------------------------------------------
//! Возвращает true, если строка начинается с указанного префикса (StringType)
/*! \tparam StringType Тип входной и результирующей строки (std::basic_string)
    \param str         Входная строка
    \param prefix      Префикс, есть перегрузка для const StringType::value_type*
    \returns           true, если строка начинается с указанного префикса
 */
template <typename StringType> inline bool startsWith( const StringType &str, const StringType &prefix )
{
    if (str.size()<prefix.size())
        return false;

    return str.compare( 0, prefix.size(), prefix )==0;
}

//-----------------------------------------------------------------------------
//! Возвращает true, если строка начинается с указанного префикса (StringType::value_type*)
/*! \copydetails starts_with */
template <typename StringType> inline bool startsWith( const StringType &str, const typename StringType::value_type *prefix )
{
    return startsWith(str, StringType(prefix));
}

//-----------------------------------------------------------------------------
//! Возвращает true, если строка начинается с указанного префикса (StringType), удаляя префикс
/*! \copydetails starts_with */
template <typename StringType> inline bool startsWithAndStrip( StringType &str, const StringType &prefix )
{
    if (!startsWith( str, prefix ))
        return false;

    str.erase( 0, prefix.size() );

    return true;
}

//-----------------------------------------------------------------------------
//! Возвращает true, если строка начинается с указанного префикса (StringType::value_type*), удаляя префикс
/*! \copydetails starts_with */
template <typename StringType> inline bool startsWithAndStrip( StringType &str, const typename StringType::value_type *prefix )
{
    return startsWithAndStrip(str, StringType(prefix));
}

//-----------------------------------------------------------------------------
inline
nlohmann::json_pointer<nlohmann::json> makeJsonPointer(const std::string &path)
{
    if (!path.empty() && path[0]!='/')
        return nlohmann::json_pointer<nlohmann::json>(std::string("/")+path);
    return nlohmann::json_pointer<nlohmann::json>(path);
}

inline
nlohmann::json_pointer<nlohmann::json> makeJsonPointer(const char* path)
{
    if (!path)
        //return nlohmann::json_pointer<json>();
        makeJsonPointer(std::string());

    return makeJsonPointer(std::string(path));
}



} // namespace util
// marty::swagger::util::

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
#if 0
    /components/schemas/UserAccountsResponse/properties/payload/$ref - '#/components/schemas/UserAccounts'
    /paths/~1sandbox~1currencies~1balance/post/requestBody/content/application~1json/schema/$ref - '#/components/schemas/SandboxSetCurrencyBalanceRequest'
#endif

template < typename ComponentsAllSetType
         , typename ComponentsUsedSetType
         , typename DependencyFinderType
         >
void findComponents(  /* const */ nlohmann::json  &apiSpecJson
                   , DependencyFinderType         &dependencyFinder
                   , ComponentsAllSetType         *pComponentsAllSet
                   , ComponentsUsedSetType        *pComponentsUsedSet
                   )
{
    std::vector<std::string> refMatchedPaths;

    std::string refRegexStr = umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/*/$ref^", true );

    marty::json_utils::findPathMatches( apiSpecJson, refMatchedPaths
                                      , std::regex( refRegexStr ) // use anchors
                                      );

    for( auto matchedPath : refMatchedPaths )
    {
        auto refValFull = marty::json_utils::getScalarStr(apiSpecJson[util::makeJsonPointer(matchedPath)]);
             refValFull =  /* marty::swagger:: */ util::unquoteSimpleQuoted(refValFull); // umba::string_plus::unquoteSimpleQuoted - тоже самое; 

        auto refVal     =  /* marty::swagger:: */ util::getRefValue(refValFull);
        auto refPrefix  =  /* marty::swagger:: */ util::getRefPrefix(refValFull);

        if (refPrefix!="/components/schemas") // куда-то ссылается, но хз куда
             continue;

        auto componentName = matchedPath;
        if ( /* marty::swagger:: */ util::startsWithAndStrip(componentName,"/components/schemas/"))
        {
            // Тут мы нашли ссылку на "/components/schemas" и ссылается тоже откуда-то из "/components/schemas"
            // Это найдена зависимость в описаниях компонентов
            componentName = marty::swagger::util::getFirstPathItem(componentName);
            dependencyFinder.addDependency(componentName, refVal);
            if (pComponentsAllSet)
                pComponentsAllSet->insert(componentName);
        }
        else if ( /* marty::swagger:: */ util::startsWithAndStrip(componentName,"/paths/"))
        {
            // Тут кто-то ссылается из раздела "/paths/"
            // Это реально используемые в API компоненты
            if (pComponentsUsedSet)
                pComponentsUsedSet->insert(refVal);
        }

    }


    // Остались "/components/schemas", которые никуда на других не ссылаются
    // Ищем их. Они нужны, чтобы выцепить вообще всё типы из спеки

    std::vector<std::string> matchedComponentPaths;

    std::string componentsRegexStr = umba::regex_helpers::expandSimpleMaskToEcmaRegex( "^/components/schemas/*^", true );

    marty::json_utils::findPathMatches( apiSpecJson, matchedComponentPaths
                                      , std::regex( componentsRegexStr ) // use anchors
                                      );

    // lout << "### Found component paths:\n";
    for( const auto &mcp : matchedComponentPaths )
    {
        // lout << mcp << "\n";
        auto componentName = mcp;
        if ( /* marty::swagger:: */ util::startsWithAndStrip(componentName,"/components/schemas/"))
        {
            componentName =  /* marty::swagger:: */ util::getFirstPathItem(componentName);
            if (!componentName.empty())
            {
            if (pComponentsAllSet)
                pComponentsAllSet->insert(componentName);
            }
        }
    }

}






//----------------------------------------------------------------------------

} // namespace swagger
} // namespace marty

// marty::swagger::


