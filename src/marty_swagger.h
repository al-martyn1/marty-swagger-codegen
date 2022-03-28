#pragma once


#include "yaml_utils.h"
#include "json_utils.h"

#include <optional>
#include <string>


// marty::swagger::

namespace marty{
namespace swagger
{



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
namespace util
{

template<typename T> inline
void extractFromJsonTo( const nlohmann::json& jNode, const std::string &name, Opt<T> &extractTo )
{
    if (jNode.contains(name))
        extractTo = jNode[name].get<T>(); // std::make_optional(3.0);
    else
        extractTo = {}; // std::nullopt
}

template<typename T> inline
void convertToJsonFrom( nlohmann::json& jNode, const std::string &name, const Opt<T> &convertFrom )
{
    if (convertFrom)
        jNode[name] = *convertFrom;
}

} // namespace util

//----------------------------------------------------------------------------

#define    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, member )    marty::swagger::util::extractFromJsonTo(jNode, #member, o.member)
#define    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, member )      marty::swagger::util::convertToJsonFrom(jNode, #member, o.member )

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
// https://spec.openapis.org/oas/v3.1.0#infoObject
struct InfoObject
{
    Opt< std::string    >   title             ; // REQUIRED
    Opt< std::string    >   summary           ;
    Opt< std::string    >   description       ;
    Opt< std::string    >   termsOfService    ;
    Opt< ContactObject  >   contact           ;
    Opt< LicenseObject  >   license           ;
    Opt< std::string    >   version           ; // REQUIRED

}; // struct InfoObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const InfoObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, title          );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, summary        );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description    );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, termsOfService );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, contact        );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, license        );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, version        );
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
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// https://spec.openapis.org/oas/v3.1.0#serverVariableObject
struct ServerVariableObject
{
    Opt< std::vector<std::string> >   _enum   ; // REQUIRED, NOT_EMPTY
    Opt< std::string    >          _default   ; // REQUIRED
    Opt< std::string    >       description   ;

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
// https://spec.openapis.org/oas/v3.1.0#parameter-object
struct ParameterObject
{
    Opt< std::string    >                              name         ; // REQUIRED
    Opt< std::string    >                              in           ; // REQUIRED. The location of the parameter
    Opt< std::string    >                              description  ;
    Opt< bool           >                              required     ; // If the parameter location is "path", this property is REQUIRED and its value MUST be true. Otherwise, the property MAY be included and its default value is false.
    Opt< bool           >                              deprecated   ;
    Opt< bool           >                              allowEmptyValue;

}; // struct ParameterObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const ParameterObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, name        );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, in          );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, required    );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, deprecated  );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, allowEmptyValue );
}

inline
void from_json( const nlohmann::json& jNode, ParameterObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, name        );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, in          );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, required    );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, deprecated  );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, allowEmptyValue );
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
/*
    // https://spec.openapis.org/oas/v3.1.0#parameter-object
    name            - REQUIRED

    // https://spec.openapis.org/oas/v3.1.0#reference-object

    $ref         - REQUIRED

 */

struct ParameterOrReferenceObject
{
    Opt< ParameterObject    >                          parameter ;
    Opt< ReferenceObject    >                          reference ;

}; // struct ParameterOrReferenceObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const ParameterOrReferenceObject& o)
{
    if (o.parameter)
        to_json(jNode, *o.parameter);
    else if (o.reference)
        to_json(jNode, *o.reference);
}

inline
void from_json( const nlohmann::json& jNode, ParameterOrReferenceObject& o)
{
    o.parameter = std::nullopt; // {}
    o.reference = std::nullopt;

    if (jNode.contains("name"))
    {
        ParameterObject tmp;
        from_json(jNode, tmp);
        o.parameter = tmp;
    }
    else if (jNode.contains("$ref"))
    {
        ReferenceObject tmp;
        from_json(jNode, tmp);
        o.reference = tmp;
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
// https://spec.openapis.org/oas/v3.1.0#operation-object
struct OperationObject
{
    Opt< std::vector<std::string> >                    tags         ;
    Opt< std::string    >                              summary      ;
    Opt< std::string    >                              description  ;
    Opt< ExternalDocumentationObject >                 externalDocs ;
    Opt< std::vector<std::string> >                    operationId  ;
    Opt< std::vector<ParameterOrReferenceObject> >     parameters   ;

    //TODO: 'requestBody'
    //TODO: 'responses'
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
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, summary     );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, externalDocs);
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, operationId );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, parameters  );
}

inline
void from_json( const nlohmann::json& jNode, OperationObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, tags        );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, summary     );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, externalDocs);
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, operationId );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, parameters  );
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
#if 0
// https://spec.openapis.org/oas/v3.1.0#paths-object
struct PathsObject
{
    Opt< std::string    >                              url          ; // REQUIRED
    Opt< std::string    >                              description  ;
    Opt< std::map<std::string, ServerVariableObject> > variables    ;

}; // struct PathsObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const PathsObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, url         );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, variables   );
}

inline
void from_json( const nlohmann::json& jNode, PathsObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, url         );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, variables   );
}
#endif
//----------------------------------------------------------------------------




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
    //TODO: !!! Add 'components' field
    //TODO: !!! Add 'security' field
    //TODO: !!! Add 'tags' field

    Opt< ExternalDocumentationObject >              externalDocs      ;

}; // struct OpenApiSpecObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const OpenApiSpecObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, openapi );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, info    );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, jsonSchemaDialect );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, servers );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, paths   );

    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, externalDocs );
}

inline
void from_json( const nlohmann::json& jNode, OpenApiSpecObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, openapi );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, info    );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, jsonSchemaDialect );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, servers );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, paths   );

    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, externalDocs );
}

//----------------------------------------------------------------------------





} // namespace swagger
} // namespace marty

// marty::swagger::


