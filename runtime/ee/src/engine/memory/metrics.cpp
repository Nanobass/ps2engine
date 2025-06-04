//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        metrics.cpp
//
// Description: 
//
//=============================================================================

#include <engine/memory/metrics.hpp>

//========================================
// System Includes
//========================================

#include <sstream>

#include <ps2gl/metrics.h>

//========================================
// Project Includes
//========================================

#include <core/memory.hpp>
#include <engine/memory/memory.hpp>

//========================================
// Definitions
//========================================

namespace pse::metrics
{  

std::string generate_metrics()
{
    std::stringstream ss;
    ss << "Metrics per 30 frames" << std::endl;
    ss << "  Renderer Uploads: " << pglGetMetric(kMetricsRendererUpload) << std::endl;
    ss << "  Texture Uploads: " << pglGetMetric(kMetricsTextureUploadCount) << std::endl;
    ss << "  Texture Throughput: " << pglGetMetric(kMetricsTextureUploadBytes) / 1024 << " KBytes" << std::endl;
    ss << "  Texture Binds: " << pglGetMetric(kMetricsBindTexture) << std::endl;
    ss << "  CLUT Uploads: " << pglGetMetric(kMetricsClutUploadCount) << std::endl;
    pglResetMetrics();
    
    ss << "  Total Free Memory: " << pse::memory::get_total_free_memory() / 1024 << " KBytes" << std::endl;

    size_t total_free_memory;
    size_t largest_block;
    size_t number_of_allocations;
    size_t peak_usage;

    pse::memory::get_allocator(pse::memory::PSE_ALLOCATOR_DEFAULT)->get_statistics(total_free_memory, largest_block, number_of_allocations, peak_usage);
    ss << "  Default Allocator: " << std::endl;
    ss << "    Total Free Memory: " << total_free_memory / 1024 << " KBytes" << std::endl;
    ss << "    Largest Block: " << largest_block << " Bytes" << std::endl;
    ss << "    Number of Allocations: " << number_of_allocations << std::endl;
    ss << "    Peak Usage: " << peak_usage / 1024 << " KBytes" << std::endl;

    pse::memory::get_allocator(pse::GME_FAST)->get_statistics(total_free_memory, largest_block, number_of_allocations, peak_usage);
    ss << "  Fast Allocator: " << std::endl;
    ss << "    Total Free Memory: " << total_free_memory / 1024 << " KBytes" << std::endl;
    ss << "    Largest Block: " << largest_block << " Bytes" << std::endl;
    ss << "    Number of Allocations: " << number_of_allocations << std::endl;
    ss << "    Peak Usage: " << peak_usage / 1024 << " KBytes" << std::endl;

    return ss.str();
}

}