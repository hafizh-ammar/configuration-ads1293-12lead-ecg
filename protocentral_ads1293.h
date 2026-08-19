#pragma once

#include <Arduino.h>
#include <SPI.h>

constexpr uint8_t WREG_MASK = 0x7F;
constexpr uint8_t RREG_FLAG = 0x80;

enum class Register : uint8_t {
  CONFIG = 0x00,
  FLEX_CH1_CN = 0x01,
  FLEX_CH2_CN = 0x02,
  FLEX_CH3_CN = 0x03,
  FLEX_PACE_CN = 0x04,
  FLEX_VBAT_CN = 0x05,
  LOD_CN = 0x06,
  LOD_EN = 0x07,
  LOD_CURRENT = 0x08,
  LOD_AC_CN = 0x09,
  CMDET_EN = 0x0A,
  CMDET_CN = 0x0B,
  RLD_CN = 0x0C,

  WILSON_EN1 = 0x0D,
  WILSON_EN2 = 0x0E,
  WILSON_EN3 = 0x0F,
  WILSON_CN = 0x10,
  REF_CN = 0x11,
  OSC_CN = 0x12,
  AFE_RES = 0x13,
  AFE_SHDN_CN = 0x14,
  AFE_FAULT_CN = 0x15,
  AFE_PACE_CN = 0x17,
  ERR_STATUS = 0x19,
  MASK_ERR = 0x2A,
  R2_RATE = 0x21,
  R3_RATE_CH1 = 0x22,
  R3_RATE_CH2 = 0x23,
  R3_RATE_CH3 = 0x24,
  R1_RATE = 0x25,
  DIS_EFILTER = 0x26,
  DRDYB_SRC = 0x27,
  SYNCB_CN = 0x28,
  CH_CNFG = 0x2F,
  DATA_STATUS = 0x30,
  DATA_CH1_PACE = 0x31,
  DATA_CH2_PACE = 0x32,
  DATA_CH3_PACE = 0x33,
  DATA_CH1_ECG = 0x37,
  DATA_CH2_ECG = 0x3A,
  DATA_CH3_ECG = 0x3D,
  REVID = 0x40
};

enum class FlexCh1Mode : uint8_t {
  Default     = 0x11,
  LeadI_LA_RA = 0x11,
  Chest_V_Wct = 0x0C
};
enum class FlexCh2Mode : uint8_t {
  Default      = 0x19,
  LeadII_LL_RA = 0x19,
  Chest_V_Wct  = 0x14
};

enum class CMDetMode : uint8_t {
  Enabled  = 0x07,
  Disabled = 0x00
};

enum class RLDMode : uint8_t {
  Default  = 0x04,
  ShutDown = 0x08

};
enum class OscMode : uint8_t {
  Default             = 0x04,
  MasterCrystalAndOut = 0x05,

  SlaveExternalClock  = 0x06

};

enum class AFEShutdownMode : uint8_t {
  AllEnabled = 0x00,
  Ch1Ch2Only = 0x04,
  AllShutdown = 0x3F
};
enum class R2Rate : uint8_t { Rate_2 = 0x02 };
enum class R3Rate : uint8_t { Rate_2 = 0x02 };
enum class DRDYSource : uint8_t {
  Default     = 0x08,
  Channel1Ecg = 0x08

};
enum class ChannelConfig : uint8_t { Default3Lead = 0x30, Default5Lead = 0x70 };
enum class GlobalConfig : uint8_t { Start = 0x01 };

enum class FlexCh3Mode : uint8_t {
  Default       = 0x2E,
  LeadIII_LL_LA = 0x1A,
  Chest_V_Wct   = 0x1C
};

enum class RefMode : uint8_t {
  Default             = 0x00,
  ShutdownInternalRef = 0x01,
  ShutdownCmRldRef    = 0x02,
  ShutdownBoth        = 0x03
};

enum class TestSignal : uint8_t {
  Positive = 0x01,
  Negative = 0x02,
  Zero = 0x03
};

enum class WilsonInput : uint8_t {
  None = 0x00,
  IN1  = 0x01,
  IN2  = 0x02,
  IN3  = 0x03,
  IN4  = 0x04,
  IN5  = 0x05,
  IN6  = 0x06
};

enum class SyncbMode : uint8_t {
  MasterDriveCh1Ecg = 0x08,
  SlaveInput        = 0x40
};

class ADS1293 {
public:

  explicit ADS1293(uint8_t drdyPin, uint8_t csPin, SPIClass *spi = &SPI) noexcept;

  void begin(bool startSPI = true);

  void begin(uint8_t sck, uint8_t miso, uint8_t mosi);

  bool begin3LeadECG();

  bool getECGData(int32_t &ch1, int32_t &ch2, int32_t &ch3);

  struct Samples {
    int32_t ch1 = 0;
    int32_t ch2 = 0;
    int32_t ch3 = 0;
    bool ok = false;
  };

  Samples getECGData();

  bool getRaw24(uint8_t channel, uint32_t &raw24);

  static int32_t signExtend24(uint32_t raw24) noexcept;

  static float rawToVoltage(int32_t signedCode, float vref = 2.4f, int32_t adcFullscale = ((1 << 23) - 1), float gain = 1.0f) noexcept;

  uint8_t readDeviceID();
  uint8_t readErrorStatus();

  bool isDataReady();

  void disableChannel(uint8_t channel);
  void disableFilterAll();
  bool disableFilter(uint8_t channel);

  bool configureChannel1(FlexCh1Mode m = FlexCh1Mode::Default);
  bool configureChannel2(FlexCh2Mode m = FlexCh2Mode::Default);
  bool configureChannel3(FlexCh3Mode m = FlexCh3Mode::Default);
  bool enableCommonModeDetection(CMDetMode m = CMDetMode::Enabled);
  bool configureRLD(RLDMode m = RLDMode::Default);
  bool configureOscillator(OscMode m = OscMode::Default);
  bool configureAFEShutdown(AFEShutdownMode m = AFEShutdownMode::AllEnabled);
  bool configureRef(RefMode m = RefMode::Default);

  bool configureSamplingRates(R2Rate r2 = R2Rate::Rate_2,
                               R3Rate r3ch1 = R3Rate::Rate_2,
                               R3Rate r3ch2 = R3Rate::Rate_2,
                               R3Rate r3ch3 = R3Rate::Rate_2);
  bool configureDRDYSource(DRDYSource m = DRDYSource::Default);
  bool configureChannelConfig(ChannelConfig m = ChannelConfig::Default3Lead);
  bool applyGlobalConfig(GlobalConfig m = GlobalConfig::Start);

  bool attachTestSignal(uint8_t channel, TestSignal sig);

  bool enableTestSignalAll(TestSignal sig);

  bool configureWilsonCentralTerminal();

  bool configureWilsonReference(WilsonInput buf1, WilsonInput buf2, WilsonInput buf3);

  bool configureWilsonControl(bool goldbergerInternal, bool wilsonInternal);

  bool configureSyncb(SyncbMode m);

  bool writeRegisterRaw(uint8_t address, uint8_t value);
  bool readRegisterRaw(uint8_t address, uint8_t &value);

  enum class SamplingRate : uint8_t {
    SPS_853,
    SPS_512,
    SPS_256,
    SPS_128,
    SPS_64,
    SPS_32
  };

  bool setSamplingRate(SamplingRate s);

private:
  uint8_t drdyPin_ = 255;
  uint8_t csPin_ = 255;
  SPIClass *spi_ = nullptr;

  bool writeRegister(Register reg, uint8_t value) noexcept;
  bool readRegister(Register reg, uint8_t &value) noexcept;

};

using ads1293 = ADS1293;

using SamplingRate = ADS1293::SamplingRate;
