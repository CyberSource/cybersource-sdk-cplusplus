# cybersource-sdk-cplusplus

[![Build Status](https://api.travis-ci.com/CyberSource/cybersource-sdk-cplusplus.svg?token=rrCMh7Lm3FTR6wnVxCQ6&branch=future)]
(https://travis-ci.com/CyberSource/cybersource-sdk-cplusplus)

CyberSource C++ SDK for windows
##Requirements
###Windows
1. Visual Studio 2012 or later
2. gsoap 2.8

###Linux
1. g++ (GCC) 4.8.3
2. gsoap 2.8

##Prerequisites
1. Registration
A CyberSource Evaluation account is required. Sign up here:  http://www.cybersource.com/register
Complete your Evaluation account creation by following the instructions in the Registration email

2. Transaction Security Keys
Create security keys in the Enterprise Business Center (ebctest) after you've created your Merchant Admin account.
Refer to our Developer's Guide for details http://apps.cybersource.com/library/documentation/dev_guides/security_keys/creating_and_using_security_keys.pdf> under Simple Order API Security Keys to generate .P12 key.

##Installing & building the SDK
###Windows
1. Download the cybersource-sdk-cpp-master.zip package into a directory of your choice.
2. Extract and go to the cybersource-sdk-cpp directory.
3. Open Solution "client.sln" in Visual Studio.
4. Build/Rebuild the Solution.
5. To generate binary run mkinstall.bat win32 or mkinstall.bat win64 to build 32 bit or 64 bit respectively.
 
###Linux
1. Download the cybersource-sdk-cpp-master.zip package into a directory of your choice.
2. Extract and go to the cybersource-sdk-cpp directory.
3. Run "make clean" to clean up the old build/binaries.
4. Run "make build32" or "make build64" to build 32 or 64 bit respectively.
5. Run "make dist" to generate tar.
6. Binaries will be generated in /builds/simapi-c-linux-6.0.0.tar.gz.

##Running the Samples
1. Edit cybs.ini (\samples\cybs.ini)

* merchantID. MerchantID that will be used for ending the request. If both config as well as request message don’t have the value, then error will be thrown.

* keysDirectory. Location of the merchant’s security key.

* keyFilename. Name of the merchant p12 file (e.g merchant.p12). If not specified then by-default it takes <merchantID>.p12 as the value.

* password. Password of merchant p12 file. If not specified then by-default it uses merchantID as the password.

* enableLog. If set to true then it will create a log file and log the transaction details.

* logDirectory. Path of the log file directory.

* logFilename. Name of the log file.

* sslCertFile. Path of the SSL ca bundle certificate file (e.g C:\certfile\ca-bundle.crt). If not specified, then it will look into keysDirectory location for a file name ca-bundle.crt.

* useSignAndEncrypted. This is a boolean flag, if set to true then request will be both signed as well as encrypted.

* sendToAkamai. This is a boolean flag, if set to true, the request will be routed through Amamai to CyberSource.

* sendToProduction. This is a boolean flag, if set to true the request will be sent to production endpoint.

* serverURL. This config parameter will take precedence over sendToProduction and sendToAkamai config parameters. By default, the "serverURL" configuration is commented out.
 
2. Running XML request
Go to \samples\xml in command prompt and run XMLTest.exe.

3. Running NVP request
Go to \samples\nvp in command prompt and run NVPTest.exe

For running the sample follow "Running the Samples" section.

##Generating stub code for different wsdl version:
1. Go to /stub_generator.
2. copy the wsdl for particular version from "https://ics2wsa.ic3.com/commerce/1.x/transactionProcessor/" to NVP or XML.
3. Run stub_generator.bat for windows or stub_generator.sh for linux and follow the instruction.
4. Rebuild the SDK. Follow Installing & building the SDK


