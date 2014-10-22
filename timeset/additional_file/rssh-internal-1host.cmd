@echo off

set host=%1

echounix
echounix
echounix
echounix "==== %DATE% %TIME% %host% started ==============================="
set script=timeset.sh
set func=pos_program_restart.sh
set datetime=datetime.sh
set remote_script_dir=/tmp
set remote_script=%remote_script_dir%/%script%

set remote_dir=%remote_script_dir%
echo Transferring %script% to %remote_dir% at %host%
pscp -batch -pw xxxxxx %script% root@%host%:%remote_dir%
IF %ERRORLEVEL% NEQ 0 GOTO error

echo Transferring %func% to %remote_dir% at %host%
pscp -batch -pw xxxxxx %func% root@%host%:%remote_dir%
IF %ERRORLEVEL% NEQ 0 GOTO error

echo Transferring %datetime% to %remote_dir% at %host%
pscp -batch -pw xxxxxx %datetime% root@%host%:%remote_dir%
IF %ERRORLEVEL% NEQ 0 GOTO error

echo Setting %remote_script% permissions
plink -batch -pw xxxxxx root@%host% chmod -v a+x %remote_dir%/*.sh
IF %ERRORLEVEL% NEQ 0 GOTO error

echo Executing %remote_script% at %host%
plink -batch -pw xxxxxx root@%host% %remote_script%
IF %ERRORLEVEL% NEQ 0 GOTO error

plink -batch -pw xxxxxx root@%host% rm -f %remote_dir%/*.sh

echounix "==== %DATE% %TIME% %host% completed ============================"
echounix


exit /B 0
GOTO end

:error

exit /B 1
:end
