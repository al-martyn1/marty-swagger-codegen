@set TEST_NAME=%1
@set SELF_PATH=%~dp0
@rem echo Args (before shift): %*
@rem shift /1
@rem echo Args (input): %*
@rem make ARGS
@rem 
@set ARGS=
:ARGS_LOOP
@shift
@if "%1"=="" goto ARGS_END
@set ARGS=%ARGS% %1
@rem echo args (in loop) %ARGS%
@goto ARGS_LOOP
:ARGS_END
@rem echo args (before call) %ARGS%
@call %SELF_PATH%\run_test_impl.bat %TEST_NAME% %ARGS%