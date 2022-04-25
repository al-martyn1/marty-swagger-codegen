@set SELF_PATH=%~dp0

@for %%i in (%SELF_PATH%\test017_*.txt) do @call ..\run_test.bat test017 %%~nxi %SELF_PATH%\swagger-example-tinkoff-openapi.yaml >%%~nxi.log

@rem call ..\run_test.bat test017 001 %* >test017_001.log

