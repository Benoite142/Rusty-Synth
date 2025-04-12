CPPFLAGS = -std=c++20
LIBFLAGS = -lX11 -lasound
synthObjects = ./backend/synthetiser/*.cpp ./backend/synthetiser/oscillator/*.cpp ./backend/sound_player/*.cpp ./backend/midi/*.cpp ./backend/synthetiser/envelope/*.cpp ./backend/synthetiser/operator/*.cpp ./backend/synthetiser/low_frequency_oscillator/*.cpp ./backend/wav_writer/*.cpp ./backend/synthetiser/filters/*.cpp
utils = ./backend/utils/*.cpp
synth_main_files = ./backend/keyboard_sniffer/sniffer.cpp ./backend/controller/*.cpp ./backend/messager/*.cpp ./backend/booter/main.cpp 

synth:
	mkdir -p ./build
	@echo this build is deprecated 
	# g++ $(CPPFLAGS) -o ./build/standalone_synth $(synthObjects) $(LIBFLAGS)

run_synth:
	./build/standalone_synth

build: clean_build
	mkdir -p ./build
	g++ $(CPPFLAGS) -o ./build/app $(synthObjects) $(utils) $(synth_main_files) $(LIBFLAGS)

clang:
	mkdir -p ./build
	clang++ -Wall $(CPPFLAGS) $(LIBFLAGS) -o ./build/app ./backend/keyboard_sniffer/sniffer.cpp ./backend/booter/main.cpp

release: clean_release
	mkdir ./release

	# front end
	cd interface && npm run package
	# catching the app
	cp ./interface/release_build/*.AppImage ./release/synth-interface.AppImage

	# booter
	cp ./release_src_files/booter.sh ./release/boot
	cp ./release_src_files/README.md ./release/

	# back end
	g++ $(CPPFLAGS) -O3 -o ./release/synth-logic $(synthObjects) $(utils) $(synth_main_files)  $(LIBFLAGS)

run:
	./build/app

clean_build:
	rm -rf ./build

clean_release:
	rm -rf ./release
