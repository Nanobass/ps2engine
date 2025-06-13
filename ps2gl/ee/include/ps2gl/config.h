#pragma once

#include "GL/ps2gl.h"

/* display lists */
const int kDListByteSize = 2048;
const int kDListBufferMaxQwordLength = 16 * 1024;
const int kDListMaxNumRenderPackets = 512;
const int kMaxDListID = 4096;
const int kMaxDListsToBeFreed = 1024;

/* geometry block */
const int kMaxNumStrips = 40;

/* gl context */
const int kDmaPacketMaxQwordLength = 64 * 1024;
const int kMaxDrawEnvChanges = 100;
const int kMaxBuffersToBeFreed = 1024;

/* lighting */
const int kMaxNumLights = 8;

/* matrix stack */
const int kMaxMatStackDepth = 16;

/* render manager */
const int kMaxDefaultRenderers = 64;
const int kMaxUserRenderers = PGL_MAX_CUSTOM_RENDERERS;

/* texture manager */
const int kMaxNumTexNames = 512;
