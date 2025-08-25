/*
 * =====================================================================================
 *
 * Filename:  quest_udp_common.hpp
 *
 * Description:  This is the SHARED header file. Both your publisher (on the Quest)
 * and your receiver (on the robot) must include this file. It
 * guarantees they agree on the data format.
 *
 * =====================================================================================
 */
#pragma once

#include <cstdint>

// This struct defines the exact data packet we will send.
// It's packed to ensure it has the same memory layout on all systems.
#pragma pack(push, 1)
struct JoystickData {
    float left_x;
    float left_y;
    float right_x;
    float right_y;
    int64_t timestamp_ns; // Timestamp for latency tracking
};
#pragma pack(pop)