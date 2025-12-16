// Mixxx controller script — TerminalMix4 (AS5600 jog wheels + optional knobs)

var TerminalMix4 = {};

// Optional knob targets (keep or remove if not used)
TerminalMix4.ccTargets = {
  knob9:  { control: "rate", mode: "bipolar" },  // CC9 → [Channel1]
  knob11: { control: "rate", mode: "bipolar" }   // CC11 → [Channel2] (only if not used for jog!)
};

// Scratch feel
TerminalMix4._scratch = {
  // With Teensy RAW_PER_TICK ≈ 8: 4096 / 8 ≈ 512 ticks/rev
  resolution: 4000,
  rpm: 33 + 1/3,
  alpha: 1.0 / 8.0,
  beta:  (1.0 / 8.0) / 32.0
};

TerminalMix4._idleTimers = {}; // per-deck idle timers

// Lifecycle
TerminalMix4.init = function (id, debugging) {};
TerminalMix4.shutdown = function () {};

// Helpers
TerminalMix4._deckFromGroup = function (group) { return script.deckFromGroup(group); };

// 7-bit encoder (binary offset) → signed delta
// 65..127 => +1..+63, 63..1 => -1..-63, 64 => 0
TerminalMix4._deltaFromValue = function (value) {
  return (value > 63) ? (value - 128) : value;
};

TerminalMix4._toUnipolar = function (val) { return val / 127.0; };
TerminalMix4._toBipolar  = function (val) { return (val - 64.0) / 64.0; };

TerminalMix4._applyKnob = function (cfg, group, value) {
  var v = (cfg.mode === "bipolar") ? TerminalMix4._toBipolar(value) : TerminalMix4._toUnipolar(value);
  engine.setValue(group, cfg.control, v);
};

TerminalMix4._ensureScratchEnabled = function (deck) {
  if (!engine.isScratching(deck)) {
    engine.scratchEnable(
      deck,
      TerminalMix4._scratch.resolution,
      TerminalMix4._scratch.rpm,
      TerminalMix4._scratch.alpha,
      TerminalMix4._scratch.beta
    );
  }
};

TerminalMix4._armIdleDisable = function (deck, ms) {
  if (TerminalMix4._idleTimers[deck]) engine.stopTimer(TerminalMix4._idleTimers[deck]);
  TerminalMix4._idleTimers[deck] = engine.beginTimer(ms, function () {
    if (engine.isScratching(deck)) engine.scratchDisable(deck);
  }, true); // single-shot
};

// Optional knob handlers (only if you’ve bound them in XML)
TerminalMix4.knob9 = function (channel, control, value, status, group) {
  TerminalMix4._applyKnob(TerminalMix4.ccTargets.knob9, group, value);
};
TerminalMix4.knob11 = function (channel, control, value, status, group) {
  TerminalMix4._applyKnob(TerminalMix4.ccTargets.knob11, group, value);
};

// Jog wheel handlers — match XML bindings
TerminalMix4.jogWheel1 = function (channel, control, value, status, group) {
  var deck = TerminalMix4._deckFromGroup(group);
  var delta = TerminalMix4._deltaFromValue(value);
  if (!delta) return;
  TerminalMix4._ensureScratchEnabled(deck);
  engine.scratchTick(deck, delta * 0.8);
  TerminalMix4._armIdleDisable(deck, 150); // auto-disable after 150 ms idle
};

TerminalMix4.jogWheel2 = function (channel, control, value, status, group) {
  var deck = TerminalMix4._deckFromGroup(group);
  var delta = TerminalMix4._deltaFromValue(value);
  if (!delta) return;
  TerminalMix4._ensureScratchEnabled(deck);
  engine.scratchTick(deck, delta * 0.8);
  TerminalMix4._armIdleDisable(deck, 150);
};

// Optional touch handlers (if you later bind a touch Note/CC)
/*
TerminalMix4.wheelTouch1 = function (channel, control, value, status, group) {
  var deck = TerminalMix4._deckFromGroup(group);
  if (value > 0) {
    engine.scratchEnable(deck,
      TerminalMix4._scratch.resolution,
      TerminalMix4._scratch.rpm,
      TerminalMix4._scratch.alpha,
      TerminalMix4._scratch.beta);
  } else {
    engine.scratchDisable(deck);
  }
};

TerminalMix4.wheelTouch2 = function (channel, control, value, status, group) {
  var deck = TerminalMix4._deckFromGroup(group);
  if (value > 0) {
    engine.scratchEnable(deck,
      TerminalMix4._scratch.resolution,
      TerminalMix4._scratch.rpm,
      TerminalMix4._scratch.alpha,
      TerminalMix4._scratch.beta);
  } else {
    engine.scratchDisable(deck);
  }
};
*/