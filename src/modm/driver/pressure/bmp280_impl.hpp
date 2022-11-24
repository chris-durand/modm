/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2018, Niklas Hauser
 * Copyright (c) 2022, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BMP280_HPP
#	error  "Don't include this file directly, use 'bmp280.hpp' instead!"
#endif

#include <span>
#include <modm/math/utils/operator.hpp>
#include <modm/math/utils/bit_operation.hpp>
#include <modm/math/utils/endianness.hpp>

namespace modm
{

// ----------------------------------------------------------------------------
template<typename Transport>
template<typename... Args>
Bmp280<Transport>::Bmp280(Data& data, Args&&... transportArgs)
	: Transport(std::forward<Args>(transportArgs)...), data_{data}
{
}

// ----------------------------------------------------------------------------

template<typename Transport>
modm::ResumableResult<bool>
Bmp280<Transport>::initialize(Mode mode, Oversampling pressure, Oversampling temperature)
{
	RF_BEGIN();

	// verify Chip Id
	{
		if (RF_CALL(this->read(Register::CHIP_ID, std::span{&buffer_, 1})))
		{
			if (buffer_ != ChipId) {
				MODM_LOG_ERROR.printf("BMP280 Chip Id mismatch. Read %02x, expected %02x\n", buffer_, ChipId);
				RF_RETURN(false);
			}
		} else {
			RF_RETURN(false);
		}
	}

	{
		CtrlMeas_t ctrl_meas = Mode_t(mode);
		ctrl_meas |= Pressure(pressure);
		ctrl_meas |= Temperature(temperature);

		buffer_ = ctrl_meas.value;
	}
	if (not RF_CALL(this->write(Register::CTRL_MEAS, buffer_))) {
		RF_RETURN(false);
	}

	// Configure the standby time
	{
		Config_t config = TimeStandby_t(TimeStandby::Ms1000);
		buffer_ = config.value;
	}

	if (not RF_CALL(this->write(Register::CONFIG, buffer_))) {
		RF_RETURN(false);
	}

	// Read 24 bytes of Calib00 to Calib23
	// TODO: check why RF_CALL_END_RETURN does not compile here
	buffer_ = RF_CALL(this->read(Register::CALIB00, std::span(reinterpret_cast<uint8_t*>(&data_.calibration_), 12 * sizeof(uint16_t))));
	RF_END_RETURN(bool(buffer_));
}

template<typename Transport>
modm::ResumableResult<bool>
Bmp280<Transport>::readout()
{
	RF_BEGIN();

	// Get the raw data from sensor
	// It is advised by the datasheet to readout the complete sensor
	// data at once to avoid mixing old and new data.
	// And a single 6-byte transaction is even faster than multiple 3-byte
	// transactions.
	buffer_ = RF_CALL(this->read(Register::PRESS_MSB, data_.raw_));

	MODM_LOG_DEBUG.printf("RAW: %02x %02x %02x %02x %02x %02x\n",
		data_.raw_[0], data_.raw_[1], data_.raw_[2],
		data_.raw_[3], data_.raw_[4], data_.raw_[5]);

	// There will come new data, so new calculation necessary
	data_.calculated_ = Bmp280Data::ValueCalculated{0};

	RF_END_RETURN((bool) buffer_);
}

template<typename Transport>
modm::ResumableResult<bool>
Bmp280<Transport>::startMeasurement(Oversampling pressure, Oversampling temperature)
{
	RF_BEGIN();

	{
		Mode mode = Mode::Forced;

		CtrlMeas_t ctrl_meas = Mode_t(mode);
		ctrl_meas |= Pressure(pressure);
		ctrl_meas |= Temperature(temperature);

		buffer_ = ctrl_meas.value;
	}

	RF_END_RETURN_CALL(this->write(Register::CTRL_MEAS, buffer_));
}

} // namespace modm
