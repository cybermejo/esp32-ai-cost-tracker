# Host-side verification builds (run on the Mac, no hardware needed).
CXXFLAGS = -std=c++17 -O2 -Isrc -Itest/shim -DUNIT_TEST
all: data agg input theme swap manager idle render
clean:
	rm -rf build
data: test/host_data.cpp src/data/SampleProvider.cpp
	@-mkdir -p build
	$(CXX) $(CXXFLAGS) test/host_data.cpp src/data/SampleProvider.cpp -o build/host_data
	./build/host_data
agg: test/host_agg.cpp src/data/SampleProvider.cpp src/data/Aggregator.cpp
	@-mkdir -p build
	$(CXX) $(CXXFLAGS) test/host_agg.cpp src/data/SampleProvider.cpp src/data/Aggregator.cpp -o build/host_agg
	./build/host_agg
input: test/host_input.cpp src/input/Gesture.cpp
	@-mkdir -p build
	$(CXX) $(CXXFLAGS) test/host_input.cpp src/input/Gesture.cpp -o build/host_input
	./build/host_input
render: test/host_render.cpp src/data/SampleProvider.cpp src/data/Aggregator.cpp src/ui/Theme.cpp src/ui/PieScene.cpp src/ui/ListScene.cpp src/ui/DetailScene.cpp src/ui/VersionScene.cpp src/ui/DotFont.cpp
	@-mkdir -p build
	$(CXX) $(CXXFLAGS) test/host_render.cpp src/data/SampleProvider.cpp src/data/Aggregator.cpp src/ui/Theme.cpp src/ui/PieScene.cpp src/ui/ListScene.cpp src/ui/DetailScene.cpp src/ui/VersionScene.cpp src/ui/DotFont.cpp -o build/host_render
	./build/host_render
theme: test/host_theme.cpp src/ui/Theme.cpp
	@-mkdir -p build
	$(CXX) $(CXXFLAGS) test/host_theme.cpp src/ui/Theme.cpp -o build/host_theme
	./build/host_theme
swap: test/host_swap.cpp src/ui/Theme.cpp
	@-mkdir -p build
	$(CXX) $(CXXFLAGS) test/host_swap.cpp src/ui/Theme.cpp -o build/host_swap
	./build/host_swap
manager: test/host_manager.cpp src/data/SampleProvider.cpp src/data/Aggregator.cpp src/ui/Theme.cpp src/ui/PieScene.cpp src/ui/ListScene.cpp src/ui/DetailScene.cpp src/ui/VersionScene.cpp src/ui/DotFont.cpp src/ui/SceneManager.cpp src/input/Gesture.cpp
	@-mkdir -p build
	$(CXX) $(CXXFLAGS) test/host_manager.cpp src/data/SampleProvider.cpp src/data/Aggregator.cpp src/ui/Theme.cpp src/ui/PieScene.cpp src/ui/ListScene.cpp src/ui/DetailScene.cpp src/ui/VersionScene.cpp src/ui/DotFont.cpp src/ui/SceneManager.cpp src/input/Gesture.cpp -o build/host_manager
	./build/host_manager
idle: test/host_idle.cpp
	@-mkdir -p build
	$(CXX) $(CXXFLAGS) test/host_idle.cpp -o build/host_idle
	./build/host_idle
