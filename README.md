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
  channel/AFE di level register chip. Notch Filter untuk noise 34-35 Hz dan
  101-105 Hz pada jalur WCT IN6 (master) → IN4 (slave) diterapkan di lapisan
  terpisah (bukan di kode ini), bersifat post-processing terhadap data mentah
  yang keluar dari chip.
- **Filter tersebut belum berhasil menghilangkan noise sepenuhnya** —
  sejauh ini hanya berhasil **mengurangi** magnitude noise pada frekuensi
  tersebut, bukan menghilangkannya. Artinya residual noise pada jalur WCT
  masih ada dan berpotensi memengaruhi kualitas sinyal, termasuk risiko
  memicu misklasifikasi pada model AI (lihat catatan terkait di laporan KP).
- Konfigurasi WCT untuk chest lead (V1-V6) pada `configureSlaveChestLeads()`
  **belum diuji coba langsung**, sehingga performa filter pada kondisi 6
  chest lead aktif bersamaan di kedua slave belum diketahui — bisa jadi
  residual noise-nya lebih besar atau lebih kecil dari yang terukur pada
  limb lead.
- Pengujian sebelumnya (termasuk karakterisasi noise 34-35 Hz & 68-70 Hz)
  dilakukan menggunakan **ECG simulator (MS400)**, bukan sinyal dari
  tubuh manusia langsung. Sinyal simulator bersifat sintetis dan bersih dari
  noise fisiologis (motion artifact, EMG, variasi impedansi kulit-elektroda),
  sehingga hasil pengujian ini belum tentu merepresentasikan kondisi noise
  saat akuisisi pada subjek manusia sebenarnya.
- Perlu penyempurnaan filter (misal menaikkan orde, mempersempit bandwidth
  notch, atau kombinasi dengan filter lain) serta pengujian akuisisi data
  end-to-end (hardware + filter), baik dengan simulator maupun subjek
  manusia, sebelum konfigurasi ini dipakai untuk pengambilan data
  produksi/klasifikasi AI.

## Konteks Proyek

Bagian dari proyek pengembangan sistem ECG 12-lead berbasis modul CJMCU-1293
dan AI untuk deteksi aritmia (Kerja Praktik / Internship, PUI-PT IS-IoT,
Telkom University).

<img width="1918" height="827" alt="Screenshot 2026-07-17 161911" src="https://github.com/user-attachments/assets/6af738c8-7510-4f45-87db-355abf1522f2" />

