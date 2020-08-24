CC = cl.exe
LINK = link.exe
CFLAGS = -EHsc -Z7 -FC $(INC)
LIBS = Shell32.lib SDL2.lib opengl32.lib
SRC = src
BUILD = build
ASSETS = assets

INC = -I lib/SDL2/include -I lib/GLAD/include -I include
PROGRAM_NAME = POLYGUN

# global defines
DEBUG_DEFINES = -D SOUND=false -D RENDER_SPINE=ture -D COLLISION_PRINT=false
PROD_DEFINES = -D SOUND=true -D RENDER_SPINE=false -D COLLISION_PRINT=false


# incremental debug build
debug: $(BUILD)\*.obj
	nmake glad.obj
	@if not exist $(BUILD) mkdir $(BUILD)
	$(LINK) -LIBPATH:lib/SDL2/lib/win64 -SUBSYSTEM:CONSOLE $** $(LIBS) -OUT:"$(BUILD)\$(PROGRAM_NAME).exe" -DEBUG:FULL
	@if not exist $(BUILD)\SDL2.dll xcopy.exe lib\SDL2\lib\win64\SDL2.dll $(BUILD)

glad.obj:
	@if not exist $(BUILD) mkdir $(BUILD)
	$(CC) $(CFLAGS) -Fo:$(BUILD)\ -c lib\GLAD\src\glad.c

{$(SRC)}.cpp{$(BUILD)}.obj:: 
	@if not exist $(BUILD) mkdir $(BUILD)
	$(CC) $(CFLAGS) $(DEBUG_DEFINES) -Fo:$(BUILD)\ -c $<

# single pass production build
prod: *.*
	@if not exist $(BUILD) mkdir $(BUILD)
	$(CC) -EHsc -O2 \
    src\* lib\GLAD\src\glad.c \
	$(PROD_DEFINES) -Fe:$(BUILD)\$(PROGRAM_NAME).exe -Fo:$(BUILD)\ $(INC) $(LIBS) \
    -link -LIBPATH:lib/SDL2/lib/win64 -SUBSYSTEM:CONSOLE -PDB:vc140.pdb 
	@if not exist $(BUILD)\SDL2.dll xcopy.exe lib\SDL2\lib\win64\SDL2.dll $(BUILD)

clean:
	del /s /q "build\*"
	@if exist $(PROGRAM_NAME)_test.zip del $(PROGRAM_NAME)_test.zip 

run:
	@if exist $(BUILD)\$(PROGRAM_NAME).exe $(BUILD)\$(PROGRAM_NAME).exe 

zip:
	nmake clean
	nmake prod
	@if not exist $(PROGRAM_NAME)_test.zip powershell "Compress-Archive -Path $(BUILD)/$(PROGRAM_NAME).exe -DestinationPath $(PROGRAM_NAME)_test.zip"
	@if exist $(PROGRAM_NAME)_test.zip powershell "Compress-Archive -Path $(BUILD)/SDL2.dll -Update -DestinationPath $(PROGRAM_NAME)_test.zip"
	@if exist $(PROGRAM_NAME)_test.zip powershell "Compress-Archive -Path $(ASSETS) -Update -DestinationPath $(PROGRAM_NAME)_test.zip"
