#pragma once
#include <cstdint>

extern "C" void
seperateUVChannelNV12(uint8_t * UVbuf, uint8_t * Ubuf, uint8_t * Vbuf, int linesize, int widthOfOutPut, int height);
