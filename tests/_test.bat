@echo off
call "%VS120COMNTOOLS%\vsvars32.bat"

setlocal enabledelayedexpansion

set CC=..\Debug\ccomp

set $sucessful_count=0
set $failed_count=0

if "%1"=="" (
  set src= ^
empty.c ^
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
test_scopes.c ^
test_break_continue.c ^
test_return.c ^
test_ptr.c ^
test_compound_assignment.c ^
test_ptr_char.c ^
test_compound_assignment_ptr.c ^
test_logical_expr.c ^
test_ternary.c ^
test_switch.c ^
test_enum.c ^
test_sizeof.c

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
  if exist "%name%.asm" del /f /q "%name%.asm"
  if exist "%name%.obj" del /f /q "%name%.obj"
  if exist "%name%.exe" del /f /q "%name%.exe"
  if exist "%name%.out" del /f /q "%name%.out"

  %CC% %input% %name%.asm 2> %name%.err
  if not %errorlevel% equ 0 (goto :fail)

  ML /c /nologo /Sg /Zi  /W3 /errorReport:prompt  /Ta %name%.asm 
  if not %errorlevel% equ 0 (goto :fail)

  LINK /nologo /SUBSYSTEM:CONSOLE %name%.obj 
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
