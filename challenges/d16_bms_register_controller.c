/*
Day 16: BMS Register Controller

Scenario:
You're writing firmware for a Battery Management System that communicates with a protection IC over a register-based interface. 
In real hardware, BMS protection ICs (like the Texas Instruments BQ76940 or Analog Devices LTC6811) expose their internal state through 
8-bit or 16-bit registers — each bit or group of bits encodes a specific piece of information: a fault flag, a protection status, a configuration setting.
Your firmware must read, decode, set, and clear individual bits within these registers — without disturbing any of the other bits. 
This is the most fundamental hardware interfacing skill in all of embedded firmware.
------------------------------------------------------------------------------------------------------------------------------------------
Task:
Your Task
You will simulate a BMS Protection Status Register — a single uint16_t value where each bit has a specific hardware meaning:
Bit     Name        Meaning when SET (1)
0       OV          Cell Overvoltage detected  
1       UV          Cell Undervoltage detected 
2       OCD         Overcurrent in Discharge
3       OCC         Overcurrent in Charge
4       SCD         Short Circuit in Discharge
5       TEMP_HOT    Overtemperature detected
6       TEMP_COLD   Undertemperature detected
7       RESERVED    Unused — must always remain 0
8–15    CONFIG      Upper byte — pack configuration flags

Define a struct called BMSRegister with these fields:
-   uint16_t protection_reg — the raw 16-bit protection status register
-   uint8_t fault_count — number of active faults in the lower byte
-   uint8_t config_byte — the upper configuration byte (bits 8–15)

Then write 5 functions:

1.  A function that sets a specific bit — takes a BMSRegister pointer and a uint8_t bit_position. 
    Sets that bit in protection_reg using bitwise OR and a shifted mask. 
    Refuses to set bit 7 (RESERVED) and prints a warning if attempted.

2.  A function that clears a specific bit — takes a BMSRegister pointer and a uint8_t bit_position. 
    Clears that bit using AND with an inverted mask. 
    Refuses to clear bits 8–15 (CONFIG byte — handled separately) and prints a warning.

3.  A function that checks if a specific bit is set — takes a const BMSRegister pointer and 
    a uint8_t bit_position. 
    Returns 1 if the bit is set, 0 if not. No side effects.

4.  A function that counts active faults — scans bits 0–6 of protection_reg 
    (excluding RESERVED bit 7) and writes the total number of set 
    bits into fault_count through the pointer.

5.  A function that extracts the config byte — isolates bits 8–15 of protection_reg and 
    writes the result into config_byte through the pointer. 
    Use a right shift after masking.

In main(), simulate a register sequence:

-   Start with protection_reg = 0x0000
-   Set bits based on user input (ask which fault bits to activate, 0–6)
-   Set the config byte to 0xA5 (upper byte)
-   Count faults and extract config
-   Print the full register state in binary, the fault count, and the config byte in hex
-   Then clear one user-specified fault bit and recount
------------------------------------------------------------------------------------------------------------------------------------------
Core C Concepts Being Tested:
-   Bitwise AND & for reading/masking bits
-   Bitwise OR | for setting bits
-   Bitwise XOR ^ for toggling bits
-   Bitwise NOT ~ for clearing bits
-   Left shift << for building masks
-   Fixed-width types (uint8_t, uint16_t) from <stdint.h>
-   Struct-based register modeling

NOTE:
Printing in Binary*
You'll need to print uint16_t values in binary format. 
C has no %b format specifier — you'll need to write a small function that loops through each bit using shifts and masking to print 1 or 0.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>

typedef struct BMSRegister
{
    uint16_t protection_reg;    
    uint8_t fault_count;
    uint8_t config_byte;
} BMSRegister;

typedef enum State
{
    OV = 0, //* Based on selection input
    UV,
    OCD,
    OCC,
    SCD,
    TEMP_HOT,
    TEMP_COLD,
    RESERVED,
    NONE
} State;

// Constant
static const uint8_t MAX_CYCLE = 6;

// Prototypes
void setBitPosition(BMSRegister *registry, uint8_t bit_position, uint8_t *set_bits, uint8_t cycle_index);
void setRegistry(BMSRegister *registry, uint8_t bit_position, uint8_t *set_bits, uint8_t cycle_index);
void cycleCheck(bool *cycleCheck, uint8_t *cycle_index);
void setConfig(BMSRegister *registry);
void updateSetBits(uint8_t *set_bits, const uint8_t check);
void clearFault(BMSRegister *registry, uint8_t bit_position, uint8_t *set_bits, bool *clear_fault_check, uint8_t *cleared_bits);
void faultCounter(BMSRegister *registry, const uint8_t bit_position);
void printBinary(const BMSRegister *registry);
void printSetorClearedBits(const uint8_t *set_bits);
void printReport(const BMSRegister *registry, const uint8_t *set_bits, const bool clear_fault_check, const uint8_t *cleared_bits);

int main(void)
{
    printf("\nBMS REGISTER CONTROLLER\n");
    printf("=======================\n\n");

    // Declarations
    BMSRegister registry = {0};
    uint8_t bit_position = 0;
    bool cycle_check = true;
    uint8_t cycle_index = 0;
    uint8_t set_bits[CHAR_BIT];
    bool clear_fault_check = false;
    uint8_t cleared_bits[2] = {NONE, NONE};

    for (uint8_t i = 0; i < CHAR_BIT; i++)
    {
        set_bits[i] = NONE; //* Initializing all to NONE first
    }
    
    while (cycle_check && cycle_index <= MAX_CYCLE)
    {
        setRegistry(&registry, bit_position, set_bits, cycle_index);
        cycleCheck(&cycle_check, &cycle_index);
    }

    // Setting config byte
    registry.config_byte = 0xA5;
    setConfig(&registry);

    for (uint8_t i = 0; i < CHAR_BIT ; i++) //* CHAR_BIT = 8
    {
        uint8_t mask = (1 << i);
        faultCounter(&registry, mask);
    }

    printf("REPORT 1:\n");
    printReport(&registry, set_bits, clear_fault_check, cleared_bits); // First report

    if ((registry.protection_reg >> CHAR_BIT) == registry.config_byte) // Check if registry is empty  
    {
        return 0;
    }

    //* Clearing One Fault
    clearFault(&registry, bit_position, set_bits, &clear_fault_check, cleared_bits);
    registry.fault_count = 0;
    for (uint8_t i = 0; i < CHAR_BIT; i++) //* Recalculating fault_count
    {
        uint8_t mask = (1 << i);
        faultCounter(&registry, mask);
    }

    printf("\nREPORT 2:\n");
    printReport(&registry, set_bits, clear_fault_check, cleared_bits); // Second report

    return 0;
}

void setBitPosition(BMSRegister *registry, uint8_t bit_position, uint8_t *set_bits, uint8_t cycle_index)
{
    registry->protection_reg = registry->protection_reg | (1 << bit_position);
    set_bits[cycle_index] = bit_position; //* Setting the bit set in order
}

void setRegistry(BMSRegister *registry, uint8_t bit_position, uint8_t *set_bits, uint8_t cycle_index)
{
    while (true)
    {
        printf("[NOTE] PRESS ""ENTER"" TO CANCEL SELECTION.\n");
        printf("PLEASE INPUT A VALID VALUE TO ACTIVATE A FAULT BIT (0-7): ");
        char buffer[50];
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("[ERROR] INPUT MUST BE A VALID VALUE.\n\n");
            continue;
        }

        if (strchr(buffer, '\n') == NULL && feof(stdin) == false)
        {
            printf("[ERROR] INPUT IS TOO LONG.\n\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF); 
            continue;
        }

        char *endptr;
        errno = 0; 

        const intmax_t check = strtoimax(buffer, &endptr, 10);

        if (errno == ERANGE)
        {
            printf("[ERROR] VALUE IS OUT OF RANGE.\n\n");
            continue;
        }

        if (buffer == endptr)
        {
            printf("CANCELLING SELECTION...\n\n");
            break;
        }

        while (*endptr != '\0' && isspace((unsigned char)*endptr))
        {
            endptr++;
        }

        if (*endptr != '\0')
        {
            printf("[ERROR] TRAILING CHARACTERS DETECTED.\n\n");
            continue;
        }
        
        if (check < OV || check > RESERVED)
        {
            printf("[ERROR] INVALID VALUE RANGE\n\n");
            continue;
        }

        if (check == RESERVED)
        {
            printf("[ERROR] RESERVED BIT, PLEASE TRY AGAIN.\n\n");
            continue;
        }

        if (check > UINT32_MAX)
        {
            printf("[ERROR] VALUE EXCEEDS MAXIMUM 32-BIT CAPACITY (%u).\n\n", UINT32_MAX);
            continue;
        }

        else
        {
            bit_position = (uint8_t)check;
            setBitPosition(registry, bit_position, set_bits, cycle_index);
            break;
        }
    }
}

void cycleCheck(bool *cycleCheck, uint8_t *cycle_index)
{
    while (true)
    {
        printf("CONTINUE? (1-YES), (0-NO): ");
        char buffer[50];
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("[ERROR] INPUT MUST BE A VALID VALUE.\n\n");
            continue;
        }

        if (strchr(buffer, '\n') == NULL && feof(stdin) == false)
        {
            printf("[ERROR] INPUT IS TOO LONG.\n\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF); 
            continue;
        }

        char *endptr;
        errno = 0; 

        const intmax_t check = strtoimax(buffer, &endptr, 10);

        if (errno == ERANGE)
        {
            printf("[ERROR] VALUE IS OUT OF RANGE.\n\n");
            continue;
        }

        if (buffer == endptr)
        {
            printf("[ERROR] INPUT MUST BE A VALID VALUE.\n\n");
            continue;
        }

        while (*endptr != '\0' && isspace((unsigned char)*endptr))
        {
            endptr++;
        }

        if (*endptr != '\0')
        {
            printf("[ERROR] TRAILING CHARACTERS DETECTED.\n\n");
            continue;
        }
        
        if (check < 0 || check > 1)
        {
            printf("[ERROR] INVALID VALUE RANGE\n\n");
            continue;
        }

        if (check > UINT32_MAX)
        {
            printf("[ERROR] VALUE EXCEEDS MAXIMUM 32-BIT CAPACITY (%u).\n\n", UINT32_MAX);
            continue;
        }

        else
        {
            printf("\n");
            if (check == 0)
            {
                *cycleCheck = false;
                break;
            }
            else
            {
                *cycle_index = *cycle_index + 1;
                break;
            }
        }
    }
}

void setConfig(BMSRegister *registry)
{
    uint16_t buffer_config = (registry->config_byte << 8); // 00000000 10100101 -> 10100101 00000000
    registry->protection_reg = registry->protection_reg | buffer_config;
}

void updateSetBits(uint8_t *set_bits, const uint8_t check)
{
    for (uint8_t i = 0; i < CHAR_BIT; i++)
    {
        if (set_bits[i] == check)
        {
            set_bits[i] = NONE;
            break;
        }
    }
    for (uint8_t i = 0; i < (CHAR_BIT - 1); i++) //* Recursion loop prevent NONE gaps in array
    {
        if (set_bits[i] == NONE)
        {
            if (set_bits[i+1] != NONE)
            {
                set_bits[i] = set_bits[i+1];
                set_bits[i+1] = NONE;
            }
        }
    }
}

void clearFault(BMSRegister *registry, uint8_t bit_position, uint8_t *set_bits, bool *clear_fault_check, uint8_t *cleared_bits)
{
    while (true)
    {
        printf("PLEASE SELECT A FLAG TO DISABLE (0-7): ");
        char buffer[50];
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("[ERROR] INPUT MUST BE A VALID VALUE.\n\n");
            continue;
        }

        if (strchr(buffer, '\n') == NULL && feof(stdin) == false)
        {
            printf("[ERROR] INPUT IS TOO LONG.\n\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        char *endptr;
        errno = 0;

        const intmax_t check = strtoimax(buffer, &endptr, 10);

        if (errno == ERANGE)
        {
            printf("[ERROR] VALUE IS OUT OF RANGE.\n\n");
            continue;
        }

        if (buffer == endptr)
        {
            printf("[ERROR] INPUT MUST BE A VALID VALUE.\n\n");
            continue;
        }

        while (*endptr != '\0' && isspace((unsigned char)*endptr))
        {
            endptr++;
        }

        if (*endptr != '\0')
        {
            printf("[ERROR] TRAILING CHARACTERS DETECTED.\n\n");
            continue;
        }

        if (check < OV || check > RESERVED)
        {
            printf("[ERROR] INVALID VALUE RANGE\n\n");
            continue;
        }

        if (check > UINT32_MAX)
        {
            printf("[ERROR] VALUE EXCEEDS MAXIMUM 32-BIT CAPACITY (%u).\n\n", UINT32_MAX);
            continue;
        }

        bool valid = false;
        for (uint8_t i = 0; i < CHAR_BIT; i++) // Check if the chosen bit is actually set
        {
            if (set_bits[i] == check)
            {
                valid = true;
                break;
            }
        }

        if (valid == false)
        {
            printf("[ERROR] CHOSEN BIT IS NOT AN ACTIVATED FAULT.\n\n");
            continue;
        }

        else
        {
            bit_position = (1 << check);
            registry->protection_reg = registry->protection_reg ^ bit_position;
            *clear_fault_check = true;
            cleared_bits[0] = (uint8_t)check;
            updateSetBits(set_bits, (uint8_t)check);
            break;
        }
    }
}

void faultCounter(BMSRegister *registry, const uint8_t bit_position)
{
    if ((registry->protection_reg & bit_position) == bit_position)
    {
        registry->fault_count ++;
        return;
    }
}

void printBinary(const BMSRegister *registry)
{
    // if ((registry->protection_reg & bit_position) == bit_position)
    size_t size = sizeof(registry->protection_reg) * CHAR_BIT;
    for (size_t i = 0; i < size; i++)
    {
        uint16_t mask = (1 << (size - (i+1)));
        if ((registry->protection_reg & mask) == mask)
        {
            printf("1");
        }
        else
        {
            printf("0");
        }
    }
}

void printSetorClearedBits(const uint8_t *set_bits)
{
    for (uint8_t i = 0; i < CHAR_BIT; i++)
    {
        if (set_bits[i] ==  NONE)
        {
            break;
        }
        
        switch (set_bits[i])
        {
        case OV:
            if (set_bits[i + 1] != NONE)
                printf("0 (OV), ");
            else
                printf("0 (OV)\n");
            break;
        case UV:
            if (set_bits[i + 1] != NONE)
                printf("1 (UV), ");
            else
                printf("1 (UV)\n");
            break;
        case OCD:
            if (set_bits[i + 1] != NONE)
                printf("2 (OCD), ");
            else
                printf("2 (OCD)\n");
            break;
        case OCC:
            if (set_bits[i + 1] != NONE)
                printf("3 (OCC), ");
            else
                printf("3 (OCC)\n");
            break;
        case SCD:
            if (set_bits[i + 1] != NONE)
                printf("4 (SCD), ");
            else
                printf("4 (SCD)\n");
            break;
        case TEMP_HOT:
            if (set_bits[i + 1] != NONE)
                printf("5 (TEMP_HOT), ");
            else
                printf("5 (TEMP_HOT)\n");
            break;
        case TEMP_COLD:
            if (set_bits[i + 1] != NONE)
                printf("6 (TEMP_COLD), ");
            else
                printf("6 (TEMP_COLD)\n");
            break;      
        default:
            break;
        }
    }
}

void printReport(const BMSRegister *registry, const uint8_t *set_bits, const bool clear_fault_check, const uint8_t *cleared_bits)
{
    //* Set bit printing
    printf("Set bits: ");
    printSetorClearedBits(set_bits);

    printf("Config byte: 0xA5\n");

    //* Print Binary
    printf("protection_reg binary: ");
    printBinary(registry);
    
    if (clear_fault_check == false)
        printf("\nFault Count: %u\n\n", registry->fault_count);

    if (clear_fault_check == true)
    {
        printf("\nCleared bit: ");
        printSetorClearedBits(cleared_bits);
        
        printf("New Fault Count: %u", registry->fault_count);
    }
}

