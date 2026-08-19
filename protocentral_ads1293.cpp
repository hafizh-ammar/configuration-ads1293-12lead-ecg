#include "protocentral_ads1293.h"

ADS1293::ADS1293(uint8_t drdyPin, uint8_t csPin, SPIClass *spi) noexcept
	: drdyPin_(drdyPin), csPin_(csPin), spi_(spi) {}

void ADS1293::begin(bool startSPI)
{
	if (drdyPin_ != 255)
	{
		pinMode(drdyPin_, INPUT_PULLUP);
	}
#ifdef MISO
	pinMode(MISO, INPUT_PULLUP);
#endif
	pinMode(csPin_, OUTPUT);
	digitalWrite(csPin_, HIGH);
	if (startSPI && spi_)
	{
		spi_->begin();
	}
}

void ADS1293::begin(uint8_t sck, uint8_t miso, uint8_t mosi)
{
	if (drdyPin_ != 255)
	{
		pinMode(drdyPin_, INPUT_PULLUP);
	}
	pinMode(miso, INPUT_PULLUP);
	pinMode(csPin_, OUTPUT);
	digitalWrite(csPin_, HIGH);
	if (spi_)
	{
#if defined(ARDUINO_ARCH_ESP32)
		spi_->begin(sck, miso, mosi);
#else
		spi_->begin();
#endif
	}
}

bool ADS1293::writeRegister(Register reg, uint8_t value) noexcept
{
	if (!spi_)
		return false;
	uint8_t addr = static_cast<uint8_t>(reg) & WREG_MASK;
	spi_->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
	digitalWrite(csPin_, LOW);
	spi_->transfer(addr);
	spi_->transfer(value);
	digitalWrite(csPin_, HIGH);
	spi_->endTransaction();
	delayMicroseconds(10);
	return true;
}

bool ADS1293::readRegister(Register reg, uint8_t &value) noexcept
{
	if (!spi_)
		return false;
	uint8_t cmd = static_cast<uint8_t>(reg) | RREG_FLAG;
	spi_->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
	digitalWrite(csPin_, LOW);
	spi_->transfer(cmd);
	value = spi_->transfer(0x00);
	digitalWrite(csPin_, HIGH);
	spi_->endTransaction();
	return true;
}

int32_t ADS1293::signExtend24(uint32_t value) noexcept
{

	value &= 0xFFFFFFu;
	if (value & 0x800000u)
	{
		return static_cast<int32_t>(value | 0xFF000000u);
	}
	return static_cast<int32_t>(value);
}

bool ADS1293::getECGData(int32_t &ch1, int32_t &ch2, int32_t &ch3)
{

	if (!spi_)
		return false;

	const uint8_t startAddr = 0x37;
	uint8_t buf[9] = {0};

	spi_->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
	digitalWrite(csPin_, LOW);
	spi_->transfer(startAddr | RREG_FLAG);
	for (int i = 0; i < 9; ++i)
	{
		buf[i] = spi_->transfer(0x00);
	}
	digitalWrite(csPin_, HIGH);
	spi_->endTransaction();

	uint32_t raw1 = (static_cast<uint32_t>(buf[0]) << 16) |
					(static_cast<uint32_t>(buf[1]) << 8) |
					static_cast<uint32_t>(buf[2]);
	uint32_t raw2 = (static_cast<uint32_t>(buf[3]) << 16) |
					(static_cast<uint32_t>(buf[4]) << 8) |
					static_cast<uint32_t>(buf[5]);
	uint32_t raw3 = (static_cast<uint32_t>(buf[6]) << 16) |
					(static_cast<uint32_t>(buf[7]) << 8) |
					static_cast<uint32_t>(buf[8]);

	ch1 = signExtend24(raw1);
	ch2 = signExtend24(raw2);
	ch3 = signExtend24(raw3);
	return true;
}

bool ADS1293::getRaw24(uint8_t channel, uint32_t &raw24)
{
	if (!spi_)
		return false;
	if (channel < 1 || channel > 3)
		return false;

	const uint8_t startAddr = static_cast<uint8_t>(0x37 + ((channel - 1) * 3));
	uint8_t buf3[3] = {0};

	spi_->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
	digitalWrite(csPin_, LOW);
	spi_->transfer(startAddr | RREG_FLAG);
	for (int i = 0; i < 3; ++i)
		buf3[i] = spi_->transfer(0x00);
	digitalWrite(csPin_, HIGH);
	spi_->endTransaction();

	raw24 = (static_cast<uint32_t>(buf3[0]) << 16) | (static_cast<uint32_t>(buf3[1]) << 8) | static_cast<uint32_t>(buf3[2]);
	return true;
}

float ADS1293::rawToVoltage(int32_t signedCode, float vref, int32_t adcFullscale, float gain) noexcept
{
	if (adcFullscale == 0) adcFullscale = (1 << 23) - 1;
	return (static_cast<float>(signedCode) / static_cast<float>(adcFullscale)) * vref * gain;
}

ADS1293::Samples ADS1293::getECGData()
{
	Samples s;
	int32_t a = 0, b = 0, c = 0;
	if (getECGData(a, b, c))
	{
		s.ch1 = a;
		s.ch2 = b;
		s.ch3 = c;
		s.ok = true;
	}
	return s;
}

uint8_t ADS1293::readDeviceID()
{
	uint8_t val = 0;
	readRegister(Register::REVID, val);
	return val;
}

uint8_t ADS1293::readErrorStatus()
{
	uint8_t val = 0;
	readRegister(Register::ERR_STATUS, val);
	return val;
}

bool ADS1293::isDataReady()
{
	uint8_t status = 0;
	if (!readRegister(Register::DATA_STATUS, status))
		return false;

	return (status & 0xE0) != 0;
}

bool ADS1293::begin3LeadECG()
{

	if (!configureChannel1(FlexCh1Mode::Default))
		return false;
	if (!configureChannel2(FlexCh2Mode::Default))
		return false;
	if (!enableCommonModeDetection(CMDetMode::Enabled))
		return false;
	if (!configureRLD(RLDMode::Default))
		return false;
	if (!configureOscillator(OscMode::Default))
		return false;
	if (!configureAFEShutdown(AFEShutdownMode::AllEnabled))
		return false;
	if (!configureSamplingRates(R2Rate::Rate_2, R3Rate::Rate_2, R3Rate::Rate_2))
		return false;
	if (!configureDRDYSource(DRDYSource::Default))
		return false;
	if (!configureChannelConfig(ChannelConfig::Default3Lead))
		return false;
	if (!applyGlobalConfig(GlobalConfig::Start))
		return false;

	return true;
}

bool ADS1293::configureChannel1(FlexCh1Mode m)
{

	return writeRegister(Register::FLEX_CH1_CN, static_cast<uint8_t>(m));
}

bool ADS1293::configureChannel2(FlexCh2Mode m)
{

	return writeRegister(Register::FLEX_CH2_CN, static_cast<uint8_t>(m));
}

bool ADS1293::enableCommonModeDetection(CMDetMode m)
{

	return writeRegister(Register::CMDET_EN, static_cast<uint8_t>(m));
}

bool ADS1293::configureRLD(RLDMode m)
{

	return writeRegister(Register::RLD_CN, static_cast<uint8_t>(m));
}

bool ADS1293::configureOscillator(OscMode m)
{

	return writeRegister(Register::OSC_CN, static_cast<uint8_t>(m));
}

bool ADS1293::configureAFEShutdown(AFEShutdownMode m)
{

	return writeRegister(Register::AFE_SHDN_CN, static_cast<uint8_t>(m));
}

bool ADS1293::configureChannel3(FlexCh3Mode m)
{
	return writeRegister(Register::FLEX_CH3_CN, static_cast<uint8_t>(m));
}

bool ADS1293::configureRef(RefMode m)
{
	return writeRegister(Register::REF_CN, static_cast<uint8_t>(m));
}

bool ADS1293::configureSamplingRates(R2Rate r2, R3Rate r3ch1, R3Rate r3ch2, R3Rate r3ch3)
{

	bool ok = true;
	ok &= writeRegister(Register::R2_RATE, static_cast<uint8_t>(r2));
	delay(1);
	ok &= writeRegister(Register::R3_RATE_CH1, static_cast<uint8_t>(r3ch1));
	delay(1);
	ok &= writeRegister(Register::R3_RATE_CH2, static_cast<uint8_t>(r3ch2));
	delay(1);
	ok &= writeRegister(Register::R3_RATE_CH3, static_cast<uint8_t>(r3ch3));
	delay(1);
	return ok;
}

bool ADS1293::configureDRDYSource(DRDYSource m)
{

	return writeRegister(Register::DRDYB_SRC, static_cast<uint8_t>(m));
}

bool ADS1293::configureChannelConfig(ChannelConfig m)
{

	return writeRegister(Register::CH_CNFG, static_cast<uint8_t>(m));
}

bool ADS1293::applyGlobalConfig(GlobalConfig m)
{

	return writeRegister(Register::CONFIG, static_cast<uint8_t>(m));
}

void ADS1293::disableChannel(uint8_t channel)
{
	if (channel == 1)
	{
		writeRegister(Register::FLEX_CH1_CN, 0x00);
		delay(1);
	}
}

void ADS1293::disableFilterAll()
{
	writeRegister(Register::DIS_EFILTER, 0x07);
	delay(1);
}

bool ADS1293::disableFilter(uint8_t channel)
{
	if (channel < 1 || channel > 3)
	{
		return false;
	}
	uint8_t mask = static_cast<uint8_t>(1u << (channel - 1));
	writeRegister(Register::DIS_EFILTER, mask);
	delay(1);
	return true;
}

bool ADS1293::attachTestSignal(uint8_t channel, TestSignal sig)
{
	if (channel < 1 || channel > 3)
	{
		return false;
	}
	Register reg = static_cast<Register>(0x00 + channel);

	uint8_t current = 0;
	if (!readRegister(reg, current))
	{
		return false;
	}

	uint8_t value = (current & 0x3F) | (static_cast<uint8_t>(sig) << 6);
	bool ok = writeRegister(reg, value);
	delay(1);
	return ok;
}

bool ADS1293::enableTestSignalAll(TestSignal sig)
{
	bool ok = true;
	for (uint8_t ch = 1; ch <= 3; ++ch)
	{
		ok &= attachTestSignal(ch, sig);
	}
	return ok;
}

bool ADS1293::configureWilsonCentralTerminal()
{

	bool ok = true;
	ok &= writeRegister(Register::WILSON_EN1, 0x01);
	delay(1);
	ok &= writeRegister(Register::WILSON_EN2, 0x02);
	delay(1);
	ok &= writeRegister(Register::WILSON_EN3, 0x03);
	delay(1);
	ok &= writeRegister(Register::WILSON_CN, 0x01);
	delay(1);
	return ok;
}

bool ADS1293::configureWilsonReference(WilsonInput buf1, WilsonInput buf2, WilsonInput buf3)
{

	bool ok = true;
	ok &= writeRegister(Register::WILSON_EN1, static_cast<uint8_t>(buf1));
	delay(1);
	ok &= writeRegister(Register::WILSON_EN2, static_cast<uint8_t>(buf2));
	delay(1);
	ok &= writeRegister(Register::WILSON_EN3, static_cast<uint8_t>(buf3));
	delay(1);
	return ok;
}

bool ADS1293::configureWilsonControl(bool goldbergerInternal, bool wilsonInternal)
{

	if (goldbergerInternal && wilsonInternal)
	{
		return false;
	}
	uint8_t value = 0;
	if (goldbergerInternal) value |= 0x02;
	if (wilsonInternal)     value |= 0x01;
	return writeRegister(Register::WILSON_CN, value);
}

bool ADS1293::configureSyncb(SyncbMode m)
{

	return writeRegister(Register::SYNCB_CN, static_cast<uint8_t>(m));
}

bool ADS1293::writeRegisterRaw(uint8_t address, uint8_t value)
{
	return writeRegister(static_cast<Register>(address), value);
}

bool ADS1293::readRegisterRaw(uint8_t address, uint8_t &value)
{
	return readRegister(static_cast<Register>(address), value);
}

bool ADS1293::setSamplingRate(ADS1293::SamplingRate s)
{

	uint8_t r2Reg = 0x02;
	uint8_t r3Reg = 0x10;

	switch (s)
	{
	case ADS1293::SamplingRate::SPS_853:

		r2Reg = 0x04;
		r3Reg = 0x01;
		break;
	case ADS1293::SamplingRate::SPS_512:

		r2Reg = 0x04;
		r3Reg = 0x04;
		break;
	case ADS1293::SamplingRate::SPS_256:

		r2Reg = 0x02;
		r3Reg = 0x04;
		break;
	case ADS1293::SamplingRate::SPS_128:

		r2Reg = 0x02;
		r3Reg = 0x10;
		break;
	case ADS1293::SamplingRate::SPS_64:

		r2Reg = 0x02;
		r3Reg = 0x20;
		break;
	case ADS1293::SamplingRate::SPS_32:

		r2Reg = 0x02;
		r3Reg = 0x40;
		break;
	default:
		return false;
	}

	bool ok = true;
	ok &= writeRegister(Register::R2_RATE, r2Reg);
	delay(1);
	ok &= writeRegister(Register::R3_RATE_CH1, r3Reg);
	delay(1);
	ok &= writeRegister(Register::R3_RATE_CH2, r3Reg);
	delay(1);
	ok &= writeRegister(Register::R3_RATE_CH3, r3Reg);
	delay(1);

	return ok;
}
