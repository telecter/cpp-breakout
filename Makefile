all:
	g++ $(pkg-config --cflags sfml-all) -o game game.cpp \
	-lsfml-graphics-s \
	-lsfml-window-s \
	-lsfml-audio-s \
	-lsfml-network-s \
	-lsfml-system-s \
	-lX11 \
	-lXcursor \
	-lXrandr \
	-ludev \
	-lXi \
	-lfreetype \