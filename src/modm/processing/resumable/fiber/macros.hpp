/*
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_RF_MACROS_HPP
#define MODM_RF_MACROS_HPP

#include <modm/architecture/interface/assert.hpp>
#include <modm/architecture/utils.hpp>
#include <modm/processing/fiber/scheduler.hpp>

/// @ingroup modm_processing_resumable
/// @{

#define RF_BEGIN(index)

#define RF_BEGIN()

#define RF_END_RETURN(...) return __VA_ARGS__

#define RF_END()

#define RF_END_RETURN(...) return __VA_ARGS__

#define RF_YIELD() modm::fiber::yield()

#define RF_WAIT_WHILE(...) \
    do { \
        while (__VA_ARGS__) { \
            modm::fiber::yield(); \
        } \
    } while(0)

#define RF_WAIT_UNTIL(...) \
    RF_WAIT_WHILE(!(__VA_ARGS__))

#define RF_CALL(...) __VA_ARGS__

// TODO: implement blocking
#define RF_CALL(...) __VA_ARGS__

#define RF_RETURN_CALL(...) return __VA_ARGS__

#define RF_RETURN(...) return __VA_ARGS__

#define RF_RETURN() return

/// @}

#endif

#endif // MODM_RF_MACROS_HPP
