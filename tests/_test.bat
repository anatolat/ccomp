@echo off

@call "%VS120COMNTOOLS%\vsvars32.bat"

set CC=..\Debug\ccomp
set src= ^
main.c 

setlocal enabledelayedexpansion
for %%x in (%src%) do call :runTest %%x
goto :end

:runTest 
  set input=%1
  set name=%~n1

  echo.
  echo.Build '%name%'
  %CC% %input% > %name%.asm
  if not %errorlevel% equ 0 (goto :eof)

  ML /c /nologo /Sg /Zi  /W3 /errorReport:prompt  /Ta %name%.asm
  if not %errorlevel% equ 0 (goto :eof)

  LINK /nologo /SUBSYSTEM:CONSOLE %name%.obj  
  if not %errorlevel% equ 0 (goto :eof)

  echo. Run '%name'
  %name% > %name%.out

  echo. Compare output '%name%'
  FC %name%.expected %name%.out >NUL 2> NUL

  if %errorlevel% equ 0 (echo ok)
  if not %errorlevel% equ 0 (echo Test '%name%' failed: different output, code=%errorlevel% & goto :eof)
  goto :eof

:end