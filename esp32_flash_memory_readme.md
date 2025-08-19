# ESP32 Flash Memory Layout Guide

Understanding ESP32 flash memory organization is crucial for firmware development, data persistence, and troubleshooting.

## Overview

The ESP32 uses **SPI Flash** for storing firmware, data, and files. The total flash size varies by model (4MB, 8MB, 16MB, etc.). This memory is divided into **partitions** that serve different purposes.

## Default Flash Memory Layout (4MB)

```
ESP32 Flash Memory (4MB = 4,194,304 bytes)
┌─────────────────────────────────────────┐ 0x000000
│                BOOTLOADER               │
│              (Second Stage)             │ 32KB
│            0x000000 - 0x008000          │
├─────────────────────────────────────────┤ 0x008000
│             PARTITION TABLE             │
│            0x008000 - 0x009000          │ 4KB
├─────────────────────────────────────────┤ 0x009000
│                   NVS                   │
│           (Non-Volatile Storage)        │
│            0x009000 - 0x00E000          │ 20KB
├─────────────────────────────────────────┤ 0x00E000
│                PHY_INIT                 │
│         (WiFi/BT Calibration)           │
│            0x00E000 - 0x00F000          │ 4KB
├─────────────────────────────────────────┤ 0x00F000
│                FACTORY                  │
│            (Main Application)           │
│            0x010000 - 0x200000          │ 2MB
├─────────────────────────────────────────┤ 0x200000
│                 SPIFFS                  │
│             (File System)               │
│            0x200000 - 0x400000          │ 2MB
└─────────────────────────────────────────┘ 0x400000
```

## Partition Descriptions

### **1. Bootloader (32KB)**
- **Address**: `0x000000 - 0x008000`
- **Purpose**: Second-stage bootloader
- **Content**: 
  - Initializes hardware
  - Loads and starts main application
  - Handles OTA updates
- **Erasable**: No (system critical)
- **User Access**: Read-only

### **2. Partition Table (4KB)**
- **Address**: `0x008000 - 0x009000`
- **Purpose**: Defines memory layout
- **Content**:
  - Partition names, addresses, sizes
  - Partition types and subtypes
  - Bootable partition flags
- **Erasable**: No (system critical)
- **User Access**: Read-only

### **3. NVS - Non-Volatile Storage (20KB)**
- **Address**: `0x009000 - 0x00E000`
- **Purpose**: Key-value storage for settings
- **Content**:
  - WiFi credentials
  - Application configuration
  - User preferences
  - Calibration data
- **Erasable**: Yes (with `nvs_flash_erase()`)
- **User Access**: Read/Write via NVS API
- **Survives**: Firmware updates ✅

```cpp
// Example NVS usage
#include <Preferences.h>
Preferences preferences;

preferences.begin("my_app", false);
preferences.putString("wifi_ssid", "MyNetwork");
preferences.putInt("led_brightness", 128);
preferences.end();
```

### **4. PHY_INIT (4KB)**
- **Address**: `0x00E000 - 0x00F000`
- **Purpose**: RF calibration data
- **Content**:
  - WiFi PHY parameters
  - Bluetooth calibration
  - Country-specific RF settings
- **Erasable**: Yes (auto-regenerated)
- **User Access**: System managed
- **Survives**: Firmware updates ✅

### **5. Factory/Application (2MB)**
- **Address**: `0x010000 - 0x200000`
- **Purpose**: Main firmware storage
- **Content**:
  - Your compiled C++ code
  - Libraries and dependencies
  - Program data and constants
- **Erasable**: Yes (during firmware upload)
- **User Access**: Execute-only
- **Survives**: Firmware updates ❌

### **6. SPIFFS File System (2MB)**
- **Address**: `0x200000 - 0x400000`
- **Purpose**: File storage
- **Content**:
  - HTML, CSS, JS files
  - Images, certificates
  - Data files, logs
- **Erasable**: Yes (with `uploadfs`)
- **User Access**: Read/Write via SPIFFS API
- **Survives**: Firmware updates ✅ (unless `uploadfs`)

```cpp
// Example SPIFFS usage
#include <SPIFFS.h>

SPIFFS.begin();
File file = SPIFFS.open("/config.json", "r");
String data = file.readString();
file.close();
```

## PlatformIO Operations and Effects

### **Firmware Upload** (`pio run --target upload`)
```
✅ Preserved:
- NVS (user settings)
- PHY_INIT (WiFi calibration)
- SPIFFS (web files)

❌ Overwritten:
- Factory/Application partition
```

### **Filesystem Upload** (`pio run --target uploadfs`)
```
✅ Preserved:
- NVS (user settings)
- PHY_INIT (WiFi calibration)  
- Factory/Application (firmware)

❌ Overwritten:
- SPIFFS partition
```

### **Full Erase** (`pio run --target erase`)
```
❌ Erased:
- Everything except bootloader
- All user data lost
- Factory reset equivalent
```

## Custom Partition Tables

You can define custom partitions in `partitions.csv`:

```csv
# Name,   Type, SubType, Offset,  Size,    Flags
nvs,      data, nvs,     0x9000,  0x5000,
phy_init, data, phy,     0xe000,  0x1000,
factory,  app,  factory, 0x10000, 1M,
storage,  data, spiffs,  0x110000, 1M,
logs,     data, spiffs,  0x210000, 512K,
backup,   app,  ota_0,   0x290000, 1M,
```

Then reference it in `platformio.ini`:
```ini
board_build.partitions = partitions.csv
```

## Memory Size by ESP32 Variant

| **Model** | **Flash Size** | **Typical Layout** |
|-----------|----------------|-------------------|
| ESP32-WROOM-32 | 4MB | Standard layout |
| ESP32-WROVER | 4MB/8MB/16MB | Extra space for files |
| ESP32-S2 | 4MB | Similar to ESP32 |
| ESP32-S3 | 8MB/16MB/32MB | Larger app partition |
| ESP32-C3 | 4MB | Optimized layout |

## Advanced Memory Operations

### **Check Partition Info**
```cpp
#include <esp_partition.h>

void printPartitionInfo() {
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);
    
    while (it != NULL) {
        const esp_partition_t *partition = esp_partition_get(it);
        Serial.printf("Partition: %s, Type: %d, Address: 0x%X, Size: %d KB\n",
                     partition->label, partition->type, partition->address, partition->size / 1024);
        it = esp_partition_next(it);
    }
    esp_partition_iterator_release(it);
}
```

### **Check Free Space**
```cpp
void checkMemoryUsage() {
    // Application partition
    size_t app_size = ESP.getSketchSize();
    size_t app_free = ESP.getFreeSketchSpace();
    
    // SPIFFS
    size_t spiffs_total = SPIFFS.totalBytes();
    size_t spiffs_used = SPIFFS.usedBytes();
    
    // Heap (RAM)
    size_t heap_free = ESP.getFreeHeap();
    size_t heap_total = ESP.getHeapSize();
    
    Serial.printf("App: %d KB used, %d KB free\n", app_size/1024, app_free/1024);
    Serial.printf("SPIFFS: %d KB used, %d KB total\n", spiffs_used/1024, spiffs_total/1024);
    Serial.printf("Heap: %d KB free, %d KB total\n", heap_free/1024, heap_total/1024);
}
```

## Best Practices

### **1. Data Persistence Strategy**
- **Settings/Config**: Use NVS (survives firmware updates)
- **Large Files**: Use SPIFFS (web assets, logs)
- **Temporary Data**: Use heap RAM (lost on reboot)

### **2. Update Strategy**
```cpp
// Safe firmware update process:
1. Backup critical NVS data (optional)
2. Upload new firmware (NVS preserved)
3. Verify settings compatibility
4. Update SPIFFS if needed
```

### **3. Memory Monitoring**
```cpp
// Regular memory health checks
void memoryHealthCheck() {
    if (ESP.getFreeHeap() < 10000) {
        Serial.println("WARNING: Low heap memory!");
    }
    
    if (SPIFFS.usedBytes() > SPIFFS.totalBytes() * 0.9) {
        Serial.println("WARNING: SPIFFS nearly full!");
    }
}
```

### **4. Factory Reset Implementation**
```cpp
void factoryReset() {
    // Clear NVS
    nvs_flash_erase();
    nvs_flash_init();
    
    // Format SPIFFS
    SPIFFS.format();
    
    // Restart
    ESP.restart();
}
```

## Troubleshooting

### **Common Issues**

| **Problem** | **Cause** | **Solution** |
|-------------|-----------|--------------|
| Settings lost after update | Used wrong storage method | Use NVS instead of files |
| Web files missing | SPIFFS not uploaded | Run `pio run --target uploadfs` |
| Out of memory errors | App too large | Optimize code or increase partition |
| Boot loops | Corrupted partition table | Full erase and reflash |

### **Memory Debugging**
```cpp
void debugMemoryLayout() {
    Serial.println("=== ESP32 Memory Info ===");
    Serial.printf("Chip model: %s\n", ESP.getChipModel());
    Serial.printf("Flash size: %d MB\n", ESP.getFlashChipSize() / 1024 / 1024);
    Serial.printf("Sketch size: %d KB\n", ESP.getSketchSize() / 1024);
    Serial.printf("Free sketch space: %d KB\n", ESP.getFreeSketchSpace() / 1024);
    Serial.printf("Free heap: %d KB\n", ESP.getFreeHeap() / 1024);
    Serial.printf("SPIFFS total: %d KB\n", SPIFFS.totalBytes() / 1024);
    Serial.printf("SPIFFS used: %d KB\n", SPIFFS.usedBytes() / 1024);
}
```

## Key Takeaways

1. **NVS is your friend** - Use it for persistent settings
2. **SPIFFS survives firmware updates** - Unless you run `uploadfs`
3. **Application partition is overwritten** - Every firmware upload
4. **Bootloader and partition table** - Never modified in normal development
5. **Plan your data strategy** - Choose the right partition for each data type

Understanding this layout helps you build robust ESP32 applications that handle updates gracefully and provide a great user experience! 🚀