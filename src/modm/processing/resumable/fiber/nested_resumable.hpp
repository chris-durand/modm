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

#ifndef MODM_NESTED_RESUMABLE_HPP
#define MODM_NESTED_RESUMABLE_HPP

#include "macros.hpp"
#include <cstdint>

namespace modm
{

/**
 * Dummy implementation to be used with fibers
 *
 * @ingroup	modm_processing_resumable
 */
template<uint8_t Levels = 1>
class NestedResumable
{};
/// @endcond

} // namespace modm

#endif // MODM_NESTED_RESUMABLE_HPP
