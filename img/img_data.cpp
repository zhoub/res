#include "img_data.h"
#include "inner_include.h"

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

void create_img_data(img_data& img)
{
	img.data_length = img.has_alpha ? img.width * img.height * 4 : img.width * img.height * 3;
	img.raw_data = new unsigned char[img.data_length];
}

bool read_image(unsigned char* file_data, img_data& output)
{
	if (is_bmp(file_data))
	{
		return read_bmp(file_data, output);
	}
	else if (is_tga(file_data))
	{
		return read_tga(file_data, output);
	}
	else if (is_png(file_data))
	{
		return read_png(file_data, output);
	}
	return false;
}