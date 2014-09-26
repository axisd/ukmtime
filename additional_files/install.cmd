@echo off

set host=%1
set remote_dir=/usr/local/ukmtimeup
set file_to_transfer=timezone_patch.zip
set remote_script=install.sh

echounix "Create dir %remote_dir% at %host%"
plink -batch -pw xxxxxx root@%host% mkdir %remote_dir%
IF %ERRORLEVEL% NEQ 0 GOTO error
echounix "Create dir success"

echounix "Copy file %file_to_transfer% to %host%"
pscp -batch -pw xxxxxx %file_to_transfer% root@%host%:%remote_dir%
IF %ERRORLEVEL% NEQ 0 GOTO error
echounix "Copy file success"

echounix "Unzip file %file_to_transfer% to %remote_dir% at %host%"
plink -batch -pw xxxxxx root@%host% unzip %remote_dir%/%file_to_transfer% -d %remote_dir%
IF %ERRORLEVEL% NEQ 0 GOTO error
echounix "Unzip file success"

echounix "chmod -v a+x to file %remote_script% at %host%"
plink -batch -pw xxxxxx root@%host% chmod -v a+x %remote_dir%/%remote_script%
IF %ERRORLEVEL% NEQ 0 GOTO error
echounix "chmod file success"

echounix "Execute file %remote_script% at %host%"
plink -batch -pw xxxxxx root@%host% %remote_dir%/%remote_script%
IF %ERRORLEVEL% NEQ 0 GOTO error
echounix "Execute file success"

exit /B 0
GOTO end

:error
echounix "#### FAIL ####"

exit /B 1
:end