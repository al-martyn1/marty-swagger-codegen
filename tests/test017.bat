@set SELF_PATH=%~dp0

@for %%i in (%SELF_PATH%\test017_*.txt) do @call ..\run_test.bat test017 %%~nxi >%%~nxi.log

@rem call ..\run_test.bat test017 001 %* >test017_001.log

