@echo off

echo -----------------------------------------------------------
echo WSF: EN to PROJECT
echo -----------------------------------------------------------

if not exist cz_resources_in_project_flag goto COPY

copy /Y "wsf\wsf.rc" "RCT\wsf_cz.rc"
rename cz_resources_in_project_flag en_resources_in_project_flag 

:COPY
copy /Y "RCT\wsf_en.rc" "wsf\wsf.rc"

del "wsf\debug\wsf.res"
del "wsf\release\wsf.res"
del "wsf\wsf.aps"

echo *** OK ***
if not "%1" == "embedded" pause

