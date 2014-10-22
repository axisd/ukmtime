@echo off

echo Setting new time ...
echo Check rssh.log for detailes
call rssh-internal.cmd >> rssh.log 2>&1
IF %ERRORLEVEL% NEQ 0 GOTO error

echo SUCCESS: time is set
pause
exit 0

:error
echo ERROR: time is not set
pause
exit 1
