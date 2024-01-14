@echo off

echo -----------------------------------------------------------
echo WSF: CZ to PROJECT
echo -----------------------------------------------------------

if exist cz_resources_in_project_flag goto ERROR

copy /Y "RCT\wsf_cz.rc" "wsf\wsf.rc"

del "wsf\debug\wsf.res"
del "wsf\release\wsf.res"
del "wsf\wsf.aps"

rename en_resources_in_project_flag cz_resources_in_project_flag
echo *** OK ***
goto :DONE

:ERROR
echo Czech resources ARE already in the project!

:DONE
if not "%1" == "embedded" pause
