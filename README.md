# Ivanita-s-DIY-DJ-Controller

DIY DJ Controller — Sensor-Based Expressive Interface
This repository documents the design and implementation of a DIY DJ controller built from scratch, combining custom hardware, sensor-based interaction, and open-source DJ software. The project explores alternative approaches to DJ controller design that prioritize expressive physical interaction, creative authorship, and hands-on making over closed, commercial systems.
Overview
The controller features a symmetrical two-deck layout and interfaces with Mixxx via MIDI. Instead of traditional rotary encoders and faders, the system uses magnetic angle sensing, time-of-flight distance sensing, and touch-based control to support more fluid and gestural performance techniques.
The controller was initially developed as part of a music technology course and later expanded into a master’s thesis, investigating DIY instrument design, physical interaction, ethical AI use, and authorship in digital DJ performance.
As Leah Buechley has argued, “the experience of making things by hand is an important part of being human.”
This philosophy directly informs the design and construction of this instrument.
Hardware Features
Dual-deck, fully symmetrical controller layout
AS5600 magnetic angle sensors for smooth, continuous rotary encoder behavior (jog wheels)
VL53L0X time-of-flight sensors for contactless tempo and parameter control
Trill Touch Bar used as a crossfader
Teensy-based embedded system
Custom PCB design
Laser-cut enclosure made from recycled plastic
3D-printed components for mechanical integration
Software
DJ Software: Mixxx
Communication: MIDI
Custom firmware maps sensor data to MIDI CCs and notes for real-time performance control
Research Context
This project treats the DJ controller as a research artifact, using practice-based research to explore:
Expressive interaction in digital DJ performance
DIY hardware as a form of creative and technical authorship
Alternatives to proprietary music technology ecosystems
The relationship between physical gesture and musical control
Repository Structure
/firmware        → Teensy / microcontroller code  
/hardware        → Schematics, PCB designs, and enclosure files  
/mappings        → Mixxx MIDI mappings  
/media           → Photos and demo materials  
/docs            → Additional documentation
Demo
🎥 Video demo:
[YouTube link here]
