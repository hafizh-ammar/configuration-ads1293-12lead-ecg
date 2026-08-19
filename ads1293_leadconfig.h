#pragma once

#include <Arduino.h>
#include "protocentral_ads1293.h"

bool configureMasterLeadIandIIandIII(ADS1293 &ads);

bool configureSlaveChestLeads(ADS1293 &ads);

bool startSynchronizedAcquisition(ADS1293 &master, ADS1293 &slave1, ADS1293 &slave2);
