@echo off
copy 3rdparty\Debug\*.dll x64\Debug
copy 3rdparty\Release\*.dll x64\Release
copy 3rdparty\*.dll x64\Release
copy 3rdparty\*.dll x64\Debug
copy 3rdparty\lua542\*.dll x64\Release
copy 3rdparty\lua542\*.dll x64\Debug
