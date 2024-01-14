echo WSF Source Backup Tool    Copyright (C)1998-2003 Radek Tetík - SpyTech

set bak_folder=%temp%\BAK
set archive_name=wsf_wsl.zip

xcopy * "%bak_folder%\WSF\" /y /E /exclude:exclude2.txt

pushd ..\wsl
call export_src_embedded.bat "%bak_folder%\WSL\"
popd

pushd "%bak_folder%"
zip -r -9 -q "%archive_name%" *
popd

move "%bak_folder%\%archive_name%" ..
rmdir /s /q %bak_folder%
