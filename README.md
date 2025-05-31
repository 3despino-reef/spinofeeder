# 🐟 Comedero Automático para Acuarios con Motor NEMA

Este proyecto implementa un **comedero automático programable para acuarios** utilizando un motor NEMA y una rueda dosificadora de alta precisión, diseñado para garantizar una alimentación constante, controlada y fiable de peces en entornos acuáticos.

👉 Mira el proyecto en acción en [este video de YouTube](https://www.youtube.com/watch?v=YcooHASoYPQ)

---

## 🎯 Características Principales

- ✅ **Motor NEMA + rueda dosificadora** para una dispensación precisa del alimento.
- 🕒 **Dos horarios configurables** de alimentación diaria.
- 🔄 **3 modos de funcionamiento**:
  - `auto`: dosifica automáticamente según la hora.
  - `cycle`: modo de emergencia cíclico (sin red ni hora).
  - `on`: dosificación manual.
- 🌐 **Conexión WiFi automática** mediante [WiFiManager](https://github.com/tzapu/WiFiManager).
- 📡 **Sincronización horaria por NTP** (CET/CEST).
- 💾 **Almacenamiento persistente** en la EEPROM (Preferences).
- 📟 **Pantalla informativa** para el estado del sistema y menús.
- 🟢 **Watchdog integrado** para reinicio automático en caso de bloqueo.

---

## 🧠 Arquitectura del Sistema

El firmware está dividido en módulos claros:

| Módulo            | Descripción                                       |
|-------------------|---------------------------------------------------|
| `FeederManager`   | Lógica de dosificación y control del motor NEMA   |
| `DisplayManager`  | Renderizado de información en pantalla            |
| `MenuManager`     | Navegación por menú físico con botón              |
| `LEDManager`      | LED indicador de estado general                   |
| `WiFiManager`     | Gestión automática de red WiFi                    |
| `NTPClient`       | Sincronización horaria y ajuste por horario de verano |

---

## 🔧 Requisitos de Hardware

- ✅ **ESP32**
- ✅ **Motor paso a paso NEMA + Driver (A4988 o similar)**
- ✅ **Pantalla OLED/LCD (I2C)**
- ✅ **Botón físico** para navegación
- ✅ **Fuente de alimentación independiente para el motor**

---

## 🚀 Instalación

1. Clona este repositorio:
   ```bash
   git clone https://github.com/3despino-reef/spinofeeder
