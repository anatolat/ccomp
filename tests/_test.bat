@echo off
call "%VS120COMNTOOLS%\vsvars32.bat"

setlocal enabledelayedexpansion
set CC=..\Debug\ccomp

if "%1"=="" (
  set src= ^
helloworld.c ^
fact_rec.c ^
test_multiargs.c ^
test_locals.c ^
test_incdec.c ^
test_expr.c ^
test_if_else.c ^
fizzbuzz.c ^
test_array.c ^
test_array_local.c

) else (
  set src=%1
)

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

  echo. Run '%name%'
  %name% > %name%.out

  echo. Compare output '%name%'
  FC %name%.expected %name%.out >NUL 2> NUL

  if %errorlevel% equ 0 (echo ok)
  if not %errorlevel% equ 0 (echo Test '%name%' failed: different output, code=%errorlevel% & goto :eof)
  goto :eof

:end