# Question Deck

An ESP8266-based captive portal that creates an open Wi-Fi access point ("icebreaker") hosting a web interface for drawing random icebreaker and meaningful questions. Designed for bars, cafes, and social gatherings.

## Features

- **400 Questions**: 200 light icebreakers + 200 deep, meaningful questions
- **Progressive Difficulty**: Questions get deeper as the session progresses
- **Synchronized Rotation**: All devices see the same question, rotating every 60 seconds
- **One Reader Model**: One person reads the question to the group; others see listen statements
- **Screen Wake Lock**: Keeps screens awake while the app is active
- **Responsive Design**: Works on phones, tablets, and computers
- **No Internet Required**: Works entirely on local Wi-Fi network

## Architecture

- **Microcontroller**: ESP8266 (Generic)
- **Questions**: Stored in PROGMEM (flash) to preserve RAM
- **Web Interface**: Single-page app with embedded HTML/CSS/JS
- **Networking**: DNS captive portal redirects all domains to the app

## Build & Deployment

### Requirements
- Arduino IDE (latest) or arduino-cli
- ESP8266 Arduino core installed

### Flashing

**Arduino IDE:**
1. Open `questions.ino`
2. Select Board: **ESP8266 Modules > Generic ESP8266 Module**
3. Set upload speed to **115200**
4. Connect ESP8266 via USB/UART adapter (use blue USB 3.0 ports for stable power)
5. Sketch > Upload

**arduino-cli:**
```bash
arduino-cli compile --fqbn esp8266:esp8266:generic questions.ino
arduino-cli upload -p COM6 --fqbn esp8266:esp8266:generic questions.ino
```

## Usage

1. Flash the device
2. Connect to **"icebreaker"** Wi-Fi (open network)
3. Open any URL in browser (e.g., `http://example.com`)
4. You'll be redirected to the question page
5. Questions rotate every 60 seconds with one person designated as the reader

## Configuration

Edit these constants in `questions.ino`:

```cpp
const char* ssid = "icebreaker";          // Wi-Fi SSID
const unsigned long QUESTION_DURATION = 60000;  // 60 seconds per question
const unsigned int TOTAL_QUESTIONS = 60;        // Total in progression
```

## Memory Usage

- **Flash**: 26% used (279 KB / 1 MB)
- **RAM**: 41% used (mostly WiFi/server buffers)
- Questions stored in PROGMEM (flash), not RAM

## Adding Questions

Edit the `lightQuestions[]` or `deepQuestions[]` arrays in `questions.ino`. Keep questions under 256 bytes and escape special characters (`\"` for quotes, `\\` for backslashes).

## Hardware Notes

- **Power**: Use USB 3.0 (blue) ports for stable power
- **UART Adapter**: CH340 or similar USB-to-serial adapter
- **Flash wear**: Normal development is fine; frequent uploads (>100K) can degrade flash
- **Current draw**: ~200 mA with active clients

## Troubleshooting

**Board not detected on COM port:**
- Check CH340 drivers (reinstall if needed)
- Use a powered USB hub
- Try a different USB cable or port

**Questions not showing:**
- Ensure you're connected to "icebreaker" Wi-Fi
- Hard refresh browser (Ctrl+F5)
- Clear browser cache

## License

MIT
