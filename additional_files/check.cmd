@echo off

set host=%1

echounix
echounix
echounix " ************************************ "
echounix "	==== CHECK.CMD ==== "

echounix "Check %host%"
echounix "yes" | plink -pw xxxxxx root@%host% echo "ok"
IF %ERRORLEVEL% NEQ 0 GOTO error
echounix
echounix "Check SUCCESS"

exit /B 0
GOTO end

:error
echounix "	====   FAIL   ==== "
echounix " ************************************ "
echounix
echounix

exit /B 1
:end