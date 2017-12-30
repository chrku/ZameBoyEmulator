# Zameboy Emulator
Small personal project to create a working gameboy emulator in C.

## Foreword

I wrote this as a programming exercise in C using libSDL2 for the graphics.
This emulates the Gameboy CPU (DMG), PPU,
interrupt controller, joypad, MBC1 and timers. It passes all of the instruction tests,
but fails some timing tests at the moment, which I still need to fix. Nevertheless,
it emulates a lot of games, but is not complete, lacking the APU, MBC 2, 3, 5
and serial interface and has the aforementioned timing issues.

It plays most games fairly well, however.

## CPU:

Still needs work on the timing of some instructions and interrupts. I haven't implemented
any of the more obscure nuances yet. 

Instructions are decoded using a jump table for normal instructions and algorithmically
for prefix-CB instructions. The CPU uses binary interpretation; I also plan on writing 
an emulator, probably not for the DMG, that uses JIT (Dynamic recompilation) for AMD64.

Everything runs synchronously off the CPU clock.

## PPU: 

Is updated based on CPU clock. Still needs proper sprite priority. OAM bug is not 
emulated yet, neither are the access restrictions.

## APU: 

Needs to be implemented.

## TODO:

- Clean the code up a little.
- Add the APU.
- Fix the timing issues.
- Add various hardware quirks for more accurate emulation.
- Add link cable.
- Add more MBC varieties.
