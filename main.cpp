/*
 * Lucky7-Decoder -  A small C++ program for decoding images from the Lucky-7 cubesat
 * Copyright (C) 2021 Xerbo (xerbo@protonmail.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <vector>

#define FRAME_SIZE 35
#define FRAME_DATA_SIZE 28

struct Frame {
	uint16_t Counter;
	uint16_t Length;
	uint8_t Data[FRAME_DATA_SIZE];
};

struct Telemetry {
	std::string Callsign;
	std::string SatelliteName;
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

Frame parseFrame(uint8_t *in) {
	Frame frame;
	frame.Counter  = in[1] << 8 | in[2];
	frame.Length   = in[5] << 8 | in[6];
	std::memcpy(frame.Data, &in[7], FRAME_DATA_SIZE);

	return frame;
}

void parseTelemetry(Frame in) {
	Telemetry telm;

	telm.Callsign.append((char *)&in.Data[0], (char *)&in.Data[5]);
	telm.SatelliteName.append((char *)&in.Data[5], (char *)&in.Data[11]);
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
	std::cout << "  Callsign: " << telm.Callsign << std::endl;
	std::cout << "  Satellite Name: " << telm.SatelliteName << std::endl;
	std::cout << "  Total Reset Counter: " << (int)telm.TotalResetCounter << std::endl;
	std::cout << "  Swap Reset Counter: " << (int)telm.SwapResetCounter << std::endl;
	std::cout << "  Battery Voltage: " << telm.BatteryVoltage * 0.02f << "V" << std::endl;
	std::cout << "  MCU Temperature: " << (int)telm.MCUTemperature << "°C" << std::endl;
	std::cout << "  PA Temperature: " << (int)telm.PATemperature << "°C" << std::endl;
	std::cout << "  Processor Current: " << (int)telm.ProcessorCurrent << "mA" << std::endl;
	std::cout << "  MCU Voltage 3V3: " << telm.MCUVoltage3V3 * 0.02f << "V" << std::endl;
	std::cout << "  MCU Voltage 1V2: " << telm.MCUVoltage1V2 * 0.02f << "V" << std::endl;
	std::cout << "  Angular Rate X Axis: " << (telm.AngularRateXAxis == 2001 ? "Gyro Off" : std::to_string(telm.AngularRateXAxis)) << std::endl;
	std::cout << "  Angular Rate Y Axis: " << (telm.AngularRateYAxis == 2001 ? "Gyro Off" : std::to_string(telm.AngularRateYAxis)) << std::endl;
	std::cout << "  Angular Rate Z Axis: " << (telm.AngularRateZAxis == 2001 ? "Gyro Off" : std::to_string(telm.AngularRateZAxis)) << std::endl;
	std::cout << "  Antenna Burnwire: " << (telm.AntennaBurnwire ? "Enabled" : "Disabled") << std::endl << std::endl;
}

int main(int argc, char *argv[]) {
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

	// Up to 56kb of image data
	Frame image_frames[2000];
	size_t length = 0;

	while (!data_in.eof()) {
		uint8_t buffer[FRAME_SIZE];
		data_in.read((char *)buffer, FRAME_SIZE);

		Frame frame = parseFrame(buffer);

		if (frame.Counter == 0){
			parseTelemetry(frame);
		}
		if (frame.Counter >= 0xC000) {
			image_frames[frame.Counter - 0xC000] = frame;
			length = frame.Length;
		}
	}

	for (size_t i = 0; i < length; i++) {
		data_out.write((char *)&image_frames[i].Data, FRAME_DATA_SIZE);
	}

	data_in.close();
	data_out.close();

	return 0;
}
