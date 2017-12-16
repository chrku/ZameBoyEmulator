
// Header for all things to do with GAMEBOY ROMS; i.e loading them from files

#ifndef ROM_H
#define ROM_H

#include <stdint.h>
#include <stdlib.h>

#define ROM_TOO_BIG 12

// Load a ROM from a file
/// @param ROM_MEM a memory segment to load the ROM into
/// @param mem_size the size of that memory
/// @param path path to the rom on disk
/// @return int an error code
//
int loadROM(uint8_t* ROM_mem, size_t mem_size, char* path);

#endif // ROM_H
