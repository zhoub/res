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