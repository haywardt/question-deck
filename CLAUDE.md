# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**Question Deck** is an ESP8266-based captive portal that creates an open Wi-Fi access point ("QuestionDeck") hosting a web interface for drawing random icebreaker questions. It's designed for bars, cafes, and social gatherings.

**Architecture**: Single Arduino sketch with embedded questions (stored in PROGMEM to preserve RAM), a DNS captive portal, and a minified web frontend.

## Build & Deployment

This is an **Arduino sketch for ESP8266** microcontrollers.

### Prerequisites
- Arduino IDE (latest) or PlatformIO
- ESP8266 Arduino core installed (Board Manager > esp8266 by ESP8266 Community)

### Compiling & Flashing

**Arduino IDE**:
1. Open `questions.cpp` in Arduino IDE
2. Select Board: **ESP8266 Modules > Generic ESP8266 Module** (or your specific board)
3. Set upload speed to **115200**
4. Connect ESP8266 via USB/UART adapter
5. Sketch > Upload (or Ctrl+U)
6. Monitor Serial output at **115200 baud** to verify AP started

**PlatformIO** (if using VSCode extension):
```
pio run -t upload
pio device monitor
```

**Arduino CLI**:
```bash
# Compile
arduino-cli compile -b esp8266:esp8266:generic questions.ino

# Upload (replace COM6 with your serial port)
arduino-cli upload -b esp8266:esp8266:generic -p COM6 questions.ino

# Monitor serial output (optional)
arduino-cli monitor -p COM6 -c baudrate=115200
```

On Linux/Mac, use `/dev/ttyUSB0` or `/dev/ttyACM0` instead of `COM6`. Find your port with:
- **Windows**: `mode` (COM6, COM3, etc.)
- **Linux/Mac**: `ls /dev/tty*`

## Code Structure

- **Question arrays** (lines 25–130): Two PROGMEM const arrays of 50 questions each
  - `lightQuestions`: Fun icebreakers (~50 questions)
  - `deepQuestions`: Meaningful bar-friendly questions (~50 questions)
- **Helper functions** (lines 133–151):
  - `getQuestion()`: Safely copies PROGMEM string to RAM buffer (required on ESP8266 due to limited SRAM)
  - `sendQuestionArray()`: Serializes question array to JSON with proper escaping
- **Embedded HTML/CSS/JS** (lines 154–156): Minified single-page app (all in one R"rawliteral" string for flash efficiency)
- **setup()** (lines 158–193):
  - Starts WiFi in AP mode
  - Initializes DNS server (captive portal)
  - Registers HTTP routes
- **loop()** (lines 195–198): Continuously processes DNS and HTTP requests

## Key Implementation Details

### PROGMEM & Memory Management
- Questions stored in flash (PROGMEM) because ESP8266 has only ~80 KB free SRAM
- `strcpy_P()` copies PROGMEM strings safely to stack buffer
- 256-byte buffer in `getQuestion()` is sufficient for all questions; increase if adding longer questions

### Web Frontend
- Fully minified to fit in ~3 KB
- Uses `fetch()` to load both question arrays on startup
- Client-side JSON escaping for strings with backslashes or quotes
- Responsive CSS with dark-mode support (prefers-color-scheme)
- Button interactions: Light/Deep/Any questions, Next button repeats last category

### Captive Portal
- DNS server redirects **all domains** to AP IP: "catch-all" for `server.arg("*")`
- When user connects, any HTTP request (e.g., captive portal detection) redirects to `/`
- Works transparently on most devices (iOS, Android, Windows, Mac)

## Modifications & Common Tasks

### Adding Questions
- Edit `lightQuestions[]` or `deepQuestions[]` arrays (lines 25–130)
- Keep within 256-byte buffer limit (check via Serial output if unsure)
- Update array size in `sendQuestionArray()` calls (currently hardcoded 50) if adding/removing questions
- Escape special characters: `\"` for quotes, `\\` for backslashes

### Changing Wi-Fi SSID or Making Network Require Password
- Line 16–17: `ssid` and `password` constants
- To add password: `WiFi.softAP(ssid, password)` already supports non-NULL values
- Note: Open networks increase discoverability in bars/cafes (current design choice)

### Modifying Web UI
- Minified HTML is in `index_html[]` PROGMEM string (lines 154–156)
- Colors: `#b45f2b` (brown), `#5e3a1f` (dark), `#fef7e0` (cream) set the bar theme
- To debug: un-minify by pasting raw HTML into browser DevTools, then re-minify before committing
- JS logic: category tracking, random selection, and fade-in of questions on button click

### Serial Debugging
- Initialized at 115200 baud (line 159)
- Logs AP IP on startup
- To add debug logging: `Serial.println("your message")`
- Use Serial Monitor (Arduino IDE > Tools > Serial Monitor) to view output

## Testing & Verification

1. **Flash device** and open Serial Monitor at 115200 baud → confirm "Server started"
2. **Connect to "QuestionDeck" Wi-Fi** (should appear immediately after AP starts)
3. **Open any URL** (e.g., `http://example.com`) → should redirect to question page
4. **Click buttons** → verify questions load from both arrays, random selection works
5. **Test on multiple devices** (phone, tablet) → responsive design should adapt

## Gotchas & Notes

- **Question buffer overflow**: If adding questions >256 bytes, increase `char buffer[256]` in `getQuestion()`
- **DNS conflicts**: Some networks or devices may bypass captive portal (e.g., corporate networks with mandatory login)
- **ESP8266 flash wear**: Frequent uploads (>~100K times) can degrade flash. Normal development is fine.
- **Power consumption**: AP with active clients uses ~200 mA; runs off USB or 5V with minimal current limiting
- **HTML minification**: Before making large UI changes, unminify the string, edit in a proper editor, then re-minify to fit in PROGMEM
