@ECHO OFF
md "..\Packages\ubjson-converter"

:: So, uh, apparently XCOPY deletes empty subdirectories in your destination.
XCOPY "*.cpp" "..\Packages\ubjson-converter" /S /C /I /R /Y
XCOPY "*.c" "..\Packages\ubjson-converter" /S /C /I /R /Y
XCOPY "*.h" "..\Packages\ubjson-converter" /S /C /I /R /Y
XCOPY "*.vcxproj" "..\Packages\ubjson-converter" /S /C /I /R /Y
XCOPY "*.filters" "..\Packages\ubjson-converter" /S /C /I /R /Y
XCOPY "*.sln" "..\Packages\ubjson-converter" /S /C /I /R /Y
XCOPY "*.rc" "..\Packages\ubjson-converter" /C /I /R /Y
XCOPY "*.ico" "..\Packages\ubjson-converter" /C /I /R /Y

md "..\Packages\ubjson-converter\bin"

XCOPY "bin\embed.json" "..\Packages\ubjson-converter\bin\" /C /I /Y
XCOPY "bin\pretty.json" "..\Packages\ubjson-converter\bin\" /C /I /Y
XCOPY "bin\out.ubj" "..\Packages\ubjson-converter\bin\" /C /I /Y
XCOPY "bin\*.exe" "..\Packages\ubjson-converter\bin\" /C /I /Y
DEL "..\Packages\ubjson-converter\bin\*_d.exe"

Pause