CC = cl.exe
LINK = link.exe
CFLAGS = -EHsc -Z7 -FC -O2 $(INC)
LIBS = Shell32.lib SDL2.lib opengl32.lib
SRC = src
BUILD = build

INC = -I lib/SDL2/include -I lib/GLAD/include -I include
EXE = game.exe

# global defines
DEFINES = -D SOUND=false -D RENDER_SPINE=false

# incremental debug build
debug: $(BUILD)\*.obj
	nmake glad.obj
	@if not exist $(BUILD) mkdir $(BUILD)
	$(LINK) -LIBPATH:lib/SDL2/lib/win64 -SUBSYSTEM:CONSOLE $** $(LIBS) -OUT:"$(BUILD)\$(EXE)" -DEBUG:FULL
	@if not exist $(BUILD)\SDL2.dll xcopy.exe lib\SDL2\lib\win64\SDL2.dll $(BUILD)

glad.obj:
	@if not exist $(BUILD) mkdir $(BUILD)
	$(CC) $(CFLAGS) -Fo:$(BUILD)\ -c lib\GLAD\src\glad.c

{$(SRC)}.cpp{$(BUILD)}.obj:: 
	@if not exist $(BUILD) mkdir $(BUILD)
	$(CC) $(CFLAGS) $(DEFINES) -Fo:$(BUILD)\ -c $<

# single pass build
exe: *.*
	@if not exist $(BUILD) mkdir $(BUILD)
	$(CC) -EHsc -O2 \
    src\* lib\GLAD\src\glad.c \
	$(DEFINES) -Fe:$(BUILD)\$(EXE) -Fo:$(BUILD)\ $(INC) $(LIBS) \
    -link -LIBPATH:lib/SDL2/lib/win64 -SUBSYSTEM:CONSOLE -PDB:vc140.pdb 
	@if not exist $(BUILD)\SDL2.dll xcopy.exe lib\SDL2\lib\win64\SDL2.dll $(BUILD)

clean:
	del /s /q "build\*"

run:
	@if exist $(BUILD)\$(EXE) $(BUILD)\$(EXE) 