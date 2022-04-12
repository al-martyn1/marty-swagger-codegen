@marty-json-diff test011_openapi.yaml test011_openapi_tilde.yaml test011_openapi.yaml.dif
@call ..\run_test.bat test011  swagger-example-tinkoff-openapi.yaml    %* >test011_swagger-example-tinkoff-openapi.log  2>&1
@call ..\run_test.bat test011  openapi.yaml                            %* >test011_openapi.log                          2>&1

