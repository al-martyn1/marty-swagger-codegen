@rem SELF_PATH
@set SP=%~dp0

@if not exist %SP%\APIs_log mkdir %SP%\APIs_log
call ..\run_test.bat test009 %SP%\..\..\__3dp\APIs_guru_openapi_directory\APIs  %SP%\APIs_log      >test004.log 2>&1


