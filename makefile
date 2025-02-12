make:
	@echo "Makefile not setup yet"

synth:
	mkdir -p ./build
	g++ -o ./build/standalone_synth ./synthetiser/synth.cpp -l asound -l X11 -std=c++20

run_synth:
	./build/standalone_synth
