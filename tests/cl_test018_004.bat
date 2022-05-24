@set SELF_PATH=%~dp0
@set SWAGGER_FILE=%SELF_PATH%\swagger-example-tinkoff-openapi.yaml

@rem echo SELF_PATH=%SELF_PATH%
@rem echo SWAGGER_FILE=%SWAGGER_FILE%

@for %%i in (%SELF_PATH%\test018_004.txt) do @call ..\run_test.bat test018 %%~nxi %SWAGGER_FILE% >%%~nxi.h

@call cxx17 /I..\_libs test018_004_main.cpp