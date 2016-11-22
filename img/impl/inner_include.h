#pragma once

namespace
{
	typedef char int8;
	typedef short int16;
	typedef int int32;
	typedef unsigned char uint8;
	typedef unsigned short uint16;
	typedef unsigned int uint32;
}
struct img_data;

void create_img_data(img_data& data);
inline int32 line_size(int32 width, int32 bits) { return (width * bits + 31) / 32 * 4; }

bool is_bmp(unsigned char* file_data);
bool is_png(unsigned char* file_data);
bool is_tga(unsigned char* file_data);
bool read_bmp(unsigned char* file_data, img_data& output);
bool read_png(unsigned char* file_data, img_data& output);
bool read_tga(unsigned char* file_data, img_data& output);