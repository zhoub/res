#pragma once

struct img_data
{
	img_data();
	unsigned char* raw_data;
	int data_length;
	int width;
	int height;
	bool has_alpha;
};

//only support limited bmp/tga/png
bool read_image(unsigned char* file_data, img_data& output);
void destroy_img_data(img_data& output);