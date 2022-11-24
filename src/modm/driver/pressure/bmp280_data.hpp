/*
 * Copyright (c) 2016, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Fabian Greif
 * Copyright (c) 2022, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BMP280_DATA_HPP
#define MODM_BMP280_DATA_HPP

#include <cstdio>
#include <cmath>

#include <modm/debug/logger/logger.hpp>

namespace modm
{

template <typename Transport>
class Bmp280;

/**
 * Holds the calibration data from the sensor.
 * Values are used for calculation of calibrated
 * sensor values from raw sensor data
 */
struct modm_packed
Bmp280Calibration
{
	uint16_t T1; // 88 89
	int16_t  T2; // 8A 8B
	int16_t  T3; // 8C 8D
	uint16_t P1; // 8e 8f
	int16_t  P2; // 90 91
	int16_t  P3; // 92 93
	int16_t  P4; // 94 95
	int16_t  P5; // 96 97
	int16_t  P6; // 98 99
	int16_t  P7; // 9a 9b
	int16_t  P8; // 9c 9d
	int16_t  P9; // 9e 9f
};

static_assert(sizeof(Bmp280Calibration) == 12*2);

/// @ingroup modm_driver_bmp280
class modm_packed Bmp280Data
{
	template <typename Transport>
	friend class Bmp280;

public:
	/// \return calibrated temperature in 0.01 °C
	int32_t
	getTemperatureFractional();

	/// \return calibrated temperature in °C as 32-bit float
	float
	getTemperature();

	/// \return calibrated pressure in Pascal
	int32_t
	getPressure();

protected:
	/**
	 * Calculate the calibrated temperature from measurement data.
	 * The result is cached.
	 */
	void
	calculateTemperature();

	/**
	 * Calculate the calibrated pressure from measurement data.
	 * The result is cached.
	 */
	void
	calculatePressure();

private:
	// The raw data that was read from the sensor
	// 0 .. 2 pressure data
	// 3 .. 5 temperature data
	std::array<uint8_t, 6> raw_;

	Bmp280Calibration calibration_;

	int32_t temperature_; // in 0.01 degree Celsius
	int32_t pressure_; // in Pa

	// internal temperature value, required for pressure calculation
	int32_t t_fine_;

	enum class ValueCalculated : uint8_t
	{
		Temperature = 0b01,
		Pressure    = 0b10
	};
	MODM_FLAGS8(ValueCalculated);

	ValueCalculated_t calculated_;
};

} // modm namespace

#include "bmp280_data_impl.hpp"

#endif // MODM_BMP280_DATA_HPP
