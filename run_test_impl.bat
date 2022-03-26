@set TEST_NAME=%1
@set SELF_PATH=%~dp0
@rem  set OLDDIR=%CD%
@setlocal
@rem cd %~dp0\.out
@pushd %~dp0\.out
@rem echo Args (in impl): %*
@set ARGS=
:ARGS_LOOP
@shift
@if "%1"=="" goto ARGS_END
@set ARGS=%ARGS% %1
@rem echo args (in loop) %ARGS%
@goto ARGS_LOOP
:ARGS_END
@rem echo args (before call) %ARGS%
@%SELF_PATH%\.out\msvc2019\x64\Debug\%TEST_NAME% %ARGS%
@rem cd %OLDDIR%
@endlocal
@popd