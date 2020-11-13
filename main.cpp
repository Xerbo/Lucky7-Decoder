#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <vector>
#include <bits/stdc++.h>

#define FRAME_SIZE_BITS 280
#define FRAME_SIZE_BYTES (FRAME_SIZE_BITS/8)
#define FRAME_DATA_SIZE 28

typedef struct {
	uint8_t Type;
	uint16_t Counter;
	uint16_t Length;
	uint8_t Data[FRAME_DATA_SIZE];
} frame_t;

frame_t ParseFrame(uint8_t *in){
	frame_t frame;
	frame.Type     = in[0];
	frame.Counter  = in[1] << 8 | in[2];
	frame.Length   = in[5] << 8 | in[6];
	std::memcpy(frame.Data, &in[7], FRAME_DATA_SIZE);
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
		Frames.push_back(ParseFrame(Buffer));
	}

	// Behold, this ungodly mess of a sorting algorithm
	std::vector<frame_t> SortedFrames;
	int FramesSize = Frames.size();
	for(int j = 0; j < FramesSize; j++){
		int Smallest = INT_MAX; int SmallestIndice = 0;
		for(int i = 0; i < (int)Frames.size(); i++) {
			if(Frames[i].Counter < Smallest){
				Smallest = Frames[i].Counter;
				SmallestIndice = i;
			}
		}

		SortedFrames.push_back(Frames[SmallestIndice]);
		Frames.erase(Frames.begin() + SmallestIndice);
	}

	// Actually write the data, skipping duplicate frames
	for(int i = 0; i < (int)SortedFrames.size(); i++) {
		if(SortedFrames[i].Counter == SortedFrames[i+1].Counter) continue;

		// We probably should check when the image transmission ends here, but unless you somehow get 2 images in a single pass that doesn't matter
		if(SortedFrames[i].Type == 0x80 && SortedFrames[i].Counter >= 0xC000){
			data_out.write((char *)SortedFrames[i].Data, FRAME_DATA_SIZE);
		}
	}

	return 0;
}
