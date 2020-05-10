@echo off
if not exist build mkdir build
pushd build
if not exist SDL2.dll xcopy ..\lib\SDL\lib\win64\SDL2.dll
cl -EHsc -Zi -FC -O2^
    ..\src\* ..\lib\GLAD\src\glad.c^
	-Fe:engine.exe^
    -I ../lib/SDL/include -I ../lib/GLAD/include -I ../include^
    Shell32.lib SDL2.lib opengl32.lib^
    -link -LIBPATH:../lib/SDL/lib/win64 -SUBSYSTEM:CONSOLE -PDB:vc140.pdb 
popd