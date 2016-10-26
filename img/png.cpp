#include "img_data.h"
#include "inner_include.h"
#include "../zlib/zlib.h"
#include <memory.h>
#include <assert.h>
#pragma pack(push)
#pragma pack(1)
enum PixelFormat
{
	GRAYSCALE = 0,
	RGB = 2,
	INDEX = 3,
	GRAY_ALPHA = 4,
	RGBA = 6,
};

enum FilterMethod
{
	None = 0,
	Sub = 1,
	Up = 2,
	Average = 3,
	Paeth = 4,
};
typedef struct tagPNGFileHead
{
	uint32 MagicPNG;	//0x89+'PNG'
	uint32 MagicCRLF;	//CRLF
} PNGFileHead;

typedef struct tagPNGChunkHead
{
	uint32 Length;
	uint32 Type;
} PNGChunkHead;

typedef struct tagPNGChunkIHDR
{
	uint32 Width;
	uint32 Height;
	uint8  EachColorBits;
	uint8  Format;
	uint8  Compression;
	uint8  Filter;
	uint8  Interlace;

} PNGChunkIHDR;

#pragma pack(pop)

constexpr const uint32 PNG_MAGIC_NUM = 0x474E5089;
constexpr const uint32 CRLF_MAGIC_NUM = 0x0A1A0A0D;
constexpr const int SIZE_PNG_FILE_HEAD = sizeof(PNGFileHead);
constexpr const int SIZE_PNG_CHUNK_HEAD = sizeof(PNGChunkHead);
constexpr const int SIZE_PNG_CHUNK_CRC = sizeof(uint32);

constexpr uint32 MakeFourCC(const char fourcc[5])
{
	return (((((fourcc[3] << 8) | fourcc[2]) << 8) | fourcc[1]) << 8) | fourcc[0];
};
#define FourCC(x) constexpr const uint32 CHUNK_##x = MakeFourCC(#x);
FourCC(IHDR);
FourCC(PLTE);
FourCC(IDAT);
FourCC(IEND);
#undef FourCC
uint32 SwapEndian(uint32 u)
{
	return ((u & 0xFF) << 24) | ((u & 0xFF00) << 8) | ((u & 0xFF0000) >> 8) | ((u & 0xFF000000) >> 24);
}
#include <math.h>
#include <stdlib.h>
unsigned char PaethPredictor(unsigned char a, unsigned char b, unsigned char c)
{
	auto p = a + b - c;
	auto pa = abs(p - a);
	auto pb = abs(p - b);
	auto pc = abs(p - c);

	if (pa <= pb && pa <= pc)
		return a;

	if (pb <= pc)
		return b;

	return c;
}

bool read_png(unsigned char* file_data, img_data& output)
{
	PNGFileHead* fileHead = (PNGFileHead*)file_data;
	if (fileHead->MagicPNG != PNG_MAGIC_NUM ||
		fileHead->MagicCRLF != CRLF_MAGIC_NUM)
		return false;

	auto dataPtr = file_data + SIZE_PNG_FILE_HEAD;
	PNGChunkHead* chunkHead = (PNGChunkHead*)dataPtr;
	dataPtr += SIZE_PNG_CHUNK_HEAD;

	if (chunkHead->Type != CHUNK_IHDR)
	{
		return false;
	}


	PNGChunkIHDR* IHDR = (PNGChunkIHDR*)dataPtr;
	if (IHDR->EachColorBits != 8 ||
		IHDR->Compression != 0 ||
		IHDR->Filter != 0 ||
		IHDR->Interlace != 0 ||
		(IHDR->Format != RGB && IHDR->Format != RGBA))
	{
		return false;
	}

	bool found = false;
	while (chunkHead->Type != CHUNK_IEND)
	{
		dataPtr += SwapEndian(chunkHead->Length) + SIZE_PNG_CHUNK_CRC;
		chunkHead = (PNGChunkHead*)dataPtr;
		dataPtr += SIZE_PNG_CHUNK_HEAD;
		if (chunkHead->Type == CHUNK_IDAT)
		{
			found = true;
			break;
		}

	}

	if (!found)
		return false;

	output.has_alpha = IHDR->Format == RGBA;
	output.width = SwapEndian(IHDR->Width);
	output.height = SwapEndian(IHDR->Height);
	create_img_data(output);

	unsigned long dataLength = SwapEndian(chunkHead->Length);
	unsigned long decodedDataLength = dataLength * 4;
	unsigned char* decodedData = new unsigned char[decodedDataLength];

	if (Z_OK != uncompress(decodedData, &decodedDataLength, dataPtr, dataLength))
	{
		destroy_img_data(output);
		delete[]decodedData;
		return false;
	}

	int pitch = decodedDataLength / output.height;
	int color_space = output.has_alpha ? 4 : 3;
	int* reconA = new int[color_space];
	int dstPitch = output.width * color_space;
	for (int i = 0; i < output.height; i++)
	{
		unsigned char* srcPtr = decodedData + pitch * i;
		auto filterType = *(srcPtr++);
		int invertY = output.height - i - 1;
		unsigned char* dstPtr = output.raw_data + dstPitch * invertY;
		unsigned char* reconB = output.raw_data + dstPitch * (invertY + 1);
		if (filterType == None)
		{
			memcpy(dstPtr, srcPtr, output.width * color_space);
		}
		else if (filterType == Sub)
		{
			for (int cp = 0; cp < color_space; cp++)
			{
				reconA[cp] = 0;
			}

			for (int j = 0; j < output.width; j++)
			{
				for (int cp = 0; cp < color_space; cp++)
				{
					int offset = j * color_space + cp;
					dstPtr[offset] = reconA[cp] = (srcPtr[offset] + reconA[cp]) % 256;
				}
			}
		}
		else if (filterType == Up)
		{
			for (int j = 0; j < output.width; j++)
			{
				for (int cp = 0; cp < color_space; cp++)
				{
					int offset = j * color_space + cp;
					dstPtr[offset] = reconA[cp] = (srcPtr[offset] + reconB[cp]) % 256;
				}
			}
		}
		else if (filterType == Average)
		{
			for (int cp = 0; cp < color_space; cp++)
			{
				reconA[cp] = 0;
			}

			for (int j = 0; j < output.width; j++)
			{
				for (int cp = 0; cp < color_space; cp++)
				{
					int offset = j * color_space + cp;
					dstPtr[offset] = reconA[cp] = (srcPtr[offset] + (reconA[cp] + reconB[offset]) / 2) % 256;
				}
			}
		}
		else if (filterType == Paeth)
		{
			for (int cp = 0; cp < color_space; cp++)
			{
				reconA[cp] = srcPtr[cp];
				if (i != 0)
				{
					reconA[cp] += PaethPredictor(0, reconB[cp], 0);
				}
				dstPtr[cp] = reconA[cp];
			}

			for (int j = 1; j < output.width; j++)
			{
				for (int cp = 0; cp < color_space; cp++)
				{
					int offset = j * color_space + cp;
					dstPtr[offset] = reconA[cp] = (srcPtr[offset] + PaethPredictor(reconA[cp], reconB[offset], reconB[offset - color_space])) % 256;
				}
			}
		}
		else
		{
			assert(false);
		}
	}

	delete[] reconA;
	delete[]decodedData;
	return true;
}