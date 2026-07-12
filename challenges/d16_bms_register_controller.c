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
    OV = 0,
    UV,
    OCD,
    OCC,
    SCD,
    TEMP_HOT,
    TEMP_COLD,
    RESERVED
} State;


// Prototypes
void setBitPosition(BMSRegister *registry, uint8_t bit_position);
void setRegistry(BMSRegister *registry, uint8_t bit_position);
void cycleCheck(bool *cycleCheck);
void setConfig(BMSRegister *registry);
void faultCounter(BMSRegister *registry, uint8_t bit_position);
char checkSetBit(const BMSRegister *registry, uint16_t bit_position);
void printReport(BMSRegister *registry, size_t size, char *binary);


int main(void)
{
    printf("BMS REGISTER CONTROLLER\n");
    printf("=======================\n\n");

    BMSRegister registry = {0};
    uint8_t bit_position = 0;
    bool cycle_check = true;

    while (cycle_check)
    {
        setRegistry(&registry, bit_position);
        cycleCheck(&cycle_check);
    }

    // Setting config byte
    registry.config_byte = 0xA5;
    setConfig(&registry);

    char binary[16] = {0};
    size_t size = sizeof(binary) / sizeof(binary[0]);

    for (uint8_t i = 0; i < CHAR_BIT ; i++)
    {
        uint8_t mask = (1 << i);
        faultCounter(&registry, mask);
    }

    for (size_t i = 0; i < size; i++)
    {
        uint16_t mask = (1 << i);
        binary[(size - 1) - i] = checkSetBit(&registry, mask);
    }
    
    printReport(&registry, size, binary);
    
    return 0;
}

void setBitPosition(BMSRegister *registry, uint8_t bit_position)
{
    switch (bit_position) // bit_position always clears to 0 each loop
    {
    case OV:
        registry->protection_reg = registry->protection_reg | 0x1;
        // 0000 0000
        // 0000 0001
        break;
    case UV:
        registry->protection_reg = registry->protection_reg| 0x2; 
        break;
    case OCD:
        registry->protection_reg = registry->protection_reg| 0x4;
        break;
    case OCC:
        registry->protection_reg = registry->protection_reg| 0x8;
        break;
    case SCD:
        registry->protection_reg = registry->protection_reg| 0x10;
        break;
    case TEMP_HOT:
        registry->protection_reg = registry->protection_reg| 0x20;
        break;
    case TEMP_COLD:
        registry->protection_reg = registry->protection_reg| 0x40;
        break;
    case RESERVED:
        printf("[WARNING] BIT 7 IS RESERVED.\n\n");
        break;
    default:
        break;
    }
} // TODO: Improve this rather than hard coding magic binary numbers

void setRegistry(BMSRegister *registry, uint8_t bit_position)
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

        if (check > UINT32_MAX)
        {
            printf("[ERROR] VALUE EXCEEDS MAXIMUM 32-BIT CAPACITY (%u).\n\n", UINT32_MAX);
            continue;
        }

        else
        {
            bit_position = (uint8_t)check;
            setBitPosition(registry, bit_position);
            break;
        }
    }

}

void cycleCheck(bool *cycleCheck)
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
                break;
        }
    }
}

void setConfig(BMSRegister *registry)
{
    uint16_t buffer_config = (registry->config_byte << 8); // 00000000 10100101 -> 10100101 00000000
    registry->protection_reg = registry->protection_reg | buffer_config;
}

void faultCounter(BMSRegister *registry, uint8_t bit_position)
{
    if ((registry->protection_reg & bit_position) == bit_position)
    {
        registry->fault_count ++;
        return;
    }
}

char checkSetBit(const BMSRegister *registry, uint16_t bit_position)
{
    if ((registry->protection_reg & bit_position) == bit_position)
    {
        return '1';
    }
    else
    {
        return '0';
    }
}

void printReport(BMSRegister *registry, size_t size, char *binary)
{
    // TODO: Set bits:
    // TODO: Config byte:
    printf("protection_reg binary: ");
    for (size_t i = 0; i < size; i++)
    {
        printf("%c", binary[i]);
    }
    printf("\nFault Count: %u", registry->fault_count);

    // TODO: Clear bit:
    // TODO: New Fault Count: 
}

