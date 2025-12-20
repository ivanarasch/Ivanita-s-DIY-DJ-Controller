# DIY DJ Controller

A custom-built DIY DJ controller combining sensor-based hardware, embedded systems, and open-source DJ software to explore expressive and physical approaches to digital DJ performance.

This project was developed initially as part of a music technology course and later expanded into a master’s thesis, focusing on hands-on instrument design, physical interaction, ethical AI use, and creative authorship.

🎥 Video demo:  
https://youtu.be/EEeUhQ8b_II

---

## Overview

This repository documents the design and implementation of a two-deck DJ controller built from scratch. The system uses magnetic angle, distance, and touch sensors to translate physical gestures into musical control. The controller is designed as an alternative to closed, commercial DJ gear, emphasizing expressivity, transparency, and user agency.

---

## Project Goals

- Design a fully functional DJ controller using custom hardware  
- Explore sensor-based interaction beyond traditional knobs and faders  
- Map physical gesture to meaningful musical control  
- Use open-source software and tools where possible  

---

## Repository Structure

arduino/
└── Arduino / Teensy firmware
mixxx/
├── .xml MIDI mapping files
└── .js controller scripts

hardware/
├── PCB designs
└── custom hardware documentation

media/
└── photos of PCB and controller

## System Overview

The controller uses a fully symmetrical two-deck layout. All controls on Deck 1 are mirrored on Deck 2, with the exception of a shared crossfader.

### Hardware

- Teensy microcontroller  
- 2 × AS5600 magnetic angle sensors (jog wheels)  
- 2 × VL53L0X time-of-flight sensors (tempo control)  
- 1 × Trill Touch Bar (crossfader)  
- 20 buttons  
- 12 knobs  
- 2 sliders  
- Custom PCB  
- Laser-cut enclosure made from recycled PLA  
- 3D-printed platters with embedded magnets and Lazy Susan bearings  

### Software

- Mixxx (open-source DJ software)  
- Custom Arduino firmware  
- Custom Mixxx MIDI mappings using `.xml` and `.js` files  

---

## Controls & Interaction

### Jog Wheels
Magnetic angle sensors provide smooth rotary encoder behavior for track nudging and scratching.

### Tempo Control
Time-of-flight sensors adjust tempo based on hand distance above the sensor.

### Channel Controls
- Slider: Channel volume  
- Knob 1: High EQ  
- Knob 2: Mid EQ  
- Knob 3: Low EQ  

### Effects
Three knobs per deck control effects. Effect types are configurable in software.

### Performance & Transport
Buttons handle hot cues, looping, beat sync, and play/pause.

### Crossfader
A Trill Touch Bar acts as a shared crossfader between both decks.

---

## Build Notes & Lessons Learned

Several practical challenges emerged during development. Analog noise from potentiometers caused unstable MIDI output and was resolved through firmware-level smoothing. Jog wheels required manual editing of Mixxx `.xml` and `.js` files to achieve proper scratching behavior, as this cannot be handled by the Learning Wizard alone. PCB footprint errors led to last-minute wiring fixes, reinforcing the importance of careful verification before fabrication. Working with multiple I2C devices required careful bus management, and enclosure fabrication using recycled PLA required additional material-specific testing.

---

## Demo

🎥 Video demonstration:  
https://youtu.be/EEeUhQ8b_II

---

## Author

Ivana Rasch Chinchilla  
Music Technology, NYU  
DIY instruments · embedded systems · DJ technology · creative coding
