//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        singletons.hpp
//
// Description: Global Singletons
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

/* standard library */
#include <math.h>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <utility>
#include <memory>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <chrono>

/* ps2sdk */
#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <stdio.h>
#include <graph.h>
#include <gs_psm.h>
#include <osd_config.h>

/* ps2gl */
#include <GL/ps2gl.h>
#include <GL/gl.h>

/* ps2stuff */
#include <ps2s/gs.h>

//========================================
// Project Includes
//========================================

/* engine */
#include <engine/renderer.hpp>
#include <engine/renderer/font.hpp>
#include <engine/renderer/skybox.hpp>
#include <engine/renderer/basic.hpp>

/* ps2memory */
#include <ps2memory.hpp>

/* ps2glu */
#include <ps2glu.hpp>

/* ps2math */
#include <ps2math.hpp>

inline std::unique_ptr<pse::RenderManager> g_RenderManager = nullptr;
inline std::unique_ptr<pse::TextRenderer> g_TextRenderer = nullptr;
inline std::unique_ptr<pse::SkyboxRenderer> g_SkyboxRenderer = nullptr;
inline std::unique_ptr<pse::BasicRenderer> g_BasicRenderer = nullptr;
inline std::unique_ptr<pse::OrthographicCamera> g_HudCamera = nullptr;
inline pse::Font* g_DefaultFont = nullptr;