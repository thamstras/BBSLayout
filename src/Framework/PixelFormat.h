#pragma once

enum PixelFormat
{
	PF_RGBA32,		// 
	PF_ABGR32,
};

size_t PixelFormatSize(PixelFormat pf);