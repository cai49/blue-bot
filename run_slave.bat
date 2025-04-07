@echo off

call .\Scripts\activate
call python.exe .\\SlaveInterface\\main.py
if %errorlevel% neq 0 goto end

:end
call deactivate