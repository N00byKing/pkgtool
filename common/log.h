// SPDX-FileCopyrightText: Copyright 2014 Citra Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <algorithm>
#include <array>
#include <string_view>

// Define the fmt lib macros
#define LOG_GENERIC(log_class, log_level, ...) (void(0))

#define LOG_TRACE(log_class, fmt, ...) (void(0))

#define LOG_DEBUG(log_class, ...) (void(0))
#define LOG_INFO(log_class, ...) (void(0))
#define LOG_WARNING(log_class, ...) (void(0))
#define LOG_ERROR(log_class, ...) (void(0))
#define LOG_CRITICAL(log_class, ...) (void(0))
