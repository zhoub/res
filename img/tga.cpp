#include "img_data.h"
#include "inner_include.h"

enum ImageType
{
	NO_IMAGE = 0,			//colormap:no  encoding:no
	INDEX = 1,			//colormap:yes encoding:no
	TRUECOLOR = 2,			//colormap:no  encoding:no
	MONOCHROME = 3,			//colormap:no  encoding:no
	ENCODED_INDEX = 9,	//colormap:yes encoding:yes
	ENCODED_TRUECOLOR = 10,	//colormap:no  encoding:yes
	ENCODED_MONOCHROME = 11,//colormap:no  encoding:yes
};
#pragma pack(push)
#pragma pack(1)
typedef struct tagTGAFileHead
{
	uint8 IDLength;        /* 00h  Size of Image ID field */
	uint8 ColorMapType;    /* 01h  Color map type */
	uint8 ImageType;       /* 02h  Image type code */
	uint16 CMapStart;       /* 03h  Color map origin */
	uint16 CMapLength;      /* 05h  Color map length */
	uint8 CMapDepth;       /* 07h  Depth of color map entries */
	uint16 XOffset;         /* 08h  X origin of image */
	uint16 YOffset;         /* 0Ah  Y origin of image */
	uint16 Width;           /* 0Ch  Width of image */
	uint16 Height;          /* 0Eh  Height of image */
	uint8 PixelDepth;      /* 10h  Image pixel size */
	uint8 ImageDescriptor; /* 11h  Image descriptor byte */
} TGAFileHead;
constexpr const int SIZE_TGA_FILE_HEAD = sizeof(TGAFileHead);
#pragma pack(pop)

bool read_tga(unsigned char* file_data, img_data& output)
{
	TGAFileHead* fileHead = (TGAFileHead*)file_data;
	if (fileHead->ImageType != TRUECOLOR ||
		fileHead->ColorMapType != 0 ||
		fileHead->CMapLength != 0 ||
		fileHead->XOffset != 0 ||
		fileHead->YOffset != 0 ||
		(fileHead->PixelDepth != 24 && fileHead->PixelDepth != 32))
	{
		return false;
	}

	output.has_alpha = fileHead->PixelDepth == 32;
	output.width = fileHead->Width;
	output.height = fileHead->Height;
	create_img_data(output);
	auto pitch = line_size(fileHead->Width, fileHead->PixelDepth);
	auto dataPtr = file_data + SIZE_TGA_FILE_HEAD + fileHead->IDLength;
	if (output.has_alpha)
	{
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