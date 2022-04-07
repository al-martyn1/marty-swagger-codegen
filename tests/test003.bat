@call ..\run_test.bat test003 swagger-example-tinkoff-openapi.yaml     %* >test003_swagger-example-tinkoff-openapi-dif.log  2>&1
@call ..\run_test.bat test003 amadeus_com_openapi.yaml                 %* >test003_amadeus_com_openapi-dif.log              2>&1
@call ..\run_test.bat test003 apacta_com_openapi.yaml                  %* >test003_apacta_com_openapi-dif.log               2>&1
@call ..\run_test.bat test003 amazonaws_acm_openapi.yaml               %* >test003_amazonaws_acm_openapi-dif.log            2>&1
@call ..\run_test.bat test003 amazonaws_athena_openapi.yaml            %* >test003_amazonaws_athena_openapi-dif.log         2>&1

