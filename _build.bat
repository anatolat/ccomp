@echo off
call "%VS120COMNTOOLS%\vsvars32.bat"
msbuild /target:rebuild,ccomp /property:Configuration=Release;Platform=Win32
