#include <stdio.h>
#include "../file_data.h"

file_data::file_data()
{
	buffer = nullptr;
	length = 0;
}

bool load_file(const char* path, file_data& data)
{
	data.buffer = nullptr;
	FILE* file = nullptr;//fopen(path, "wb+");
	if (fopen_s(&file, path, "rb+") != 0 ||
		file == nullptr)
	{
		return false;
	}

	bool result = true;
	do
	{
		fseek(file, 0, SEEK_END);
		data.length = ftell(file);
		if (data.length == 0)
		{
			result = false;
			break;
		}

		data.buffer = new unsigned char[data.length];
		if (data.buffer == nullptr)
		{
			data.length = 0;
			result = false;
			break;
		}
		fseek(file, 0, SEEK_SET);

		data.length = static_cast<long>(fread(data.buffer, 1, data.length, file));
		if (data.length == 0)
		{
			result = false;
			break;
		}
	} while (false);

	if (!result && data.buffer)
	{
		delete data.buffer;
		data.buffer = nullptr;
	}

	fclose(file);
	return result;
}

void destroy_file_data(file_data& data)
{
	if (data.buffer)
	{
		delete data.buffer;
		data.buffer = nullptr;
	}
	data.length = 0;
}