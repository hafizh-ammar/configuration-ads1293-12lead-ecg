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

## Known Limitations / Belum Diuji

- File `ads1293_leadconfig.cpp` di repo ini hanya mengatur **routing/topology WCT**
  (`configureWilsonReference`, `configureWilsonControl`) dan konfigurasi
  channel/AFE di level register chip. **Tidak ada penerapan Notch Filter di
  level kode ini** — filter noise 34-35 Hz dan 68-70 Hz (orde-3) yang
  sebelumnya dipakai untuk membersihkan jalur WCT IN6 (master) → IN4 (slave)
  dilakukan di lapisan terpisah (post-processing/software), belum
  terintegrasi ke pipeline firmware yang ada di sini.
- Konfigurasi WCT untuk chest lead (V1-V6) pada `configureSlaveChestLeads()`
  **belum diuji coba langsung**. Karena filter noise belum tersambung ke
  jalur ini, belum bisa dipastikan noise WCT sudah hilang atau belum pada
  kondisi 6 chest lead aktif bersamaan di kedua slave.
- Perlu pengujian akuisisi data end-to-end (hardware + filter) sebelum
  konfigurasi ini dipakai untuk pengambilan data produksi/klasifikasi AI.

## Konteks Proyek

Bagian dari proyek pengembangan sistem ECG 12-lead berbasis modul CJMCU-1293
dan AI untuk deteksi aritmia (Kerja Praktik / Internship, PUI-PT IS-IoT,
Telkom University).
