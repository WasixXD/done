# https://stackoverflow.com/questions/71213580/cant-get-v8-monolith-to-genorate

CC = ccache g++
FLAGS = -Iv8/include -Ilibuv/include -Llibuv/build -Lv8/out.gn/x64.release/ -fno-rtti -pthread -lv8 -lv8_libbase -lv8_libplatform -luv -ldl -std=c++20 -DV8_COMPRESS_POINTERS -DV8_ENABLE_SANDBOX

.PHONY = all build run 

all: build run

build: 
	$(CC) -o done src/* $(FLAGS)

run: build
	./done test.js