#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"
#include "include/stepper_controller.hpp" // We'll define this later
#include "include/ui_manager.hpp"      // We'll define this later

// --- Global System State ---
enum SystemState {
    STATE_IDLE,
    STATE_SELECT_BAND,
    STATE_FINE_TUNE,
    STATE_SAVING
};

volatile SystemState currentState = STATE_IDLE;
volatile long currentStepPos = 0;
volatile long targetStepPos = 0;

// --- Core 1: Dedicated Motor Control ---
// Core 1 stays in a tight loop to ensure pulse timing is rock solid.
void core1_entry() {
    StepperController motor(12, 13); // Step Pin 12, Dir Pin 13
    
    while (1) {
        if (currentStepPos != targetStepPos) {
            motor.moveTo(targetStepPos, currentStepPos);
        }
        tight_loop_contents(); // Optimization hint for compiler
    }
}

// --- Core 0: UI, I2C, and Logic ---
int main() {
    stdio_init_all();
    
    // Initialize I2C for the Desk Box (Display + Encoder)
    UIManager ui(GP4, GP5); // SDA, SCL
    
    // Initialize ADC for SWR Bridge
    adc_init();
    adc_gpio_init(26); // Forward Power
    adc_gpio_init(27); // Reflected Power
    
    // Launch Motor Control on Core 1
    multicore_launch_core1(core1_entry);

    while (1) {
        // 1. Poll the I2C Desk Box for Input
        UIEvent event = ui.poll();

        // 2. The State Machine
        switch (currentState) {
            case STATE_IDLE:
                if (event == EVENT_ENCODER_CLICK) {
                    currentState = STATE_SELECT_BAND;
                    ui.displayMenu("Select Band");
                }
                break;

            case STATE_SELECT_BAND:
                if (event == EVENT_ENCODER_ROTATE) {
                    // Update targetStepPos based on presets
                    targetStepPos = ui.getBandPresetSteps();
                }
                if (event == EVENT_ENCODER_CLICK) {
                    currentState = STATE_FINE_TUNE;
                }
                break;

            case STATE_FINE_TUNE:
                if (event == EVENT_ENCODER_ROTATE) {
                    // Micro-adjust targetStepPos
                    targetStepPos += ui.getRotationDelta();
                }
                if (event == EVENT_LONG_PRESS) {
                    currentState = STATE_SAVING;
                }
                break;

            case STATE_SAVING:
                // Update the local struct with the current step count
                myPresets.positions[currentBandIndex] = targetStepPos;
                
                // Commit the whole struct to flash
                save_presets_to_flash(myPresets);
                
                currentState = STATE_IDLE;
                ui.displayMessage("Memory Saved");
                break;
        }

        // 3. Update SWR Readings on Screen
        float swr = ui.calculateSWR(adc_read_fwd(), adc_read_rev());
        ui.updateSWRBar(swr);

        sleep_ms(10); // Throttle Core 0 loop
    }

    return 0;
}