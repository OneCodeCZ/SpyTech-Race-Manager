@echo off

pushd ..\wsf
call copy_cz_to_rct.bat embedded
popd

echo -----------------------------------------------------------
echo RM: CZ to RCT
echo -----------------------------------------------------------

if not exist cz_resources_in_project_flag goto ERROR

copy /Y "Data Management\Data Management.rc" "RCT\dm_cz.rc"
copy /Y "Measurement\Measurement.rc" "RCT\msm_cz.rc"

echo *** OK ***
pause
goto :EOF

:ERROR
echo Czech resources are NOT in the project!
pause
