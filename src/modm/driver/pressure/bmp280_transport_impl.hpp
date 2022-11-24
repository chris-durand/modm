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
#	error  "Don't include this file directly, use 'bmp280_transport.hpp' instead!"
#endif

namespace modm
{

template<class I2cMaster>
Bmp280TransportI2c<I2cMaster>::Bmp280TransportI2c(uint8_t address)
:	I2cDevice<I2cMaster, 2>{address}
{
}

template<class I2cMaster>
modm::ResumableResult<bool>
modm::Bmp280TransportI2c<I2cMaster>::write(bmp280::Register reg, uint8_t value)
{
	RF_BEGIN();

	buffer_[0] = static_cast<uint8_t>(reg);
	buffer_[1] = value;

	this->transaction.configureWrite(buffer_, 2);

	RF_END_RETURN_CALL(this->runTransaction());
}

template<class I2cMaster>
modm::ResumableResult<bool>
modm::Bmp280TransportI2c<I2cMaster>::read(bmp280::Register startRegister, std::span<uint8_t> data)
{
	RF_BEGIN();

	this->buffer_[0] = static_cast<uint8_t>(startRegister);
	this->transaction.configureWriteRead(this->buffer_, 1, data.data(), data.size());

	RF_END_RETURN_CALL(this->runTransaction());
}

// ============================================================================

template<class SpiMaster, class Cs>
modm::Bmp280TransportSpi<SpiMaster, Cs>::Bmp280TransportSpi()
{
	Cs::setOutput(modm::Gpio::High);
}

template<class SpiMaster, class Cs>
modm::ResumableResult<bool>
modm::Bmp280TransportSpi<SpiMaster, Cs>::write(bmp280::Register reg, uint8_t value)
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	buffer_[0] = static_cast<uint8_t>(reg);
	buffer_[1] = value;

	RF_CALL(SpiMaster::transfer(buffer_, nullptr, 2));

	if (this->releaseMaster())
		Cs::set();

	RF_END_RETURN(true);
}

template<class SpiMaster, class Cs>
modm::ResumableResult<bool>
modm::Bmp280TransportSpi<SpiMaster, Cs>::read(bmp280::Register startRegister, std::span<uint8_t> data)
{
	static const uint8_t foo = 0xA2;
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	RF_CALL(SpiMaster::transfer(static_cast<uint8_t>(startRegister)));
	RF_CALL(SpiMaster::transfer(&foo, data.data(), data.size()));

	if (this->releaseMaster())
		Cs::set();

	RF_END_RETURN(true);
}

} // namespace modm
