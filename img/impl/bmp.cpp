#include "../img_data.h"
#include "inner_include.h"

#pragma pack(push)
#pragma pack(1)
typedef struct tagBITMAPFILEHEADER {
	uint16	MagicBM;	//0x4D42
	uint32	FileSize;	// whole file size.
	uint16	Reserved1;
	uint16	Reserved2;
	uint32	DataOffset;	// in bytes. rgbÊı¾İÆ«ÒÆÖµ¡£
} BitmapFileHead;

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
#pragma pack(pop)

constexpr const uint16 BMP_MAGIC_NUM = 0x4D42;
constexpr const int SIZE_BMP_FILE_HEAD = sizeof(BitmapFileHead);
constexpr const int SIZE_BMP_INFO_HEAD = sizeof(BitmapInfoHead);

bool is_bmp(unsigned char* file_data)
{
	BitmapFileHead* fileHead = (BitmapFileHead*)file_data;
	if (fileHead->MagicBM != BMP_MAGIC_NUM)
	{
		return false;
	}

	BitmapInfoHead* infoHead = (BitmapInfoHead*)(file_data + SIZE_BMP_FILE_HEAD);
	if ((infoHead->DataBits != 24 && infoHead->DataBits != 32) ||
		infoHead->Format != 0 ||
		infoHead->Plane != 1)
	{
		return false;
	}
	return true;
}

bool read_bmp(unsigned char* file_data, img_data& output)
{
	if (!is_bmp(file_data))
	{
		return false;
	}

	BitmapFileHead* fileHead = (BitmapFileHead*)file_data;
	BitmapInfoHead* infoHead = (BitmapInfoHead*)(file_data + SIZE_BMP_FILE_HEAD);

	output.has_alpha = infoHead->DataBits == 32;
	output.width = infoHead->Width;
	output.height = infoHead->Height;
	auto dataPtr = (file_data + fileHead->DataOffset);
	auto pitch = line_size(infoHead->Width, infoHead->DataBits);
	create_img_data(output);
	if (output.has_alpha)
	{
		for (int i = 0; i < output.height; i++)
		{
			for (int j = 0; j < output.width; j++)
			{
				auto srcPtr = dataPtr + i * pitch + j * 4;
				auto dstPtr = output.raw_data + 4 * (i * output.width + j);
				dstPtr[2] = srcPtr[0];
				dstPtr[1] = srcPtr[1];
				dstPtr[0] = srcPtr[2];
				dstPtr[3] = srcPtr[3];
			}
		}
	}
	else
	{
		for (int i = 0; i < output.height; i++)
		{
			for (int j = 0; j < output.width; j++)
			{
				auto srcPtr = dataPtr + i * pitch + j * 3;
				auto dstPtr = output.raw_data + 3 * (i * output.width + j);
				dstPtr[2] = srcPtr[0];
				dstPtr[1] = srcPtr[1];
				dstPtr[0] = srcPtr[2];
			}
		}
	}
	return true;
}