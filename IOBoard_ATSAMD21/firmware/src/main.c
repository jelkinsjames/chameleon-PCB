/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <stdio.h>

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
uint8_t ENC0_val;
uint8_t ENC1_val;
uint8_t ENC2_val;
uint8_t ENC3_val;

_Bool ENC0_last;
_Bool ENC1_last;
_Bool ENC2_last;
_Bool ENC3_last;
_Bool ENC0_A_curr;
_Bool ENC1_A_curr;
_Bool ENC2_A_curr;
_Bool ENC3_A_curr;
_Bool ENC0_B_curr;
_Bool ENC1_B_curr;
_Bool ENC2_B_curr;
_Bool ENC3_B_curr;
bool ENC0_SW;
bool ENC1_SW;
bool ENC2_SW;
bool ENC3_SW;


void ENC0_update (void) { // I must have set my GPIO with the wrong names, for some reason this encoder was incrementing counter clockwise instead of clockwise like the others, so this code is slightly different. 
                          // instead of if A != B increment, we want A == B increment. I don't want to deal with renaming my GPIO when this is an easy fix 
    ENC0_A_curr = ENC0_A_Get();
    if(ENC0_last != ENC0_A_curr) {
        
        ENC0_B_curr = ENC0_B_Get();
        if (ENC0_A_curr == ENC0_B_curr && ENC0_val < 255) {
            ENC0_val++;
            
        } else if (ENC0_A_curr != ENC0_B_curr && ENC0_val > 0) {
            ENC0_val--;
        }
    }
    ENC0_last = ENC0_A_curr;
}

void ENC1_update (void) {
    ENC1_A_curr = ENC1_A_Get();
    if(ENC1_last != ENC1_A_curr) {
        ENC1_B_curr = ENC1_B_Get();
        if (ENC1_A_curr == ENC1_B_curr && ENC1_val > 0) {
            ENC1_val--;
        } else if (ENC1_A_curr != ENC1_B_curr && ENC1_val < 255) {
            ENC1_val++;
        }
    }

    ENC1_last = ENC1_A_curr;
}

void ENC2_update (void) {
    ENC2_A_curr = ENC2_A_Get();
    if(ENC2_last != ENC2_A_curr) {
        ENC2_B_curr = ENC2_B_Get();
        if (ENC2_A_curr == ENC2_B_curr && ENC2_val > 0) {
            ENC2_val--;
        } else if (ENC2_A_curr != ENC2_B_curr && ENC2_val < 255) {
            ENC2_val++;
        }
    }

    ENC2_last = ENC2_A_curr;
}

void ENC3_update (void) {
    ENC3_A_curr = ENC3_A_Get();
    if(ENC3_last != ENC3_A_curr) {
        ENC3_B_curr = ENC3_B_Get();
        if (ENC3_A_curr == ENC3_B_curr && ENC3_val > 0) {
            ENC3_val--;
        } else if (ENC3_A_curr != ENC3_B_curr && ENC3_val <255) {
            ENC3_val++;
        }
    }
    ENC3_last = ENC3_A_curr;
}

void ENC_update() {
    ENC0_update();
    ENC1_update();
    ENC2_update();
    ENC3_update();
}

void LED_update(uint8_t state) {
    if(state / 8 > 0) {
        LED3_Set();
    } else {
        LED3_Clear();
    }
    state = state % 8;
    
    if(state / 4 > 0) {
        LED2_Set();
    } else {
        LED2_Clear();
    }
    state = state % 4;
    
    if(state / 2 > 0) {
        LED1_Set();
    } else {
        LED1_Clear();
    }
    state = state % 2;
    
    if (state > 0) {
        LED0_Set();
    } else {
        LED0_Clear();
    }
}

void Button_update() {
    if(ENC0_SW_Get() && !ENC0_SW){
        ENC0_SW = 1;
    }
    if(ENC1_SW_Get() && !ENC1_SW){
        ENC1_SW = 1;
    }
    if(ENC2_SW_Get() && !ENC2_SW){
        ENC2_SW = 1;
    }
    if(ENC3_SW_Get() && !ENC3_SW){
        ENC3_SW = 1;
    }
}

uint8_t getButtonState() {
    return (8*ENC3_SW) + (4*ENC2_SW) + (2*ENC1_SW) + ENC0_SW;
    
}

uint8_t getSwitchState() {
    return (8 * SW3_Get()) + (4 * SW2_Get()) + (2 * SW1_Get()) + SW0_Get();
}

uint8_t rxReg;
uint8_t txReg;

bool I2C_Callback(SERCOM_I2C_SLAVE_TRANSFER_EVENT event, uintptr_t contextHandle) {
    
    
    switch(event) {
        case SERCOM_I2C_SLAVE_TRANSFER_EVENT_RX_READY:
            rxReg = SERCOM0_I2C_ReadByte();
            // logic for filling txReg
            switch(rxReg) {
                case 0x00: // encoder 0, reset value
                    ENC0_val = 0;
                    txReg = ENC0_val;
                    break;
                case 0x01: // encoder 0, read value
                    txReg = ENC0_val;
                    break;
                    
                case 0x10: // encoder 1, reset value
                    ENC1_val = 0;
                    txReg = ENC1_val;
                    break;
                case 0x11: // encoder 1, read value
                    txReg = ENC1_val;
                    break;
                    
                case 0x20: // encoder 2, reset value
                    ENC2_val = 0;
                    txReg = ENC2_val;
                    break;
                case 0x21: // encoder 2, read value
                    txReg = ENC2_val;
                    break;
                    
                case 0x30: // encoder 3, reset value
                    ENC3_val = 0;
                    txReg = ENC3_val;
                    break;
                case 0x31: // encoder 3, read value
                    txReg = ENC3_val;
                    break;
                
                case 0x40:
                    txReg = getSwitchState();
                    break;
                case 0x50:
                    txReg = getButtonState();
                    ENC0_SW = 0;
                    ENC1_SW = 0;
                    ENC2_SW = 0;
                    ENC3_SW = 0;
                    break;
                default:
                    break;                        
            }
            
            if(rxReg >> 4 == 0x08) { // if the most significant 4 bits are 1000, then we want to update the LEDs
                LED_update(rxReg % 0x80);
            }
            
            break;
        
        case SERCOM_I2C_SLAVE_TRANSFER_EVENT_TX_READY:
            SERCOM0_I2C_WriteByte(txReg);
            txReg = 0; // reset transmission register
            break;
            
        default:
            break;
         
    }
    
    return 1;
}

// first four bits of write are going to define what we want to read or write
// 4'b0000 - encoder 0
// 4'b0001 - encoder 1
// 4'b0010 - encoder 2
// 4'b0011 - encoder 3
//              second half of byte will have reset or read command 0x_1 for read, 0x_0 for reset.
// 4'b0100 - switch state
// 4'b0101 - button state
// 4'b1000 - LEDS, second half of byte will be state of each LED
int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    ENC0_last = ENC0_A_Get();
    ENC1_last = ENC1_A_Get();
    ENC2_last = ENC2_A_Get();
    ENC3_last = ENC3_A_Get();
    
    ENC0_SW = 0;
    ENC1_SW = 0;
    ENC2_SW = 0;
    ENC3_SW = 0;
    
    SERCOM0_I2C_CallbackRegister(I2C_Callback, 0);
    
    
    while ( true )
    {
        ENC_update();
        Button_update();
        for(int i = 0; i < 5000; i++){} // debounce delay
    }
    
    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

