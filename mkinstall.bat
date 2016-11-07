@echo off

rem *************************************************
rem This is the packaging batch file for Windows.
rem For Linux, run "make" in the client subdirectory.
rem *************************************************
set platform=%1
set CLIENT_LIBRARY_VERSION_VALUE=6.0.0

set BUILD_DIR=builds
if "%platform%"=="win32" (
	set DIST_DIR_PREFIX=%BUILD_DIR%\simapi-c-win32-%CLIENT_LIBRARY_VERSION_VALUE%
)

if "%platform%"=="win64" (
	set DIST_DIR_PREFIX=%BUILD_DIR%\simapi-c-win64-%CLIENT_LIBRARY_VERSION_VALUE%
)

set DIST_DIR=%DIST_DIR_PREFIX%\simapi-c-%CLIENT_LIBRARY_VERSION_VALUE%
set LATEST_DIR=%BUILD_DIR%\simapi-c-latest

if "%platform%"=="win32" (
	set subdir="win32"
)

if "%platform%"=="win64" (
	set subdir="win64"
)	

set SAMPLES_DIR=%DIST_DIR%\samples
set NVP_SAMPLE_DIR=%SAMPLES_DIR%\nvp
set XML_SAMPLE_DIR=%SAMPLES_DIR%\xml

echo Building the client %platform%

if "%platform%"=="win32" (
    devenv client.sln /rebuild "Release|Win32"
	
)

if "%platform%"=="win64" (
	devenv client.sln /rebuild "Release|x64"
)

echo ---------------------------------------------------------------
echo Client Library Version : %CLIENT_LIBRARY_VERSION_VALUE%
echo Distribution Directory : %DIST_DIR%
echo ---------------------------------------------------------------

:LOOP
echo Removing %DIST_DIR%...
if exist %DIST_DIR% rmdir /s /Q %DIST_DIR%
echo Removing %DIST_DIR_PREFIX%...
if exist %DIST_DIR_PREFIX% rmdir /s /Q %DIST_DIR_PREFIX%
if exist %DIST_DIR_PREFIX% goto LOOP


echo Creating %BUILD_DIR%...
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
echo Creating %DIST_DIR_PREFIX%...
mkdir %DIST_DIR_PREFIX%
echo Creating %DIST_DIR%...
mkdir %DIST_DIR%

echo Copying files and subdirectories into %DIST_DIR%...

mkdir %DIST_DIR%\include
mkdir %DIST_DIR%\include\XMLClient
mkdir %DIST_DIR%\include\NVPClient
mkdir %DIST_DIR%\include\Common
mkdir %DIST_DIR%\lib
mkdir %DIST_DIR%\keys
mkdir %DIST_DIR%\logs
mkdir %SAMPLES_DIR%
mkdir %NVP_SAMPLE_DIR%
mkdir %XML_SAMPLE_DIR%

copy /Y XMLClient\XMLCybersource.h %DIST_DIR%\include\XMLClient
copy /Y XMLClient\soapStub.h %DIST_DIR%\include\XMLClient
copy /Y XMLClient\soapITransactionProcessorProxy.h %DIST_DIR%\include\XMLClient
copy /Y XMLClient\stdsoap2.h %DIST_DIR%\include\XMLClient
copy /Y XMLClient\soapH.h %DIST_DIR%\include\XMLClient

copy /Y NVPClient\NVPCybersource.h %DIST_DIR%\include\NVPClient

copy /Y BaseClient\util.h %DIST_DIR%\include\Common

copy /Y XMLClient\Release\%subdir%\XMLClient.dll %DIST_DIR%\lib
copy /Y XMLClient\Release\%subdir%\XMLClient.lib %DIST_DIR%\lib
copy /Y NVPClient\Release\%subdir%\NVPClient.dll %DIST_DIR%\lib
copy /Y NVPClient\Release\%subdir%\NVPClient.lib %DIST_DIR%\lib
copy /Y BaseClient\Release\%subdir%\BaseClient.dll %DIST_DIR%\lib
copy /Y BaseClient\Release\%subdir%\BaseClient.lib %DIST_DIR%\lib
copy /Y lib\libxml2\%subdir%\lib\libxml2.dll %DIST_DIR%\lib

copy /Y xml\Release\%subdir%\XMLTest.exe %DIST_DIR%\samples\xml
copy /Y XMLClient\Release\%subdir%\XMLClient.dll %DIST_DIR%\samples\xml
copy /Y BaseClient\Release\%subdir%\BaseClient.dll %DIST_DIR%\samples\xml
copy /Y lib\libxml2\%subdir%\lib\libxml2.dll %DIST_DIR%\samples\xml

copy /Y NVPTest\Release\%subdir%\NVPTest.exe %DIST_DIR%\samples\nvp
copy /Y NVPClient\Release\%subdir%\NVPClient.dll %DIST_DIR%\samples\nvp
copy /Y BaseClient\Release\%subdir%\BaseClient.dll %DIST_DIR%\samples\nvp
copy /Y lib\libxml2\%subdir%\lib\libxml2.dll %DIST_DIR%\samples\nvp

copy /Y resources\cybs.ini %DIST_DIR%\samples
copy /Y resources\auth.xml %DIST_DIR%\samples
copy /Y resources\ca-bundle.crt %DIST_DIR%\keys

echo Deleting %DIST_DIR_PREFIX%.zip...
if exist %DIST_DIR_PREFIX%.zip del %DIST_DIR_PREFIX%.zip

cd %BUILD_DIR%

dir

if "%platform%"=="win32" (
echo Creating the new simapi-c-win32-%CLIENT_LIBRARY_VERSION_VALUE%.zip...
jar Mcf simapi-c-win32-%CLIENT_LIBRARY_VERSION_VALUE%.zip simapi-c-win32-%CLIENT_LIBRARY_VERSION_VALUE%
)

if "%platform%"=="win64" (
echo Creating the new simapi-c-win64-%CLIENT_LIBRARY_VERSION_VALUE%.zip...
jar Mcf simapi-c-win64-%CLIENT_LIBRARY_VERSION_VALUE%.zip simapi-c-win64-%CLIENT_LIBRARY_VERSION_VALUE%
)
echo Listing simapi-c-win32-%CLIENT_LIBRARY_VERSION_VALUE%...
dir simapi-c-win32-%CLIENT_LIBRARY_VERSION_VALUE% 

cd ..