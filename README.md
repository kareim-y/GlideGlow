# GlideGlow

Smart lighting that blends hardware and mobile—an Arduino-powered device controlled from a React Native app.  
Use the app to trigger lighting modes, adjust effects, and manage the device over a wireless link. _(Add a one-line product pitch here.)_

---

## Table of contents

- [Features](#features)
- [Repo layout](#repo-layout)
- [Prereqs](#prereqs)
- [Quick start (App)](#quick-start-app)
- [Quick start (Arduino)](#quick-start-arduino)
- [Configuration](#configuration)
- [Build & release](#build--release)
- [Troubleshooting](#troubleshooting)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)

---

## Features

- Mobile app written in React Native (JavaScript) for iOS.
- Embedded firmware (Arduino/C++) to drive LEDs / peripherals.
- Image assets & UI button art included.
- Whack-a-Mole game built-in into firmware and app software

---

## Repo layout

GlideGlow/

├─ Arduino Code/ # Microcontroller firmware (C/C++)

├─ ButtonPictures/ # UI icons / button images

├─ assets/ # App assets (images, fonts, etc.)

├─ ios/ # iOS native project (React Native CLI)

├─ App.js # React Native app entry

├─ const.js # App-wide constants / config

├─ package.json # App dependencies & scripts

└─ ... # Babel config, app.json, etc.

---

## Prereqs

- **Node.js** (LTS recommended) & **npm** or **yarn**
- **Xcode** (for iOS builds)
- **CocoaPods** (`sudo gem install cocoapods`)
- **Arduino IDE** (2.x) + board support for your microcontroller
- **USB data cable** for flashing the board
- (Optional) **Expo Go** app if you decide to run with Expo dev client

---

## Quick start (App)

From the repo root:

```bash
# 1) Install JS deps
npm install
# or
yarn install

# 2) iOS native deps
cd ios && pod install && cd ..

# 3) Run the app (Metro + iOS)
npm run ios
# or
yarn ios
```

## Quick start (Arduino)

- Open Arduino IDE.
- Board & port: Select your board (e.g., Nano/UNO/ESP32) and serial port.
- Libraries: Install any required LED/driver libraries (e.g., FastLED, Adafruit_NeoPixel).
- Open the sketch from Arduino Code/ and Verify → Upload.
- Note the baud rate and protocol (e.g., BLE/GATT commands or Serial) expected by the app.

TODO (you): List exact board model, pinout, LED type (WS2812B, SK6812, etc.), power limits, and any wiring cautions.

## Configuration

const.js — central place for:
Device service/characteristic IDs (if using BLE)
Command opcodes / payload shapes
UI presets (names, colors, speeds)
Feature toggles

## Contributing

Contributions are welcome!

- Fork
- Create feature branch
- Commit with clear messages
- Open PR
- For bug reports, please include device model, iOS version, board type, and steps to reproduce.

## License

MIT. See LICENSE
