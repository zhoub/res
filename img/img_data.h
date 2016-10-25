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

bool read_bmp(unsigned char* file_data, img_data& output);
bool read_png(unsigned char* file_data, img_data& output);
bool read_tga(unsigned char* file_data, img_data& output);
void destroy_img_data(img_data& output);