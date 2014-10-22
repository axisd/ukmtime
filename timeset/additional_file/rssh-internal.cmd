@echo off
echo ===================================
echo  %0 started %DATE% %TIME%
echo ===================================


set script=timeset.sh
set remote_script_dir=/tmp
set remote_script=%remote_script_dir%/%script%

SET FILE_IPLIST="iplist.txt"


echounix -n "Checking file %FILE_IPLIST% ... "
if exist %FILE_IPLIST% (
	echounix "found"
) else (
	echounix "not found"
	goto error
)

echounix
echounix
echounix "============================================================"
echounix "1st stage - checking hosts state - started"
setlocal enabledelayedexpansion
for /F %%i in (iplist.txt) do (
	echounix -n "%%i ... "
	plink -batch -pw xxxxxx root@%%i echo -n
	IF !errorlevel! equ 0 (
		echounix "online"
	) else (
		echounix "%%i is offline"
	)
)
endlocal
echounix "1st stage - checking hosts state - completed"
echounix "============================================================"
echounix
echounix

echounix "============================================================"
echounix "2st stage - setting time - started"
setlocal enabledelayedexpansion
for /F %%i in (iplist.txt) do (
	call rssh-internal-1host.cmd %%i
	IF !ERRORLEVEL! neq 0 goto error
)
endlocal
echounix "2st stage - setting time - completed"
echounix "============================================================"


echo ===================================
echo  SUCCESS: %0 completed %DATE% %TIME%
echo ===================================
exit /B 0
GOTO end

:error
echo ===================================
echo  ERROR:   %0 failed %DATE% %TIME%
echo ===================================
exit /B 1
:end
