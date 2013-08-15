#usage : make all -f capmake
PROJECT_NAME=cap
DEFS+=-D_GNU_SOURCE=1 -D_REENTRANT
INCLUDES+=-I/usr/include/SDL -I/usr/include
#-I$(INCLUDE_DIR)/libruputils
LIBS+=-lSDL -lSDL_ttf -lSDL_image 
#-lruputils
#-L$(LIB_DIR)/


ifdef CROSS_COMPILE
CPPFLAGS+=
#-DTARGET_ARM
endif

CPPFLAGS+=-g -Wall -DFSGPP_SHOW_IMMEDIATELY
#`sdl-config --cflags`
LDFLAGS+=-lSDL -lSDL_ttf -lSDL_image
#-wl, -Map,$(PROJECT_NAME).map
#-lts
ifdef MAKE_EUMAX01_MJPEG_SUPPORT
ifdef CROSS_COMPILE
LIBS+=../capture/libc6_dsp_jpeg.lib
CPPFLAGS+= -DC6COMPILE
else
LDFLAGS+=-ldsp_jpeg
endif
endif

OBJS+= Version.h v4l_capture.o LL.o Screen.o Main.o Tools.o Globals.o \
	Event.o Button.o Poll.o iniParser.o dsp_color.o Label.o \
	Dialoge.o Rezept.o MBProt.o Protocol.o ArbeitsDialog.o NewDirectDialog.o \
	ErrorDialog.o CamCtrl.o V4L2_utils.o InfoDialog.o MenuBar.o FadenkreuzDialog.o \
	WalzeDurchmesser.o AbfrageDialog.o
#dsp_jpeg.o
#utils.o color.o

include $(MAKE_DIR)/globalpp.mak

#make clean -f capmake&&make -f capmake get_v4l_capture&&make -f capmake&&make public -f capmake

#allarm: clean get_v4l_capture
#all:	make -f capmake clean make -f capmake get_v4l_caputure make -f capmake
alles:	clean get_v4l_capture $(EXE)

get_v4l_capture:
	cp ../capture/v4l_capture.o .

Version.h:
	./makeVersion.sh

public:
	cp ${PROJECT_NAME} ${ELDK_FS}/usr/work/${PROJECT_NAME}/${PROJECT_NAME}
	cp Poll.cpp ${ELDK_FS}/usr/work/${PROJECT_NAME}/Poll.cpp
	cp Main.cpp ${ELDK_FS}/usr/work/${PROJECT_NAME}/Main.cpp
	cp Event.cpp ${ELDK_FS}/usr/work/${PROJECT_NAME}/Event.cpp
	cp Screen.cpp ${ELDK_FS}/usr/work/${PROJECT_NAME}/Screen.cpp
	cp LL.cpp ${ELDK_FS}/usr/work/${PROJECT_NAME}/LL.cpp
	cp Button.cpp ${ELDK_FS}/usr/work/${PROJECT_NAME}/Button.cpp
	cp Globals.cpp ${ELDK_FS}/usr/work/${PROJECT_NAME}/Globals.cpp
	cp cap.cpp ${ELDK_FS}/usr/work/${PROJECT_NAME}/cap.cpp

links:
	ln -fs ../fsgpp/LL.cpp
	ln -fs ../fsgpp/LL.h
	ln -fs ../fsgpp/Screen.cpp
	ln -fs ../fsgpp/Screen.h
	ln -fs ../fsgpp/Main.cpp
	ln -fs ../fsgpp/Main.h
	ln -fs ../fsgpp/Tools.cpp
	ln -fs ../fsgpp/Tools.h
	ln -fs ../fsgpp/Globals.cpp
	ln -fs ../fsgpp/Globals.h
	ln -fs ../fsgpp/Event.cpp
	ln -fs ../fsgpp/Event.h
	ln -fs ../fsgpp/Button.cpp
	ln -fs ../fsgpp/Button.h
	ln -fs ../fsgpp/Label.cpp
	ln -fs ../fsgpp/Label.h
	ln -fs ../fsgpp/Poll.cpp
	ln -fs ../fsgpp/Poll.h
	ln -fs ../fsgpp/v4l_capture.c
	ln -fs ../fsgpp/v4l_capture.h
	cp ../fsgpp/aispec.ttf .