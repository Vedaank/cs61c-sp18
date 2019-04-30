@echo off
setlocal enabledelayedexpansion
:: Just run this file and you can test your circ files!
:: Make sure your files are in the directory above this one though!
:: Credits to William Huang and Stephan Kaminsky
set testpath=tests\cpu_pipelined


::!!!!!!!!!!!!SET the python location here. Just repace "nul" with with the directory to python ex: "C:\Python27\python.exe"!!!!!!
set pythonpath="null"


::Leave this alone
set defname=python
set defname2=python2.7
set defname3=python3
set defpy=C:\Python\
set defpy2=C:\Python27\
set defpy3=C:\Python3\
set defn=%defname% %defname3% %defname2%
set defn_size=3
set defp=%defpy% %defpy3% %defpy2%
set defp_size=3

copy alu.circ %testpath%>nul
IF %ERRORLEVEL% NEQ 0 (
    echo.
    ECHO [ERROR] Copy failed: alu.circ to tests^^!
    if exist %testpath%/alu.circ echo Found an alu.circ file in the test folder already. &goto choicea
    goto end
    :choicea
    set /P c=Do you want to use it [Y/N]?
    if /I "%c%" EQU "Y" goto :cpreg
    if /I "%c%" EQU "N" goto :end
    echo Please only enter y or n^^!
    echo.
    goto :choicea
)
:cpreg
copy regfile.circ %testpath%>nul
IF %ERRORLEVEL% NEQ 0 (
    echo.
  ECHO [ERROR] Copy failed: regfile.circ to tests^^!
  if exist %testpath%/regfile.circ echo Found an regfile.circ file in the test folder already. &goto choiceb
    goto end
    :choiceb
    set /P c=Do you want to use it [Y/N]?
    if /I "%c%" EQU "Y" goto :cpmem
    if /I "%c%" EQU "N" goto :end
    echo Please only enter y or n^^!
    echo.
    goto :choiceb
)
:cpmem
copy mem.circ %testpath%>nul
IF %ERRORLEVEL% NEQ 0 (
    echo.
  ECHO [ERROR] Copy failed: mem.circ to tests^^!
  if exist %testpath%/mem.circ echo Found an mem.circ file in the test folder already. &goto choicec
    goto end
    :choicec
    set /P c=Do you want to use it [Y/N]?
    if /I "%c%" EQU "Y" goto :cpcpu
    if /I "%c%" EQU "N" goto :end
    echo Please only enter y or n^^!
    echo.
    goto :choicec
)
:cpcpu
copy cpu.circ %testpath%>nul
IF %ERRORLEVEL% NEQ 0 (
    echo.
  ECHO [ERROR] Copy failed: cpu.circ to tests^^!
  if exist %testpath%/cpu.circ echo Found an cpu.circ file in the test folder already. &goto choiced
    goto end
    :choiced
    set /P c=Do you want to use it [Y/N]?
    if /I "%c%" EQU "Y" goto :next
    if /I "%c%" EQU "N" goto :end
    echo Please only enter y or n^^!
    echo.
    goto :choiced
)
:next
cd %testpath%


::Check if python is installed and located in the path with common names
set pyloc="null"
:pathpython
(for %%a in (%defn%) do ( 
    echo [Info] Checking for %%a...
    WHERE /q %%a && set pyloc=%%a&&goto runscript
    echo [Warning] %%a not found in path.
    echo.
))
echo [Warning] Python was not found in the path^^! Checking alternative locations...
goto defaultpython

::If we got here, we now need to check if python is in common locations on the computer

:defaultpython
(for %%a in (%defp%) do (
    (for %%b in (%defn%) do (
        echo [Info] Checking for "%%a%%b.exe"
        if exist "%%a%%b.exe" set pyloc="%%a%%b.exe"&goto runscript
        echo [Warning] "%%a%%b.exe" not found.
        echo.
    ))
))
echo [Warning] Python was not found in the default install location^^! Checking for custom set location...
goto custloc

::If python is not in a default install location, we will check to see if 
:custloc
set pyloc=%pythonpath%
if not exist %pyloc% goto nend
goto runscript


::This is called if a python executable was found!
:runscript
if %pyloc%=="null" goto noPython
cls
echo %pyloc% found^^! Running python script...
echo.
%pyloc% sanity_test.py
goto end


:noPython
echo. 
echo [Error] Please open up the batch script and set the 'pythonpath' with the path to the python executable.
echo It could not be found on the system so you need to set it yourself before this will work. 
echo. 
goto end
:nend
echo.
echo [Error] Python was not found in %pyloc%^^!.
:end
cd ..
echo.
echo Press any key to exit...
pause>nul
