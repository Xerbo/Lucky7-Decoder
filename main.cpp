#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <vector>
#include <bits/stdc++.h>

#define FRAME_SIZE_BITS 280
#define FRAME_SIZE_BYTES (FRAME_SIZE_BITS/8)
#define FRAME_DATA_SIZE 28

struct Frame {
	uint8_t Type;
	uint16_t Counter;
	uint16_t Length;
	uint8_t Data[FRAME_DATA_SIZE];
};

struct Telemetry {
	char Callsign[7];
	char SatelliteName[7];
	uint16_t TotalResetCounter;
	uint16_t SwapResetCounter;
	uint8_t BatteryVoltage;
	int8_t MCUTemperature;
	int8_t PATemperature;
	uint8_t ProcessorCurrent;
	uint8_t MCUVoltage3V3;
	uint8_t MCUVoltage1V2;
	int16_t AngularRateXAxis;
	int16_t AngularRateYAxis;
	int16_t AngularRateZAxis;
	bool AntennaBurnwire;
};

Frame ParseFrame(uint8_t *in){
	Frame frame;
	frame.Type     = in[0];
	frame.Counter  = in[1] << 8 | in[2];
	frame.Length   = in[5] << 8 | in[6];
	std::memcpy(frame.Data, &in[7], FRAME_DATA_SIZE);
	return frame;
}

void ParseTelemetry(Frame in){
	Telemetry telm;

	telm.TotalResetCounter = in.Data[11] << 8 | in.Data[12];
	telm.SwapResetCounter = in.Data[13] << 8 | in.Data[14];
	telm.BatteryVoltage = in.Data[15];
	telm.MCUTemperature = in.Data[16];
	telm.PATemperature = in.Data[17];
	telm.ProcessorCurrent = in.Data[18];
	telm.MCUVoltage3V3 = in.Data[19];
	telm.MCUVoltage1V2 = in.Data[20];
	telm.AngularRateXAxis = in.Data[21] << 8 | in.Data[22];
	telm.AngularRateYAxis = in.Data[23] << 8 | in.Data[24];
	telm.AngularRateZAxis = in.Data[25] << 8 | in.Data[26];
	telm.AntennaBurnwire = in.Data[27] == 1;


	std::cout << "Telemetry Packet:" << std::endl;
	std::cout << "Callsign: " << telm.Callsign << std::endl;
	std::cout << "Satellite Name: " << telm.SatelliteName << std::endl;
	std::cout << "Total Reset Counter: " << (int)telm.TotalResetCounter << std::endl;
	std::cout << "Swap Reset Counter: " << (int)telm.SwapResetCounter << std::endl;
	std::cout << "Battery Voltage: " << telm.BatteryVoltage * 0.02f << "V" << std::endl;
	std::cout << "MCU Temperature: " << (int)telm.MCUTemperature << "°C" << std::endl;
	std::cout << "PA Temperature: " << (int)telm.PATemperature << "°C" << std::endl;
	std::cout << "Processor Current: " << (int)telm.ProcessorCurrent << "mA" << std::endl;
	std::cout << "MCU Voltage 3V3: " << telm.MCUVoltage3V3 * 0.02f << "V" << std::endl;
	std::cout << "MCU Voltage 1V2: " << telm.MCUVoltage1V2 * 0.02f << "V" << std::endl;

	std::cout << "Angular Rate X Axis: " << (telm.AngularRateXAxis == 2001 ? "Gyro Off" : std::to_string(telm.AngularRateXAxis)) << std::endl;
	std::cout << "Angular Rate Y Axis: " << (telm.AngularRateYAxis == 2001 ? "Gyro Off" : std::to_string(telm.AngularRateYAxis)) << std::endl;
	std::cout << "Angular Rate Z Axis: " << (telm.AngularRateZAxis == 2001 ? "Gyro Off" : std::to_string(telm.AngularRateZAxis)) << std::endl;
	std::cout << "Antenna Burnwire: " << (telm.AntennaBurnwire ? "Enabled" : "Disabled") << std::endl << std::endl;
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
	std::vector<Frame> Frames;
	while (!data_in.eof()) {
		data_in.read((char *)Buffer, FRAME_SIZE_BYTES);
		Frames.push_back(ParseFrame(Buffer));
		if(Frames[Frames.size()-1].Counter == 0x0000){
			ParseTelemetry(Frames[Frames.size()-1]);
		}
	}

	// Behold, this ungodly mess of a sorting algorithm
	std::vector<Frame> SortedFrames;
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
