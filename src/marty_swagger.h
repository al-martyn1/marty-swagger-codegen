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
// https://spec.openapis.org/oas/v3.1.0#schema-object
struct SchemaObject
{
    Opt< DiscriminatorObject >                         discriminator;
    Opt< XmlObject >                                   xml          ;
    Opt< ExternalDocumentationObject >                 externalDocs ;

    Opt< std::string >                                 type         ;
    Opt< std::string >                                 format       ;
    Opt< std::string    >                              description  ;

    Opt< std::string >                                 example      ; // !!! В новых доках - examples, Type: Any - хз как парсить, да оно и не надо

}; // struct SchemaObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const SchemaObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, discriminator );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, xml           );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, externalDocs  );

    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, type          );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, format        );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, description   );

    //!!!
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, example       );
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
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, type          );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, format        );
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, description   );

    //!!!
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, example       );
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
    Opt< bool           >                              required     ; // If the parameter location is "path", this property is REQUIRED and its value MUST be true. Otherwise, the property MAY be included and its default value is false.
    Opt< bool           >                              deprecated   ;
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
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, required    );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, deprecated  );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, allowEmptyValue );
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, schema      );
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
    Opt< std::map<std::string, MediaTypeObject > >     content      ;
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
// https://spec.openapis.org/oas/v3.1.0#operation-object
struct OperationObject
{
    Opt< std::vector<std::string> >                    tags         ;
    Opt< std::string    >                              summary      ;
    Opt< std::string    >                              description  ;
    Opt< ExternalDocumentationObject >                 externalDocs ;
    Opt< std::vector<std::string> >                    operationId  ;
    Opt< std::vector<ParameterOrReferenceObject> >     parameters   ;
    Opt< std::map<std::string, ResponseObject > >      responses    ; //!!!
    //Opt< std::vector<SecurityRequirementObject> >      security     ;
    Opt< std::vector< std::map<std::string, std::vector<std::string> > > >      security     ;
    Opt< RequestBodyObject        >                    requestBody  ;


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
    Opt< std::map<std::string, SchemaObject> >         schemas      ;


    // Opt< std::string    >                              url          ;

}; // struct ComponentsObject

//------------------------------
inline
void to_json( nlohmann::json& jNode, const ComponentsObject& o)
{
    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, schemas     );
}

inline
void from_json( const nlohmann::json& jNode, ComponentsObject& o)
{
    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, schemas     );
}

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

    Opt< std::vector<TagObject> >                   tags              ;

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

    MARTY_SWAGGER_UTIL_PUT_MEMBER_TO_JSON( jNode, o, tags    );

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

    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, tags    );

    MARTY_SWAGGER_UTIL_GET_MEMBER_FROM_JSON( jNode, o, externalDocs );
}

//----------------------------------------------------------------------------





} // namespace swagger
} // namespace marty

// marty::swagger::


