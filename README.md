# idf-external-hw-drivers

A curated and standards-compliant collection of external hardware driver libraries for ESP32 microcontrollers, written using the [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/) framework.

> Developed and maintained by [Adithya Venkata Narayanan](https://github.com/Adithya-187326)

---

## 🎯 Purpose

This repository aims to serve as a high-quality reference for drivers interfacing with common I2C, SPI, UART, and USB-based peripherals using ESP-IDF. These drivers are not added all at once — they are developed organically, **as I encounter and work with these devices during the course of my job or personal technical pursuits**.

My goal is to ensure that others do not need to repeat the same low-level integration efforts when working with the same devices.

---

## ✅ Code Quality and Standards

- ✳️ **SEI CERT-C compliant**: Code adheres to secure and portable C standards.
- 🚫 **No deprecated APIs**: All code uses current, supported ESP-IDF APIs.
- 📚 Modular design: Each device is isolated and easily pluggable.
- 🧪 Strict build checks: Compiler warnings are treated as errors using `-Werror`.

---

## 📦 Device Support

Devices are added **only when I work with them**, ensuring that each integration is real-world tested and meaningful.

| Device  | Protocol   | Status         | Notes                |
| ------- | ---------- | -------------- | -------------------- |
| BME280  | I2C        | ✅ Available   | Environmental sensor |
| MCP3008 | SPI        | ⚙️ In progress | ADC                  |
| TBD     | UART / USB | ⏳ Planned     | Future drivers       |

If you use a supported device, you can be confident it has been tested in an actual use case.

---

## 🛠️ Getting Started

### 📋 Requirements

- [ESP-IDF v5.x](https://docs.espressif.com/projects/esp-idf/en/latest/)
- ESP32-series microcontroller board
- Familiarity with ESP-IDF and embedded development workflows

### 📂 Repository Structure

```
idf-external-hw-drivers/
├── <device_name_1>/
│ ├── include/
│ ├── src/
│ └── example/
├── <device_name_2>/
├── <device_name_3>/
├── <device_name_4>/
├── LICENSE
├── NOTICE
├── README.md
└── CITATION.cff
```

### 🚀 Example Usage

For devices including a self-contained example project in its `example/` directory.

```bash
cd devices/bme280/example
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

---

## 📄 License

This project is licensed under the [MIT License](./LICENSE).

Please see the [NOTICE](./NOTICE) file for attribution and citation guidelines.

## 📖 Citation

If this repository contributes to your work, please cite it as:

```kotlin
Adithya Venkata Narayanan. idf-external-hw-drivers: Modular ESP-IDF drivers for external devices. GitHub, 2024. https://github.com/Adithya-187326/idf-external-hw-drivers

```

You may also use the [CITATION.cff](./CITATION.cff) file or GitHub’s "Cite this repository" feature.

---

## 📬 Contact

For questions, feedback, or collaboration opportunities, please open an issue or contact me via GitHub.
