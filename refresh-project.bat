cmake -B build\debug\ -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Debug -DENABLE_HOT_RELOAD=ON
cmake -B build\release\ -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Release -DENABLE_HOT_RELOAD=OFF
::xcopy /f /y .\build\debug\compile_commands.json .\game\exe\compile_commands.json
::xcopy /f /y .\build\debug\compile_commands.json .\game\hot\compile_commands.json
mklink /D build\debug\bin\resources ..\..\..\resources\