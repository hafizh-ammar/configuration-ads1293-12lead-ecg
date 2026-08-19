#include "ads1293_leadconfig.h"

bool configureMasterLeadIandIIandIII(ADS1293 &ads)
{
	bool ok = true;

	ok &= ads.configureChannel1(FlexCh1Mode::LeadI_LA_RA);
	ok &= ads.configureChannel2(FlexCh2Mode::LeadII_LL_RA);
	ok &= ads.configureChannel3(FlexCh3Mode::LeadIII_LL_LA);

	ok &= ads.enableCommonModeDetection(CMDetMode::Enabled);

	ok &= ads.configureRLD(RLDMode::Default);

	ok &= ads.configureWilsonReference(WilsonInput::IN1, WilsonInput::IN2, WilsonInput::IN3);
	// WILSONINT=true: WCT muncul di pin IN6 master, lalu via kabel masuk ke IN4 slave
	ok &= ads.configureWilsonControl(false, true);

	ok &= ads.configureRef(RefMode::Default);

	ok &= ads.configureAFEShutdown(AFEShutdownMode::AllEnabled);

	ok &= ads.configureSamplingRates(R2Rate::Rate_2, R3Rate::Rate_2, R3Rate::Rate_2, R3Rate::Rate_2);

	ok &= ads.configureDRDYSource(DRDYSource::Channel1Ecg);

	ok &= ads.configureSyncb(SyncbMode::MasterDriveCh1Ecg);

	ok &= ads.configureOscillator(OscMode::MasterCrystalAndOut);

	ok &= ads.configureChannelConfig(ChannelConfig::Default3Lead);

	return ok;
}

bool configureSlaveChestLeads(ADS1293 &ads)
{
	bool ok = true;

	ok &= ads.configureChannel1(FlexCh1Mode::Chest_V_Wct);
	ok &= ads.configureChannel2(FlexCh2Mode::Chest_V_Wct);
	ok &= ads.configureChannel3(FlexCh3Mode::Chest_V_Wct);

	// CM Detection HARUS enabled agar noise 50Hz common-mode bisa di-reject
	ok &= ads.enableCommonModeDetection(CMDetMode::Enabled);
	// RLD tetap shutdown pada slave (hanya master yang boleh drive RLD)
	ok &= ads.configureRLD(RLDMode::ShutDown);

	ok &= ads.configureRef(RefMode::Default);

	ok &= ads.configureAFEShutdown(AFEShutdownMode::AllEnabled);

	ok &= ads.configureSamplingRates(R2Rate::Rate_2, R3Rate::Rate_2, R3Rate::Rate_2, R3Rate::Rate_2);

	ok &= ads.configureDRDYSource(DRDYSource::Channel1Ecg);

	ok &= ads.configureSyncb(SyncbMode::SlaveInput);

	ok &= ads.configureOscillator(OscMode::SlaveExternalClock);

	ok &= ads.configureChannelConfig(ChannelConfig::Default3Lead);

	return ok;
}

bool startSynchronizedAcquisition(ADS1293 &master, ADS1293 &slave1, ADS1293 &slave2)
{
	bool ok = true;

	ok &= master.applyGlobalConfig(GlobalConfig::Start);
	delay(5);

	ok &= slave1.applyGlobalConfig(GlobalConfig::Start);
	delay(2);

	ok &= slave2.applyGlobalConfig(GlobalConfig::Start);
	delay(5);

	return ok;
}
