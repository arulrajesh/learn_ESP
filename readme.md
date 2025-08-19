# ESP32 LED Controller

A web-based LED controller for ESP32-S3-Zero with configurable colors, timing, and color order.

## Features

- **Web Interface**: Control LEDs through a beautiful web interface
- **WiFi Access Point**: Creates its own WiFi network - no existing WiFi needed
- **Persistent Configuration**: Settings save to flash memory and survive reboots
- **Color Order Configuration**: Support for different LED types (RGB, GRB, etc.)
- **Real-time Updates**: Changes apply immediately without firmware updates
- **Color Testing**: Built-in test functions to verify correct colors
- **Separate Web Files**: HTML, CSS, and JavaScript in separate files for easy editing

## Hardware Requirements

- ESP32-S3-Zero (Waveshare)
- WS2812/NeoPixel LED connected to GPIO 21

## Software Requirements

- PlatformIO
- Libraries (automatically installed):
  - Adafruit NeoPixel
  - ArduinoJson

## Project Structure

```
esp32-led-controller/
├── platformio.ini           # Build configuration
├── include/
│   ├── config.h             # Configuration management
│   ├── wifi_manager.h       # WiFi Access Point setup
│   ├── web_server.h         # Web server declarations
│   └── led_controller.h     # LED control functions
├── src/
│   ├── main.cpp             # Main program entry point
│   ├── config.cpp           # Configuration save/load
│   ├── wifi_manager.cpp     # WiFi AP implementation
│   ├── web_server.cpp       # Web server and HTTP handlers
│   └── led_controller.cpp   # LED control task
├── data/
│   ├── index.html           # Main web interface
│   ├── style.css            # Styling and layout
│   └── script.js            # JavaScript functionality
└── README.md                # This file
```

## Quick Start

1. **Clone/Extract** this project
2. **Open in PlatformIO** (VS Code + PlatformIO extension)
3. **Upload filesystem** (contains HTML/CSS/JS files):
   ```bash
   pio run --target uploadfs
   ```
4. **Build and Upload firmware**:
   ```bash
   pio run -e release --target upload --target monitor
   ```
5. **Connect to WiFi**:
   - Network: "ESP32-LED-Controller"
   - Password: "12345678"
6. **Open Web Interface**: http://192.168.4.1

## Configuration

### WiFi Settings
Edit `src/wifi_manager.cpp` to change WiFi credentials:
```cpp
const char* ap_ssid = "Your-Network-Name";
const char* ap_password = "Your-Password";
```

### Hardware Settings
Edit `include/config.h` to change pin assignments:
```cpp
#define RGB_LED_PIN 21        // LED data pin
#define NUM_PIXELS 1          // Number of LEDs
```

## Build Configurations

- **Debug**: `pio run -e debug --target upload`
  - Includes debug output
  - Easier troubleshooting
  - Slower performance

- **Release**: `pio run -e release --target upload`
  - Optimized for performance
  - Minimal debug output
  - Faster execution

## Web Interface Features

- **Color Picker**: Visual color selection for 3 colors
- **Timing Control**: Adjustable delay between color changes
- **Color Order**: Support for different LED types
- **Test Buttons**: Verify colors display correctly
- **Toggle Control**: Enable/disable LED cycling
- **Status Display**: Real-time configuration viewing
- **Keyboard Shortcuts**:
  - `Ctrl+S`: Save configuration
  - `Ctrl+R`: Refresh settings
  - `Space`: Toggle LED

## Web Files Structure

### HTML (`data/index.html`)
- Clean, semantic markup
- Responsive design
- Form validation
- Accessibility features

### CSS (`data/style.css`)
- Modern styling with hover effects
- Responsive layout for mobile
- Loading animations
- Color preview circles

### JavaScript (`data/script.js`)
- API communication functions
- Form validation
- Real-time preview updates
- Error handling
- Auto-refresh every 30 seconds

## Uploading Web Files

The web interface files must be uploaded to the ESP32's SPIFFS filesystem:

```bash
# Upload filesystem (HTML, CSS, JS files)
pio run --target uploadfs

# Then upload the firmware
pio run -e release --target upload
```

**Important**: Upload filesystem first, then firmware!

## Troubleshooting

### LED Shows Wrong Colors
1. Use the color test buttons (Test Red, Test Green, Test Blue)
2. Try different "LED Color Order" settings until colors match
3. Common orders: RGB, GRB (most WS2812), BRG

### Can't Connect to WiFi
1. Check WiFi network name: "ESP32-LED-Controller"
2. Verify password: "12345678"
3. Reset ESP32 and try again

### Web Interface Won't Load
1. Ensure you're connected to ESP32's WiFi
2. Try http://192.168.4.1 in browser
3. Check if filesystem was uploaded: `pio run --target uploadfs`
4. Check serial monitor for errors

### Files Not Loading (404 errors)
1. Make sure to run `pio run --target uploadfs` first
2. Check that `data/` folder contains all three files
3. Verify SPIFFS initialization in serial monitor

## Development

### Editing Web Interface
1. **Edit files in `data/` folder**:
   - `index.html` - Structure and content
   - `style.css` - Styling and layout
   - `script.js` - Functionality and API calls

2. **Upload changes**:
   ```bash
   pio run --target uploadfs
   ```

3. **No firmware recompilation needed** for web changes!

### Adding New Features
1. **LED Effects**: Modify `src/led_controller.cpp`
2. **Web Interface**: Update files in `data/` folder
3. **Configuration**: Add fields to `include/config.h`
4. **WiFi Features**: Extend `src/wifi_manager.cpp`

### Code Organization Benefits
- **Modular Design**: Each component in separate files
- **Clear Interfaces**: Header files define public APIs
- **Separation of Concerns**: Web, LED, Config, WiFi isolated
- **Easy Testing**: Individual components can be tested
- **Web Development**: Standard HTML/CSS/JS workflow

## License

Open source - modify and use as needed!

## Support

For issues or questions:
1. Check serial monitor output
2. Verify hardware connections
3. Ensure filesystem was uploaded
4. Try different color orders
5. Reset to factory defaults (reflash firmware)