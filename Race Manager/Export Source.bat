xcopy * "c:\!\RM\Race Manager\*" /y /E /exclude:exclude_src.txt
xcopy "..\WSF\*" "c:\!\RM\WSF\" /y /E /exclude:exclude_src.txt
xcopy "..\WSL\*" "c:\!\RM\WSL\" /y /E /exclude:exclude_src.txt

pushd
cd "c:\!\RM"
zip -R rm_src_xxxx.zip *
popd
