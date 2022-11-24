/*
 * Copyright (c) 2016-2017, Sascha Schade
 * Copyright (c) 2017, Fabian Greif
 * Copyright (c) 2017, Niklas Hauser
 * Copyright (c) 2022, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <cinttypes>
#include <cmath>
#include <cstdio>

#include <modm/debug/logger/logger.hpp>

#ifndef MODM_BMP280_DATA_HPP
#	error  "Don't include this file directly, use 'bme280_data.hpp' instead!"
#endif

namespace modm
{

inline void
Bmp280Data::calculateTemperature()
{
	const int32_t adc = (((int32_t(raw_[3])) << 16) | (raw_[4] << 8) | (raw_[5] << 0)) >> 4;

	const int32_t T1 = calibration_.T1;
	const int32_t T2 = calibration_.T2;
	const int32_t T3 = calibration_.T3;

	const int32_t var1 = ((((adc >> 3) - (T1 << 1))) * (T2)) >> 11;

	const int32_t var2 = (	(	( ((adc >> 4) - (T1)) *
								  ((adc >> 4) - (T1))
								) >> 12
							) * (T3)
						) >> 14;

	t_fine_ = var1 + var2;

	temperature_ = (t_fine_ * 5 + 128) >> 8;
	calculated_ |= ValueCalculated::Temperature;
}

inline void
Bmp280Data::calculatePressure()
{
	// The temperature is required for calculating the calibrated pressure
	if (not (calculated_ & ValueCalculated::Temperature)) {
		calculateTemperature();
	}

	const int32_t adc = (((int32_t(raw_[0])) << 16) | (raw_[1] << 8) | (raw_[2] << 0)) >> 4;

	const int64_t P1 = calibration_.P1;
	const int64_t P2 = calibration_.P2;
	const int64_t P3 = calibration_.P3;
	const int64_t P4 = calibration_.P4;
	const int64_t P5 = calibration_.P5;
	const int64_t P6 = calibration_.P6;
	const int64_t P7 = calibration_.P7;
	const int64_t P8 = calibration_.P8;
	const int64_t P9 = calibration_.P9;

	int64_t var1 = t_fine_ - 128'000;
	int64_t var2 = var1 * var1 * P6;
	var2 = var2 + ((var1 * P5) << 17);
	var2 = var2 + (P4 << 35);
	var1 = ((var1 * var1 * P3) >> 8) + ((var1 * P2) << 12);
	var1 = ((((int64_t(1)) << 47) + var1)) * (P1) >> 33;

	if (var1 == 0) {
    	return;  // avoid division by zero
  	}
  	int64_t p = 1048576 - adc;
  	p = (((p << 31) - var2) * 3125) / var1;
  	var1 = ((P9) * (p >> 13) * (p >> 13)) >> 25;
  	var2 = (P8 * p) >> 19;

	p = ((p + var1 + var2) >> 8) + (P7 << 4);

	pressure_ = p / 256;
	calculated_ |= ValueCalculated::Pressure;
}

inline int32_t
Bmp280Data::getTemperatureFractional()
{

	if (not (calculated_ & ValueCalculated::Temperature)) {
		calculateTemperature();
	}
	return temperature_;
}

inline float
Bmp280Data::getTemperature()
{
	return getTemperatureFractional() * 0.01f;
}

inline int32_t
Bmp280Data::getPressure()
{
	if (not (calculated_ & ValueCalculated::Pressure)) {
		calculatePressure();
	}
	return pressure_;
}

} // modm namespace
