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
#define MODM_BMP280_HPP

#include <modm/processing/resumable.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/interface/register.hpp>

#include "bmp280_data.hpp"

namespace modm
{

template <typename Transport>
class Bmp280;

/// @ingroup modm_driver_bmp280
struct bmp280
{
	using Calibration = modm::Bmp280Calibration;
	using Data = modm::Bmp280Data;

	/// @cond
	/// The addresses of the Configuration and Data Registers
	enum class
	Register : uint8_t
	{
		//HUM_LSB    = 0xFE,
		//HUM_MSB    = 0xFD,
		TEMP_XLSB  = 0xFC,
		TEMP_LSB   = 0xFB,
		TEMP_MSB   = 0xFA,
		PRESS_XLSB = 0xF9,
		PRESS_LSB  = 0xF8,
		PRESS_MSB  = 0xF7,

		CONFIG     = 0xF5,
		CTRL_MEAS  = 0xF4,
		STATUS     = 0xF3,
		CTRL_HUM   = 0xF2,

		// up to     0xF0
		CALIB26    = 0xE1,

		RESET      = 0xE0,

		CHIP_ID    = 0xD0,

		// up to     0xA1
		CALIB00    = 0x88,
	};

	static constexpr uint8_t ChipId = 0x58;

	/// The value written to the reset register that resets the device
	static constexpr uint8_t ResetValue = 0xb6;

	/// The options of REGISTER_CONTROL
	enum class
	Conversion : uint8_t
	{
		Temperature = 0x2E,
		Pressure = 0x34,
	};
	/// @endcond

	// Register 0xF3 "status"
	enum class
	Status : uint8_t
	{
		MEASURING = Bit3,	// Automatically set to ‘1’ whenever a conversion is running and back to ‘0’ when the results have been transferred to the data registers.
		IM_UPDATE = Bit0,	// Automatically set to ‘1’ when the NVM data are being copied to image registers and back to ‘0’ when the copying is done. The data are copied at power-on-reset and before every conversion.
	};
	MODM_FLAGS8(Status);

	// Register 0xF4 "ctrl_meas"
	enum class
	CtrlMeas : uint8_t
	{
		OSRS_T2 = Bit7,
		OSRS_T1 = Bit6,
		OSRS_T0 = Bit5,
		OSRS_P2 = Bit4,
		OSRS_P1 = Bit3,
		OSRS_P0 = Bit2,
		Mode1   = Bit1,
		Mode0   = Bit0,
	};
	MODM_FLAGS8(CtrlMeas);

	enum class
	Mode : uint8_t
	{
		Sleep = 0,
		Forced = int(CtrlMeas::Mode0),
		Normal = int(CtrlMeas::Mode1) | int(CtrlMeas::Mode0),
	};
	using Mode_t = Configuration<CtrlMeas_t, Mode, (Bit1 | Bit0)>;

	// [4:2]
	enum class
	Oversampling : uint8_t
	{
		Skipped          = 0b000,
		Oversampling_1x  = 0b001,
		Oversampling_2x  = 0b010,
		Oversampling_4x  = 0b011,
		Oversampling_8x  = 0b100,
		Oversampling_16x = 0b101,

		// for compatibility with BME280 driver
		Single     = 0b001,
		Double     = 0b010,
		Quadrupel  = 0b011,
		Octupel    = 0b100,
		Sexdecuple = 0b101,
	};
	using Pressure = Configuration<CtrlMeas_t, Oversampling, (Bit2 | Bit1 | Bit0), 2>;
	using Temperature = Configuration<CtrlMeas_t, Oversampling, (Bit2 | Bit1 | Bit0), 5>;

	// Register 0xF5 "config"
	enum class
	Config : uint8_t
	{
		T_SB_2 = Bit7,
		T_SB_1 = Bit6,
		T_SB_0 = Bit5,
		FILTER_2 = Bit4,
		FILTER_1 = Bit3,
		FILTER_0 = Bit2,
		SPI3W_EN = Bit0,
	};
	MODM_FLAGS8(Config);

	enum class
	TimeStandby : uint8_t
	{
		Us500   = 0,
		Ms62    =                                             int(Config::T_SB_0),
		Ms125   =                       int(Config::T_SB_1),
		Ms250   =                       int(Config::T_SB_1) | int(Config::T_SB_0),
		Ms500   = int(Config::T_SB_2),
		Ms1000  = int(Config::T_SB_2)                       | int(Config::T_SB_0),
		Ms2000  = int(Config::T_SB_2) | int(Config::T_SB_1),
		Ms4000  = int(Config::T_SB_2) | int(Config::T_SB_1) | int(Config::T_SB_0),
	};
	using TimeStandby_t = Configuration<Config_t, TimeStandby, (Bit7 | Bit6 | Bit5)>;

	enum class
	FilterSettings : uint8_t
	{
		Off = 0,
		F2  =                                                 int(Config::FILTER_0),
		F4  =                         int(Config::FILTER_1),
		F8  =                         int(Config::FILTER_1) | int(Config::FILTER_0),
		F16 = int(Config::FILTER_2) | int(Config::FILTER_1) | int(Config::FILTER_0),
	};

	/// @cond
	static constexpr uint8_t
	i(Mode mode) { return uint8_t(mode); }

	static constexpr uint8_t
	i(Conversion conv) { return uint8_t(conv); }

	static constexpr uint8_t
	i(Register reg) { return uint8_t(reg); }
	/// @endcond
};

/**
 * @author  Christopher Durand
 * @ingroup modm_driver_bmp280
 *
 * @tparam Transport BMP280 transport (I2C or SPI)
 */
template <typename Transport>
class Bmp280 : public bmp280, public Transport
{
public:
	/**
	 * @param	data			reference to measurement data object
	 * @param	transportArgs	arguments passed to transport
	 */
	template<typename... Args>
	Bmp280(Data& data, Args&&... transportArgs);

	modm::ResumableResult<bool>
	initialize(
			Mode mode = Mode::Normal,
			Oversampling temperature = Oversampling::Oversampling_1x,
			Oversampling pressure = Oversampling::Oversampling_1x);

	/// Read the measurement data from the sensor. The sensor must be in normal mode.
	modm::ResumableResult<bool>
	readout();

	/// Start a single measurement in forced mode. The sensor will go to standby after this.
	modm::ResumableResult<bool>
	startMeasurement(
			Oversampling temperature = Oversampling::Oversampling_1x,
			Oversampling pressure = Oversampling::Oversampling_1x);

public:
	/// Get the data object for this sensor.
	inline Data&
	data()
	{ return data; }

private:
	Data& data_;
	uint8_t buffer_;
};

}	// namespace modm

#include "bmp280_impl.hpp"

#endif // MODM_BME280_HPP
