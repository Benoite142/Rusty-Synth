CPPFLAGS = -std=c++20
LIBFLAGS = -lX11 -lasound
synthObjects = ./backend/synthetiser/*.cpp ./backend/synthetiser/oscillator/*.cpp ./backend/sound_player/*.cpp
utils = ./backend/utils/*.cpp

synth:
	mkdir -p ./build
	@echo this build is deprecated 
	# g++ $(CPPFLAGS) -o ./build/standalone_synth $(synthObjects) $(LIBFLAGS)

run_synth:
	./build/standalone_synth

build:
	mkdir -p ./build
	g++ $(CPPFLAGS) -o ./build/app $(synthObjects) $(utils) ./backend/keyboard_sniffer/sniffer.cpp ./backend/booter/main.cpp $(LIBFLAGS)

clang:
	mkdir -p ./build
	clang++ -Wall $(CPPFLAGS) $(LIBFLAGS) -o ./build/app ./backend/keyboard_sniffer/sniffer.cpp ./backend/booter/main.cpp

run:
	./build/app

clean:
	rm -rf ./build
