#include "image.h"
#include <stdlib.h>
bool read_bmp(unsigned char* file_data, img_data& output)
{
	output.has_alpha = true;
	output.width = 256;
	output.height = 256;
	if (output.has_alpha)
	{
		output.raw_data = new unsigned char[output.width * output.height * 4];
		for (int i = 0; i < output.height; i++)
		{
			for (int j = 0; j < output.width; j++)
			{
				output.raw_data[i * 4 * output.width + j * 4 + 0] = static_cast<unsigned char>(rand() * 255.0f / RAND_MAX);
				output.raw_data[i * 4 * output.width + j * 4 + 1] = static_cast<unsigned char>(rand() * 255.0f / RAND_MAX);
				output.raw_data[i * 4 * output.width + j * 4 + 2] = static_cast<unsigned char>(rand() * 255.0f / RAND_MAX);
				output.raw_data[i * 4 * output.width + j * 4 + 2] = static_cast<unsigned char>(rand() * 255.0f / RAND_MAX);
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
				output.raw_data[i * 3 * output.width + j * 3 + 0] = static_cast<unsigned char>(rand() * 255.0f / RAND_MAX);
				output.raw_data[i * 3 * output.width + j * 3 + 1] = static_cast<unsigned char>(rand() * 255.0f / RAND_MAX);
				output.raw_data[i * 3 * output.width + j * 3 + 2] = static_cast<unsigned char>(rand() * 255.0f / RAND_MAX);
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