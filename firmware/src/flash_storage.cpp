#include "hardware/flash.h"
#include "hardware/sync.h"

// Set the target offset to the last 4KB sector of a 2MB flash
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

struct BandPresets {
    uint32_t magic_key;     // To verify the data is ours (e.g., 0xDEADBEEF)
    long positions[7];      // 7 bands: 40, 30, 20, 17, 15, 12, 10
};

void save_presets_to_flash(BandPresets &data) {
    // 1. Flash programming must happen with interrupts disabled
    uint32_t ints = save_and_disable_interrupts();

    // 2. Erase the sector before writing
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);

    // 3. Write the data (must be a multiple of FLASH_PAGE_SIZE, which is 256 bytes)
    // We cast our struct to uint8_t and ensure it fits in a page
    flash_range_program(FLASH_TARGET_OFFSET, (uint8_t *)&data, FLASH_PAGE_SIZE);

    restore_interrupts(ints);
}

void load_presets_from_flash(BandPresets &data) {
    // Flash is memory-mapped, so we can just pointer-access it
    const uint8_t *flash_target_contents = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);
    
    BandPresets *stored_data = (BandPresets *)flash_target_contents;

    // Check if the flash has been initialized before
    if (stored_data->magic_key == 0xDEADBEEF) {
        data = *stored_data;
    } else {
        // First time setup: set everything to 0 or mid-point
        data.magic_key = 0xDEADBEEF;
        for(int i=0; i<7; i++) data.positions[i] = 0;
    }
}