@echo off
call "%VS120COMNTOOLS%\vsvars32.bat"

.\bin\ccomp ccomp.c bin\ccomp.asm
if not %errorlevel% equ 0 (goto :eof)

ML /c /nologo /Sg /Zi  /W3 /errorReport:prompt /Fo bin\ccomp.obj /Ta bin\ccomp.asm 
if not %errorlevel% equ 0 (goto :eof)

LINK /nologo /SUBSYSTEM:CONSOLE bin\ccomp.obj /OUT:bin\ccomp.exe
if not %errorlevel% equ 0 (goto :eof)
