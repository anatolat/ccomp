@echo off
call "%VS120COMNTOOLS%\vsvars32.bat"

setlocal enabledelayedexpansion

set src= ^
ccomp.c ^
codegen.c ^
context.c ^
lexer.c ^
preprocessor.c ^
token.c ^
type_info.c ^
types.c

for %%x in (%src%) do call :compile %%x
goto :link


:compile
  set input=%1
  set name=%~n1

  .\bin\ccomp %input% bin\%name%.asm
  if not %errorlevel% equ 0 (goto :eof)
  
  ML /c /nologo /Sg /Zi  /W3 /errorReport:prompt /Fo bin\%name%.obj /Ta bin\%name%.asm 
  if not %errorlevel% equ 0 (goto :eof)

  goto :eof


:link
LINK /nologo /SUBSYSTEM:CONSOLE bin\*.obj /OUT:bin\ccomp.exe
if not %errorlevel% equ 0 (goto :eof)

