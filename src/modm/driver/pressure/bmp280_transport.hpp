/*
 * Copyright (c) 2022, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BMP280_TRANSPORT_HPP
#define MODM_BMP280_TRANSPORT_HPP

#include <span>
#include <modm/processing/resumable.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/interface/spi_device.hpp>

#include "bmp280.hpp"

namespace modm
{

/// I2C transport for BMP280 driver
/// @ingroup modm_driver_bmp280
template<typename I2cMaster>
class Bmp280TransportI2c : public modm::I2cDevice<I2cMaster, 2>
{
public:
	/// @param address 7-bit I2C device address (0x76 or 0x77)
	Bmp280TransportI2c(uint8_t address = 0x76);

	modm::ResumableResult<bool>
	read(bmp280::Register startRegister, std::span<uint8_t> data);

	modm::ResumableResult<bool>
	write(bmp280::Register reg, uint8_t value);

private:
	uint8_t buffer_[2]{};
};


/// SPI transport for BMP280 driver
/// @ingroup modm_driver_bmp280
/// @tparam Cs Chip select gpio output
template<typename SpiMaster, typename Cs>
class Bmp280TransportSpi : public modm::SpiDevice<SpiMaster>, protected modm::NestedResumable<2>
{
public:
	Bmp280TransportSpi();

	modm::ResumableResult<bool>
	read(bmp280::Register startRegister, std::span<uint8_t> data);

	modm::ResumableResult<bool>
	write(bmp280::Register reg, uint8_t value);

private:
	uint8_t buffer_[2]{};
};

}	// namespace modm

#include "bmp280_transport_impl.hpp"

#endif // MODM_BMP280_TRANSPORT_HPP
