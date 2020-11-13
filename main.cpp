#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <vector>
#include <bits/stdc++.h>

// Frames are 272 bits if they came from a hardware demod
#define FRAME_SIZE_BITS 280
#define FRAME_SIZE_BYTES (FRAME_SIZE_BITS/8)
#define FRAME_DATA_SIZE 28

typedef struct {
	uint16_t Address;
	uint16_t Counter;
	uint16_t Length;
	uint8_t Data[FRAME_DATA_SIZE];
} frame_t;

frame_t ParseFrame(uint8_t *in){
	frame_t frame;
	frame.Address  = in[0] << 8 | in[1];
	frame.Counter  = in[2] << 8 | in[3];
	frame.Length   = in[4] << 8 | in[5];
	std::memcpy(frame.Data, &in[6], FRAME_DATA_SIZE);
	return frame;
}

int main(int argc, char **argv) {
	if(argc != 3) {
		std::cout << "Usage: " << argv[0] << " input.bin output.jpg" << std::endl;
		return 1;
	}
	std::ifstream data_in(argv[1], std::ios::binary);
	if(!data_in){
		std::cout << "Could not open input file" << std::endl;
		return 1;
	}
	std::ofstream data_out(argv[2], std::ios::binary);
	if(!data_out){
		std::cout << "Could not open output file" << std::endl;
		return 1;
	}

	uint8_t *Buffer = new uint8_t[FRAME_SIZE_BYTES];
	std::vector<frame_t> Frames;
	while (!data_in.eof()) {
		data_in.read((char *)Buffer, FRAME_SIZE_BYTES);
		// gr-satellites outputs with a byte offset (against the actual hardware demod) for some reason
		Frames.push_back(ParseFrame(&Buffer[1]));
	}

	// Behold, the worlds most horrible sorting algorithm
	std::vector<frame_t> SortedFrames;
	int FrameLen = Frames.size();
	for(int j = 0; j < FrameLen; j++){
		int Smallest = INT_MAX; int SmallestIndice = 0;
		for(int i = 0; i < (int)Frames.size(); i++) {
			if(Frames[i].Counter < Smallest){
				Smallest = Frames[i].Counter;
				SmallestIndice = i;
			}
		}

		SortedFrames.push_back(Frames[SmallestIndice]);
		Frames.erase(Frames.begin()+SmallestIndice);
	}

	// Actually write the data, skipping duplicate frames
	int LastFrameCounter = -1;
	for(const frame_t& frame: SortedFrames) {
		if(frame.Address < 0xC000) continue;
		if(frame.Address >= 0xC000 + frame.Length) break;

		if(frame.Counter != LastFrameCounter) {
			data_out.write((char *)frame.Data, FRAME_DATA_SIZE);
		}
		LastFrameCounter = frame.Counter;
	}

	return 0;
}
