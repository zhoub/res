#pragma once

struct file_data
{
	file_data();

	unsigned char* buffer;
	long length;
};

bool load_file(const char* path, file_data& data);
void destroy_file_data(file_data& data);