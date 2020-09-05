CC = cl.exe
LINK = link.exe
CFLAGS = -nologo -EHsc -Z7 -FC -MP $(INC)
PROD_CFLAGS = -nologo -EHsc -O2 -MP $(INC)
LIBS = Shell32.lib SDL2.lib opengl32.lib
PROD_LIBS = SDL2.lib opengl32.lib
SRC = src
IMGUI = lib\IMGUI
BUILD = build
ASSETS = assets

INC = -I lib/SDL2/include -I lib/GLAD/include -I lib/IMGUI -I include
PROGRAM_NAME = POLYGUN

# global defines
DEBUG_DEFINES = -D SOUND=true -D RENDER_SPINE=ture -D COLLISION_PRINT=false
PROD_DEFINES = -D SOUND=true -D RENDER_SPINE=false -D COLLISION_PRINT=false


# debug build
debug: $(BUILD)\*.obj
	@if not exist $(BUILD)\glad.obj nmake -nologo glad
	@if not exist $(BUILD)\imgui.obj nmake -nologo imgui
	@if not exist $(BUILD) mkdir $(BUILD)
	$(LINK) -nologo -LIBPATH:lib/SDL2/lib/win64 -SUBSYSTEM:CONSOLE $** $(LIBS) -OUT:"$(BUILD)\$(PROGRAM_NAME).exe" -DEBUG:FULL
	@if not exist $(BUILD)\SDL2.dll xcopy.exe lib\SDL2\lib\win64\SDL2.dll $(BUILD)
glad: *.*
	$(CC) $(CFLAGS) -Fo:$(BUILD)\ -c lib\GLAD\src\glad.c
imgui: *.*
	$(CC) $(CFLAGS) $(DEBUG_DEFINES) -Fo:$(BUILD)\ -c lib\IMGUI\*.cpp
# compile each object from src files
{$(SRC)}.cpp{$(BUILD)}.obj:: 
	@if not exist $(BUILD) mkdir $(BUILD)
	$(CC) $(CFLAGS) $(DEBUG_DEFINES) -Fo:$(BUILD)\ -c $<

# production build
prod: *.*
	@if not exist $(BUILD) mkdir $(BUILD)
	$(CC) $(PROD_CFLAGS) src\* lib\GLAD\src\glad.c \
	$(PROD_DEFINES) -Fe:$(BUILD)\$(PROGRAM_NAME).exe -Fo:$(BUILD)\ $(PROD_LIBS) \
    -link -LIBPATH:lib/SDL2/lib/win64 -SUBSYSTEM:WINDOWS
	@if not exist $(BUILD)\SDL2.dll xcopy.exe lib\SDL2\lib\win64\SDL2.dll $(BUILD)

# demo imgui build
imgui_demo: *.*
	@if not exist $(BUILD) mkdir $(BUILD)
	$(CC) $(PROD_CFLAGS) lib\GLAD\src\glad.c lib\IMGUI\*.cpp lib\IMGUI\examples\main.cpp \
	$(PROD_DEFINES) -Fe:$(BUILD)\$(PROGRAM_NAME).exe -Fo:$(BUILD)\ $(LIBS) \
    -link -LIBPATH:lib/SDL2/lib/win64 -SUBSYSTEM:WINDOWS
	@if not exist $(BUILD)\SDL2.dll xcopy.exe lib\SDL2\lib\win64\SDL2.dll $(BUILD)

# clean out the build dir and delete zip and 
clean: *.*
	del /s /q "build\*"
	@if exist $(PROGRAM_NAME)_test.zip del $(PROGRAM_NAME)_test.zip
	@if exist $(PROGRAM_NAME)_test rmdir /S /Q $(PROGRAM_NAME)_test

# run program
run: *.*
	@if exist $(BUILD)\$(PROGRAM_NAME).exe $(BUILD)\$(PROGRAM_NAME).exe 

# compress the build dir and add SDL2.dll and assets folder to it
zip: *.*
	nmake -nologo clean
	nmake -nologo prod
	@if not exist $(PROGRAM_NAME)_test.zip powershell "Compress-Archive -Path $(BUILD)/$(PROGRAM_NAME).exe -DestinationPath $(PROGRAM_NAME)_test.zip"
	@if exist $(PROGRAM_NAME)_test.zip powershell "Compress-Archive -Path $(BUILD)/SDL2.dll -Update -DestinationPath $(PROGRAM_NAME)_test.zip"
	@if exist $(PROGRAM_NAME)_test.zip powershell "Compress-Archive -Path $(ASSETS) -Update -DestinationPath $(PROGRAM_NAME)_test.zip"
# unpack the production zip to POLYGUN_test
unzip: *.* 
	powershell "Expand-Archive -path $(PROGRAM_NAME)_test.zip -destinationpath $(PROGRAM_NAME)_test"