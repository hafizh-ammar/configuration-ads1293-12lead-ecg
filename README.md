# ads1293-12lead-ecg
# ADS1293 12-Lead ECG Configuration Library

Library konfigurasi multi-device (1 master + 2 slave) untuk chip **TI ADS1293**
menggunakan modul **CJMCU-1293**, dipakai untuk akuisisi ECG 12-lead yang
tersinkronisasi antar 3 IC.

## Struktur

- `protocentral_ads1293.h/.cpp` — driver dasar untuk komunikasi SPI dengan
  chip ADS1293 (baca/tulis register, akuisisi data ECG, dsb). Dimodifikasi
  dari library resmi [ProtoCentral ADS1293 Arduino Library](https://github.com/Protocentral/protocentral-ads1293-arduino) (MIT License).
- `ads1293_leadconfig.h/.cpp` — konfigurasi lead I/II/III pada device master,
  konfigurasi chest lead (WCT) pada device slave, dan fungsi sinkronisasi
  akuisisi antar master-slave. Ditulis sendiri untuk keperluan proyek 12-lead ECG.

## Kredit / Attribution

Bagian driver dasar (`protocentral_ads1293.*`) merupakan modifikasi dari kode
open-source milik **ProtoCentral**, dirilis di bawah **MIT License**.
Repo asli: https://github.com/Protocentral/protocentral-ads1293-arduino

Lisat [LICENSE](./LICENSE) untuk detail lengkap.

## Konteks Proyek

Bagian dari proyek pengembangan sistem ECG 12-lead berbasis modul CJMCU-1293
dan AI untuk deteksi aritmia (Kerja Praktik / Internship, PUI-PT IS-IoT,
Telkom University).

