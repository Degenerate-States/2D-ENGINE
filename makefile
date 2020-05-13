CC = cl.exe
LINK = link.exe
CFLAGS = -EHsc -Z7 -FC -O2 $(INC)
LIBS = Shell32.lib SDL2.lib opengl32.lib
SRC = src
BUILD = build

INC = -I lib/SDL/include -I lib/GLAD/include -I include
EXE = game.exe

$(EXE): $(BUILD)\*.obj
	nmake glad.obj
	@if not exist $(BUILD) mkdir $(BUILD)
	$(LINK) -LIBPATH:lib/SDL/lib/win64 -SUBSYSTEM:CONSOLE $** $(LIBS) -OUT:"$(BUILD)\$(EXE)" -DEBUG:FULL
	@if not exist $(BUILD)\SDL2.dll xcopy.exe lib\SDL\lib\win64\SDL2.dll $(BUILD)

glad.obj:
	@if not exist $(BUILD) mkdir $(BUILD)
	$(CC) $(CFLAGS) -Fo:$(BUILD)\ -c lib\GLAD\src\glad.c

{$(SRC)}.cpp{$(BUILD)}.obj:: 
	@if not exist $(BUILD) mkdir $(BUILD)
	$(CC) $(CFLAGS) -Fo:$(BUILD)\ -c $< 

clean:
	del $(BUILD)\$(EXE)