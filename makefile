engine.exe:
	mkdir build &
	cl -EHsc -Zi -FC \
	src\* lib\GLAD\src\glad.c \
	-I lib/SDL/include -I lib/GLAD/include -I include\
	Shell32.lib SDL2.lib opengl32.lib \
	-Fe:build/engine.exe -Fo"build/" \
	-link -LIBPATH:lib/SDL/lib/win64 -SUBSYSTEM:CONSOLE -PDB:build/vc140.pdb &
	del *.pdb &
	xcopy.exe lib\SDL\lib\win64\SDL2.dll build
clean:
	del /s /q "build\*"