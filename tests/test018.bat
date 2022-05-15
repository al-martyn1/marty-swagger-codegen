@set SELF_PATH=%~dp0
@set SWAGGER_FILE=%SELF_PATH%\swagger-example-tinkoff-openapi.yaml

@rem echo SELF_PATH=%SELF_PATH%
@rem echo SWAGGER_FILE=%SWAGGER_FILE%

@rem for %%i in (%SELF_PATH%\test018_*.txt) do @call ..\run_test.bat test018 %%~nxi %SWAGGER_FILE% >%%~nxi.log

@for %%i in (%SELF_PATH%\test018_003.txt) do @call ..\run_test.bat test018 %%~nxi %SWAGGER_FILE% >%%~nxi.log
@for %%i in (%SELF_PATH%\test018_004.txt) do @call ..\run_test.bat test018 %%~nxi %SWAGGER_FILE% >%%~nxi.h

