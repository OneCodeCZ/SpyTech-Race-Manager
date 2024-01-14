@echo off

pushd ..\wsf
call copy_en_to_project.bat embedded
popd

echo -----------------------------------------------------------
echo EN to PROJECT
echo -----------------------------------------------------------

if not exist cz_resources_in_project_flag goto COPY

rem Backup CZ to RCT

copy /Y "Data Management\Data Management.rc" "RCT\dm_cz.rc"
copy /Y "Measurement\Measurement.rc" "RCT\msm_cz.rc"

rename cz_resources_in_project_flag en_resources_in_project_flag 


rem Copy EN to PROJECT
:COPY
copy /Y "RCT\dm_en.rc" "Data Management\Data Management.rc"
copy /Y "RCT\msm_en.rc" "Measurement\Measurement.rc"

del "Data Management\debug\Data Management.res"
del "Data Management\release\Data Management.res"
del "Data Management\Data Management.aps"

del "Measurement\debug\Measurement.res"
del "Measurement\release\Measurement.res"
del "Measurement\Measurement.aps"

echo *** OK ***
pause

