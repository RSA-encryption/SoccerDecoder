@echo off
%~dp0\translators\MSBuild\Current\Bin\msbuild.exe VS_SD_BENCHMARK.sln /p:Configuration=Release /v:normal
%~dp0\x64\Release\VS_SD_BENCHMARK.exe
pause