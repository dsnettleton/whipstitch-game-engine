#	Project Florin Makefile
#	D. Scott Nettleton
#
#	This is the C++ makefile for the Whipstitch Game Engine,
#	codenamed Project Florin.
#	For more information, email:
# 		dsnettleton at gmail dot com

#     Copyright D. Scott Nettleton, 2013-2016
#     This software is released under the terms of the MIT license

CC=g++
WIN_CC=i586-mingw32msvc-g++
COMPILER_NAME=GNU G++
WIN_COMPILER_NAME=Minimalist GNU Win32 Compiler
REMOVAL_BIN=/bin/rm -f

PROJECT_NAME = Florin.bin
DEBUG_NAME = Florin_dbg.bin
PROFILE_NAME = Florin_profile.bin
WIN_NAME = Florin.exe
INCLUDE_DIRS = -I/usr/include/freetype2 -I/usr/include/bullet
RELEASE_OPTIONS= -O3 -DNDEBUG
DEBUG_OPTIONS= -O0 -g3 -DDEBUG
PROFILE_OPTIONS= -O0 -g3 -DDEBUG -D_PROFILE
WINDOWS_OPTIONS= -I/usr/i586-mingw32msvc/include -I/usr/i586s-mingw32msvc/lib
OPTIONS = -Wall -fmessage-length=0 $(INCLUDE_DIRS)

OBJ_ASSETS = whipstitch/wsAssets/wsAnimation.o whipstitch/wsAssets/wsAsset.o whipstitch/wsAssets/wsButton.o whipstitch/wsAssets/wsFont.o whipstitch/wsAssets/wsMesh.o whipstitch/wsAssets/wsModel.o whipstitch/wsAssets/wsPanel.o whipstitch/wsAssets/wsPanelElement.o whipstitch/wsAssets/wsText.o whipstitch/wsAssets/wsTextBox.o
OBJ_AUDIO = whipstitch/wsAudio/wsSoundManager.o whipstitch/wsAudio/wsSound.o whipstitch/wsAudio/wsMusic.o
OBJ_GAME_FLOW = whipstitch/wsGameFlow/wsController.o whipstitch/wsGameFlow/wsEventManager.o whipstitch/wsGameFlow/wsGameLoop.o whipstitch/wsGameFlow/wsInputManager.o whipstitch/wsGameFlow/wsKeyboardInput.o whipstitch/wsGameFlow/wsPointerInput.o whipstitch/wsGameFlow/wsScene.o whipstitch/wsGameFlow/wsThreadPool.o
OBJ_GRAPHICS = whipstitch/wsGraphics/wsCamera.o whipstitch/wsGraphics/wsRenderSystem.o whipstitch/wsGraphics/wsScreen.o whipstitch/wsGraphics/wsScreenManager.o whipstitch/wsGraphics/wsShader.o
OBJ_PRIMITIVES = whipstitch/wsPrimitives/wsCube.o whipstitch/wsPrimitives/wsPlane.o
OBJ_UTILS = whipstitch/wsUtils/mat4.o whipstitch/wsUtils/quat.o whipstitch/wsUtils/vec4.o whipstitch/wsUtils/wsLog.o whipstitch/wsUtils/wsMemoryStack.o whipstitch/wsUtils/wsOperations.o whipstitch/wsUtils/wsProfileManager.o whipstitch/wsUtils/wsTime.o whipstitch/wsUtils/wsTransform.o whipstitch/wsUtils/wsTrig.o whipstitch/wsUtils/wsTypes.o
OBJ_WHIPSTITCH = whipstitch/ws.o
OBJS = $(OBJ_UTILS) $(OBJ_GRAPHICS) $(OBJ_GAME_FLOW) $(OBJ_ASSETS) $(OBJ_PRIMITIVES) $(OBJ_AUDIO) $(OBJ_WHIPSTITCH) ./main.o ./wsDemo.o

BULLET_LIBS = -lBulletDynamics -lBulletCollision -lLinearMath
LIBS = -lfreetype -lgomp -lpthread -lboost_system -lboost_filesystem -lglfw -lGL -lGLEW -lGLU -lSOIL -lalut -lopenal -lvorbisfile $(BULLET_LIBS)
DEFINITIONS = -DWS_DEFAULT_RENDER_MODE=2

#Rules for Building Object Files
%.o: %.cpp %.h
	#  $(COMPILER_NAME) Compiler - Building: $<
	$(CC) -o "$@" -c "$<" $(OPTIONS)
	#  Finished building: $<

main.o: main.cpp wsDemo.h
	#  $(COMPILER_NAME) Compiler - Building: $<
	$(CC) -o "$@" -c "$<" $(OPTIONS)
	#  Finished building: $<

debug: OPTIONS += $(DEBUG_OPTIONS)
debug: PROJECT_NAME = $(DEBUG_NAME)
debug: executable

release: OPTIONS += $(RELEASE_OPTIONS)
release: executable

profile: OPTIONS += $(PROFILE_OPTIONS)
profile: PROJECT_NAME = $(PROFILE_NAME)
profile: executable

# windows: CC = $(WIN_CC)
windows: COMPILER_NAME = $(WIN_COMPILER_NAME)
windows: OPTIONS += -lmingw32
windows: PROJECT_NAME = $(WIN_NAME)
windows: release

executable: $(OBJS)
	#  Building Target: $@
	$(CC) $(OBJS) -o $(PROJECT_NAME) $(OPTIONS) $(LIBS)
	#  Target $@ Built

#	Clean Command
clean:
	#  Cleaning Build
	$(REMOVAL_BIN) $(OBJS) $(PROJECT_NAME) $(DEBUG_NAME) $(PROFILE_NAME) -r
	#  Cleaned
