@echo off

set ADDR=C:/Users/.../LiBOS

copy ".\Win32makefile" ".\bin\Win32makefile.bat" >nul
	
cmd.exe /k "C:\cygwin\bin\mintty.exe /bin/sh -lc 'cd %ADDR%; cygstart ./bin/Win32makefile.bat; exit; exec bash'&exit"
