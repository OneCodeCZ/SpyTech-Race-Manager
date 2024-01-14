@echo off

pushd ..\wsf
call copy_cz_to_project.bat embedded
popd

echo -----------------------------------------------------------
echo RM: CZ to PROJECT
echo -----------------------------------------------------------

if exist cz_resources_in_project_flag goto ERROR

copy /Y "RCT\dm_cz.rc" "Data Management\Data Management.rc"
copy /Y "RCT\msm_cz.rc" "Measurement\Measurement.rc"

del "Data Management\debug\Data Management.res"
del "Data Management\release\Data Management.res"
del "Data Management\Data Management.aps"

del "Measurement\debug\Measurement.res"
del "Measurement\release\Measurement.res"
del "Measurement\Measurement.aps"

rename en_resources_in_project_flag cz_resources_in_project_flag
echo *** OK ***
pause
goto :EOF

:ERROR
echo Czech resources ARE already in the project!
pause
