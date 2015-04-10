# OpenJazz makefile

OBJS = \
	openjazz/src/game/clientgame.o openjazz/src/game/game.o openjazz/src/game/gamemode.o \
	openjazz/src/game/localgame.o openjazz/src/game/servergame.o \
	openjazz/src/io/gfx/anim.o openjazz/src/io/gfx/font.o openjazz/src/io/gfx/paletteeffects.o \
	openjazz/src/io/gfx/sprite.o openjazz/src/io/gfx/video.o \
	openjazz/src/io/gfx/scale2x/getopt.o openjazz/src/io/gfx/scale2x/pixel.o \
	openjazz/src/io/gfx/scale2x/scale2x.o openjazz/src/io/gfx/scale2x/scale3x.o \
	openjazz/src/io/gfx/scale2x/scalebit.o openjazz/src/io/gfx/scale2x/simple2x.o \
	openjazz/src/io/controls.o openjazz/src/io/file.o openjazz/src/io/network.o openjazz/src/io/sound.o \
	openjazz/src/jj1bonuslevel/jj1bonuslevelplayer/jj1bonuslevelplayer.o \
	openjazz/src/jj1bonuslevel/jj1bonuslevel.o \
	openjazz/src/jj1level/jj1event/jj1bridge.o openjazz/src/jj1level/jj1event/jj1event.o \
	openjazz/src/jj1level/jj1event/jj1guardians.o \
	openjazz/src/jj1level/jj1event/jj1standardevent.o \
	openjazz/src/jj1level/jj1levelplayer/jj1bird.o \
	openjazz/src/jj1level/jj1levelplayer/jj1levelplayer.o \
	openjazz/src/jj1level/jj1levelplayer/jj1levelplayerframe.o \
	openjazz/src/jj1level/jj1bullet.o openjazz/src/jj1level/jj1demolevel.o \
	openjazz/src/jj1level/jj1level.o openjazz/src/jj1level/jj1levelframe.o \
	openjazz/src/jj1level/jj1levelload.o \
	openjazz/src/jj1planet/jj1planet.o \
	openjazz/src/jj1scene/jj1scene.o openjazz/src/jj1scene/jj1sceneload.o \
	openjazz/src/jj2level/jj2event/jj2event.o openjazz/src/jj2level/jj2event/jj2eventframe.o \
	openjazz/src/jj2level/jj2levelplayer/jj2levelplayer.o \
	openjazz/src/jj2level/jj2levelplayer/jj2levelplayerframe.o \
	openjazz/src/jj2level/jj2layer.o openjazz/src/jj2level/jj2level.o \
	openjazz/src/jj2level/jj2levelframe.o openjazz/src/jj2level/jj2levelload.o \
	openjazz/src/level/level.o openjazz/src/level/movable.o openjazz/src/level/levelplayer.o \
	openjazz/src/menu/gamemenu.o openjazz/src/menu/mainmenu.o openjazz/src/menu/menu.o \
	openjazz/src/menu/plasma.o openjazz/src/menu/setupmenu.o \
	openjazz/src/player/player.o \
	openjazz/src/main.o openjazz/src/setup.o openjazz/src/util.o

FLAGS = -DUSE_SOCKETS -DUSE_MODPLUG


OpenJazz: $(OBJS)
	$(CXX) $(CXXFLAGS) -o OpenJazz $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(FLAGS) -Iopenjazz/src -c $< -o $@

clean:
	rm -f libapplication.so $(OBJS)
