@set SELF_PATH=%~dp0

@rem for %%i in (%SELF_PATH%\test017_*.txt) do @call ..\run_test.bat test017 %%~nxi %SELF_PATH%\swagger-example-tinkoff-openapi.yaml >%%~nxi.log

@for %%i in (%SELF_PATH%\test017_011.txt) do @call ..\run_test.bat test017 %%~nxi %SELF_PATH%\swagger-example-tinkoff-openapi.yaml >%%~nxi.log

