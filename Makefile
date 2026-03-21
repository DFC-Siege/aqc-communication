PLATFORM ?= posix
BUILD_DIR ?= build-$(PLATFORM)

.PHONY: all build clean flash monitor posix esp32

all: build

posix:
	cmake -B build-posix -DPLATFORM=posix -G Ninja
	cmake --build build-posix

esp32:
	idf.py build

build:
	cmake -B $(BUILD_DIR) -DPLATFORM=$(PLATFORM) -G Ninja
	cmake --build $(BUILD_DIR)

clean:
	cmake --build $(BUILD_DIR) --target clean

clean-all:
	rm -rf build-*

flash:
	idf.py flash

monitor:
	idf.py monitor

flash-monitor:
	idf.py flash monitor
