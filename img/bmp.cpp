#include "image.h"
#include <stdlib.h>


#pragma once

#pragma pack(push)
#pragma pack(1)

namespace
{
	typedef char int8;
	typedef short int16;
	typedef int int32;
	typedef unsigned char uint8;
	typedef unsigned short uint16;
	typedef unsigned int uint32;

	constexpr const int BMP_FOURCC = 0x4D42;
	typedef struct tagBITMAPFILEHEADER {
		uint16	MagicBM;	//0x4D42
		uint32	FileSize;	// whole file size.
		uint16	Reserved1;
		uint16	Reserved2;
		uint32	DataOffset;	// in bytes. rgbÊý¾ÝÆ«ÒÆÖµ¡£
	} BitmapFileHead;
	constexpr const int SIZE_FILE_HEAD = sizeof(BitmapFileHead);
	typedef struct tagBITMAPINFOHEADER {
		uint32	InfoSize;	// =sizeof(BitmapInfoHead) =sizeof(this)
		int32	Width;
		int32	Height;
		uint16	Plane;		// always = 1, compare to other format like yuv, i420, etc.
		uint16	DataBits;	// normally use 24or32, 1, 4 is special used for compressed.
		uint32	Format;		// must be 0 for uncompressed here.
		uint32	DataSize;	// raw data size.
		int32	PPMX;
		int32	PPMY;
		uint32	ClrUsed;
		uint32	ClrImportant;
	} BitmapInfoHead;
	constexpr const int SIZE_INFO_HEAD = sizeof(BitmapInfoHead);

	typedef struct tagColor3
	{
		uint8 B;
		uint8 G;
		uint8 R;
	} Color3;

	typedef struct tagColor4
	{
		uint8 A;
		uint8 B;
		uint8 G;
		uint8 R;
	} Color4;

#pragma pack(pop)


	inline int32 LineSize(int32 width, int32 bits) { return (width * bits + 31) / 32 * 4; }
}

bool read_bmp(unsigned char* file_data, img_data& output)
{
	BitmapFileHead* fileHead = (BitmapFileHead*)file_data;
	if (fileHead->MagicBM != BMP_FOURCC)
	{
		return false;
	}
	BitmapInfoHead* infoHead = (BitmapInfoHead*)(file_data + SIZE_FILE_HEAD);
	if ((infoHead->DataBits != 24 && infoHead->DataBits != 32) ||
		infoHead->Format != 0 ||
		infoHead->Plane != 1)
	{
		return false;
	}

	auto pitch = LineSize(infoHead->Width, infoHead->DataBits);
	output.has_alpha = infoHead->DataBits == 32;
	output.width = infoHead->Width;
	output.height = infoHead->Height;
	auto dataPtr = (file_data + fileHead->DataOffset);
	if (output.has_alpha)
	{
		output.raw_data = new unsigned char[output.width * output.height * 4];
		for (int i = 0; i < output.height; i++)
		{
			for (int j = 0; j < output.width; j++)
			{
				auto srcPtr = dataPtr + i * pitch + j * 4;
				auto dstPtr = output.raw_data + 4 * (i * output.width + j);
				dstPtr[0] = srcPtr[0];
				dstPtr[1] = srcPtr[1];
				dstPtr[2] = srcPtr[2];
				dstPtr[3] = srcPtr[3];
			}
		}
	}
	else
	{
		output.raw_data = new unsigned char[output.width * output.height * 3];

		for (int i = 0; i < output.height; i++)
		{
			for (int j = 0; j < output.width; j++)
			{
				auto srcPtr = dataPtr + i * pitch + j * 3;
				auto dstPtr = output.raw_data + 3 * (i * output.width + j);
				dstPtr[0] = srcPtr[0];
				dstPtr[1] = srcPtr[1];
				dstPtr[2] = srcPtr[2];
			}
		}
	}


	return true;
}

img_data::img_data()
{
	has_alpha = false;
	width = 0;
	height = 0;
	raw_data = nullptr;
}
void destroy_img_data(img_data& output)
{
	output.has_alpha = false;
	output.width = 0;
	output.height = 0;
	if (output.raw_data)
	{
		delete[] output.raw_data;
		output.raw_data = nullptr;
	}
}