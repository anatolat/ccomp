@echo off
call "%VS120COMNTOOLS%\vsvars32.bat"

setlocal enabledelayedexpansion
set CC=..\Debug\ccomp

set $sucessful_count=0
set $failed_count=0

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
test_array_local.c ^
test_array_chars.c ^
test_strs.c ^
100doors_simplified.c ^
test_strs2.c ^
test_scopes.c

) else (
  set src=%1
)

for %%x in (%src%) do call :runTest %%x
goto :end

:runTest 
  set input=%1
  set name=%~n1

  rem echo.
  echo Run '%name%'
  %CC% %input% > %name%.asm 2> %name%.err
  if not %errorlevel% equ 0 (goto :fail)

  ML /c /nologo /Sg /Zi  /W3 /errorReport:prompt  /Ta %name%.asm >NUL
  if not %errorlevel% equ 0 (goto :fail)

  LINK /nologo /SUBSYSTEM:CONSOLE %name%.obj >NUL
  if not %errorlevel% equ 0 (goto :fail)

  rem echo. Run '%name%'
  %name% > %name%.out

  rem echo. Compare output '%name%'
  FC %name%.expected %name%.out >NUL 2> NUL

  rem if %errorlevel% equ 0 (echo ok)
  if not %errorlevel% equ 0 (echo Test '%name%' failed: different output, code=%errorlevel% & goto :fail)

  set /a "$sucessful_count+=1"
  goto :eof

:fail
  set $fails=%$fails% %input%
  set /a "$failed_count+=1"
  goto :eof

:end

echo.
echo Sucessful %$sucessful_count%, Failed %$failed_count% 
  
if not "%$fails%"=="" (
  echo Failed tests: %$fails%
) 
