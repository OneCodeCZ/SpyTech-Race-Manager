@echo off

echo -----------------------------------------------------------
echo WSF: CZ to RCT
echo -----------------------------------------------------------

if not exist cz_resources_in_project_flag goto ERROR

copy /Y "wsf\wsf.rc" "RCT\wsf_cz.rc"
echo *** OK ***
goto :DONE

:ERROR
echo Czech resources are NOT in the project!

:DONE
if not "%1" == "embedded" pause
