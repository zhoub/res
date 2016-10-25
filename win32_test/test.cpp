
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

extern HWND hWindow;
namespace
{
	HDC hWindowDC;
	const int COLOR_BIT = 24;
}

HBITMAP CreateDIB(HDC dc, int width, int height, VOID** buffer)
{
	if (width <= 0 || height <= 0)
	{
		return NULL;
	}

	BITMAPINFO bmpInfo;
	ZeroMemory(&bmpInfo, sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biHeight = height;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = COLOR_BIT;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	HBITMAP bmp = ::CreateDIBSection(
		dc, &bmpInfo,
		DIB_RGB_COLORS,
		buffer,
		NULL, 0);

	return bmp;
}

struct BufferBMP
{
	HBITMAP	hCanvas = NULL;
	HDC		hDC = NULL;
	unsigned char*	pBuffer = NULL;
	int		Width = 0;
	int		Height = 0;
	int		Pitch = 0;

	bool Create(HDC dc, int width, int height)
	{
		hCanvas = CreateDIB(dc, width, height, (VOID**)&pBuffer);
		if (!hCanvas)
		{
			return false;
		}

		//´´½¨dc
		hDC = ::CreateCompatibleDC(dc);
		::SelectObject(hDC, hCanvas);

		Width = width;
		Height = height;
		Pitch = (width * COLOR_BIT + 31) / 32 * 4;
		return true;
	}

	void Clear()
	{
		for (int i = 0; i < Height; i++)
		{
			for (int j = 0; j < Width; j++)
			{
				int index = i * Pitch + j * 3;
				pBuffer[index + 0] = 100;
				pBuffer[index + 1] = 0;
				pBuffer[index + 2] = 0;

			}
		}
	}

	void Release()
	{
		pBuffer = NULL;

		::DeleteDC(hDC);
		hDC = NULL;

		DeleteObject(hCanvas);
		hCanvas = NULL;
	}
};
BufferBMP bufferBMP;

void CenterWindow(HWND hWnd)
{
	RECT clientRect;
	RECT windowRect;
	::GetClientRect(hWnd, &clientRect);
	::GetWindowRect(hWnd, &windowRect);


	int borderWidth = (windowRect.right - windowRect.left - clientRect.right) / 2;
	int borderHeight = (windowRect.bottom - windowRect.top - clientRect.bottom) / 2;

	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY), &mi);
	int centerX = (mi.rcWork.right + mi.rcWork.left) / 2;
	int centerY = (mi.rcWork.top + mi.rcWork.bottom) / 2;

	int halfWidth = bufferBMP.Width / 2 + borderWidth;
	int halfHeight = bufferBMP.Height / 2 + borderHeight;
	if (halfWidth > centerX)halfWidth = centerX;
	if (halfHeight > centerY)halfHeight = centerY;

	::SetWindowPos(hWnd, NULL,
		centerX - halfWidth,
		centerY - halfHeight,
		bufferBMP.Width + 2 * borderWidth,
		bufferBMP.Height + 2 * borderHeight, SWP_SHOWWINDOW);
}

#include <image.h>
bool Initialize(HWND hWnd)
{
	srand(static_cast<unsigned>(time(0)));
	hWindowDC = ::GetDC(hWindow);

	const char* fileName = "test.bmp";
	//const char* fileName = "test_alpha.bmp";
	FILE* imgFile = fopen(fileName, "rb+");
	if (imgFile == nullptr)
		return false;
	fseek(imgFile, 0, SEEK_END);
	unsigned long imgFileSize = ftell(imgFile);
	unsigned char* fileData = new unsigned char[imgFileSize];
	fseek(imgFile, 0, SEEK_SET);
	fread(fileData, 1, imgFileSize, imgFile);
	fclose(imgFile);

	img_data data;
	auto result = read_bmp(fileData, data);
	delete[] fileData;

	if (!result)
	{
		return false;
	}

	if (!bufferBMP.Create(hWindowDC, data.width, data.height))
	{
		destroy_img_data(data);
		return false;
	}

	if (data.has_alpha)
	{
		for (int i = 0; i < bufferBMP.Height; i++)
		{
			for (int j = 0; j < bufferBMP.Width; j++)
			{
				auto dstBuffer = bufferBMP.pBuffer + i * bufferBMP.Pitch + 3 * j;
				auto srcBuffer = data.raw_data + 4 * (data.width * i + j);

				dstBuffer[0] = srcBuffer[0];
				dstBuffer[1] = srcBuffer[1];
				dstBuffer[2] = srcBuffer[2];
			}
		}
	}
	else
	{
		auto linesize = data.width * 3;
		for (int i = 0; i < bufferBMP.Height; i++)
		{
			memcpy(bufferBMP.pBuffer + i * bufferBMP.Pitch, data.raw_data + linesize * i, linesize);
		}
	}

	CenterWindow(hWnd);
	destroy_img_data(data);
	return true;
}

void RenderScene()
{
	::BitBlt(hWindowDC, 0, 0, bufferBMP.Width, bufferBMP.Height, bufferBMP.hDC, 0, 0, SRCCOPY);
}

void Uninitialize(HWND hWnd)
{
	::ReleaseDC(hWindow, hWindowDC);
	bufferBMP.Release();
}