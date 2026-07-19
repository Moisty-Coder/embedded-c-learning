/*
Day 17: The Power Rail Monitor

Scenario:
In green power systems — solar inverters, BMS controllers, MPPT chargers — multiple power rails supply different subsystems at different voltages.
A dedicated monitoring circuit watches each rail and packs the status of all rails into a single 32-bit status register for the main controller to read.
Your firmware must decode this packed register, extract per-rail status fields, and make system-level decisions from the result.
This is a step up from Day 16 — instead of single-bit flags, you're now working with multi-bit fields packed into a single register.
This is how real power management ICs like the Texas Instruments TPS65xx family report rail status.

The Register Layout
You are working with a 32-bit Power Rail Status Register (uint32_t). It is divided into four 8-bit fields, one per power rail:
Bits        Rail        Description
7:0         RAIL_A      3.3V logic rail
15:8        RAIL_B      5.0V peripheral rail
23:16       RAIL_C      12V gate drive rail
31:24       RAIL_D      48V main power rail

Each 8-bit rail field is itself divided into sub-fields:
Bits within field   Name            Width       Description
1:0                 STATUS          2 bits      00=OK, 01=WARN, 10=FAULT, 11=CRITICAL
3:2                 CURRENT_LOAD    2 bits      00=IDLE, 01=LOW, 10=MED, 11=HIGH
5:4                 VOLTAGE_BAND    2 bits      00=NOMINAL, 01=HIGH, 10=LOW, 11=INVALID
7:6                 RESERVED        2 bits      Must always be 00

Your Task
Define a struct called RailStatus with these fields:
-   uint8_t status — 2-bit STATUS field (0–3)
-   uint8_t current_load — 2-bit CURRENT_LOAD field (0–3)
-   uint8_t voltage_band — 2-bit VOLTAGE_BAND field (0–3)

Define a struct called PowerRailRegister with:
-   uint32_t rail_register — the full 32-bit packed register
-   RailStatus rails[4] — decoded status for each of the 4 rails

Then write 5 functions:

1.  A function that packs a single rail's fields into its 8-bit slot — takes a RailStatus pointer,
    validates that RESERVED bits are never set, builds the 8-bit value by combining STATUS, CURRENT_LOAD, and VOLTAGE_BAND using shifts and OR,
    then writes it into the correct 8-bit position of rail_register using a shift determined by the rail index (0–3).

2.  A function that extracts a single rail's fields from the register — takes a PowerRailRegister pointer and a rail index (0–3).
    Isolates the correct 8-bit field using a mask and shift, then extracts each 2-bit sub-field into the corresponding RailStatus field.

3.  A function that scans all 4 rails and returns the highest severity STATUS found — takes a const PowerRailRegister pointer and
    writes the result through a uint8_t * output parameter. Priority: CRITICAL(3) > FAULT(2) > WARN(1) > OK(0).

4.  A function that prints the full register in binary — prints the 32-bit register MSB-first with a space separator between each 8-bit group for readability.

5.  A function that prints the decoded report — for each rail prints its STATUS, CURRENT_LOAD, and VOLTAGE_BAND as human-readable labels,
    then prints the system-level worst status.

In main(), ask the user to enter the STATUS, CURRENT_LOAD, and VOLTAGE_BAND for each of the 4 rails (validate all inputs 0–3 and reject RESERVED combinations),
pack them into the register, extract them back out, find the worst status, and print the full report.

Field Labels for Printing:
STATUS: OK, WARN, FAULT, CRITICAL
CURRENT_LOAD: IDLE, LOW, MED, HIGH
VOLTAGE_BAND: NOMINAL, HIGH, LOW, INVALID

Core C Concepts Being Tested:
-   Multi-bit field extraction using masks and shifts
-   Bit field packing — writing multi-bit values into a register
-   Building and applying masks for fields wider than 1 bit
-   Combining bit manipulation with struct-based register modeling
-   Fixed-width types throughout
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <limits.h>

// Structs
typedef struct RailStatus
{
    uint8_t status;
    uint8_t current_load;
    uint8_t voltage_band;
} RailStatus;

typedef struct PowerRailRegister
{
    uint32_t rail_register;
    RailStatus rails[4];
} PowerRailRegister;

// Enums
typedef enum RailIndex
{
    RAIL_A = 0,
    RAIL_B,
    RAIL_C,
    RAIL_D
} RailIndex;

typedef enum Field
{
    STATUS = 0,
    CURRENT_LOAD,
    VOLTAGE_BAND,
} Field;

typedef enum StatusLabel
{
    STATUS_FIRST = 0,
    OK = 0,
    WARN,
    FAULT,
    CRITICAL,
    STATUS_LAST = CRITICAL
} StatusLabel_t;

typedef enum CurrentLoadLabel
{
    CURRENTLOAD_FIRST = 0,
    IDLE = 0,
    LOW_CURRENT,
    MED_CURRENT,
    HIGH_CURRENT,
    CURRENTLOAD_LAST = HIGH_CURRENT
} CurrentLoadLabel_t;

typedef enum VoltageBandLabel
{
    VOLTAGEBAND_FIRST = 0,
    NOMINAL = 0,
    HIGH_VOLTAGE,
    LOW_VOLTAGE,
    INVALID,
    VOLTAGEBAND_LAST = INVALID
} VoltageBandLabel_t;

// Constants
static const uint8_t FIELD_COUNT = 3U;
static const uint8_t RESERVED_BIT_MASK = (2 << 6); // 1100 0000

// Prototypes
void printCurrentRegistry(size_t index);
void printValidInputs(uint8_t field_selected);
void inputParsing(uint8_t *field, uint8_t *field_selected, uint8_t min, uint8_t max);
void inputRailStatus(RailStatus *rails, size_t index);


void setRailFields(PowerRailRegister *registry, RailStatus *rail, size_t index);

void printBinary_8bit(const uint8_t buffer_field);
void printBinary_32bit(const uint32_t registry);

    int main(void)
{
    printf("\nPOWER RAIL MONITOR\n");
    printf("====================\n\n");

    //* Declarations
    PowerRailRegister registry = {0};
    size_t size_registry = sizeof(registry.rails) / sizeof(registry.rails[0]);

    //* Input loop for rail A,B,C,D
    printf("~~~USER INPUT~~~\n\n");
    for (size_t i = 0; i < size_registry; i++)
    {
        inputRailStatus(&registry.rails[i], i);
        printf("\n");
    }


    //* Function 1
    for (size_t i = 0; i < size_registry; i++)
    {
        setRailFields(&registry, &registry.rails[i], i);
    }

    //* Debugging
    
    return 0;
}

void printCurrentRegistry(size_t index)
{
    switch (index)
    {
    case RAIL_A:
        printf("RAIL A:\n");
        break;
    case RAIL_B:
        printf("RAIL B:\n");
        break;
    case RAIL_C:
        printf("RAIL C:\n");
        break;
    case RAIL_D:
        printf("RAIL D:\n");
        break;
    default:
        break;
    }
}

void printValidInputs(uint8_t field_selected)
{
    switch (field_selected)
    {
    case STATUS:
        printf("[STATUS] 0-OK, 1-WARN, 2-FAULT, 3-CRITICAL\n");
        printf("PLEASE INPUT A VALID VALUE: ");
        break;
    case CURRENT_LOAD:
        printf("[CURRENT LOAD] 0-IDLE, 1-LOW, 2-MED, 3-HIGH\n");
        printf("PLEASE INPUT A VALID VALUE: ");
        break;
    case VOLTAGE_BAND:
        printf("[VOLTAGE BAND] 0-NOMINAL, 1-HIGH, 2-LOW, 3-INVALID\n");
        printf("PLEASE INPUT A VALID VALUE: ");
        break;
    default:
        break;
    }
}

//* Input Parsing Function
void inputParsing(uint8_t *field, uint8_t *field_selected, uint8_t min, uint8_t max)
{
    while (true)
    {
        printValidInputs(*field_selected);
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
            while((c = getchar()) != '\n' && c != EOF);
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
        if (check < min || check > max)
        {
            printf("[ERROR] INPUT VALUE OUT OF RANGE.\n\n");
            continue;
        }
        *field = (uint8_t)check;
        *field_selected = *field_selected + 1;
        break;
    }
}

//* Input Function
void inputRailStatus(RailStatus *rails, size_t index)
{
    printCurrentRegistry(index); // Prints which rail is currently being registered
    uint8_t field_selected = 0;
    
    inputParsing(&rails->status, &field_selected, STATUS_FIRST, STATUS_LAST);
    printf("\n");
    inputParsing(&rails->current_load, &field_selected, CURRENTLOAD_FIRST, CURRENTLOAD_LAST);
    printf("\n");
    inputParsing(&rails->voltage_band, &field_selected, VOLTAGEBAND_FIRST, VOLTAGEBAND_LAST);
}

//* Function 1 - 
void setRailFields(PowerRailRegister *registry, RailStatus *rail, size_t index)
{
    uint8_t field_buffer = 0U;
    for (uint8_t i = 0U; i < FIELD_COUNT; i++)
    {
        uint8_t shift_increment = 0U;
        if (i == STATUS)
        {
            field_buffer = field_buffer | rail->status;

        }
        else if (i == CURRENT_LOAD)
        {
            shift_increment = (1 << i); // 1 means it shifts 1 time
            field_buffer = field_buffer | (rail->current_load << shift_increment);
        }
        else if (i == VOLTAGE_BAND)
        {
            shift_increment = (1 << i); // 1 means it shifts 1 time
            field_buffer = field_buffer | (rail->voltage_band << shift_increment);
        }
    }

    printBinary_8bit(field_buffer);
    printf("\n\n"); // DEBUG

    // Checking if reserved bits are 00
    if ((field_buffer & RESERVED_BIT_MASK) != 0)
    {
        printf("\n\n[ERROR] RESERVED BITS ARE SET. ENDING PROGRAM.\n\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        uint8_t shift_increment = (uint8_t)index * CHAR_BIT;
        registry->rail_register = ((uint32_t)field_buffer << shift_increment) | registry->rail_register; // Adding the new bits to the old bits
        printBinary_32bit(registry->rail_register);
        printf("\n\n");
    }
}

// *Debugging helper
void printBinary_8bit(const uint8_t buffer_field)
{
    size_t size = sizeof(buffer_field) * CHAR_BIT;
    for (size_t i = 0; i < size; i++)
    {
        uint8_t mask = (1 << (size - (i + 1)));
        if ((buffer_field & mask) == mask)
        {
            printf("1");
        }
        else
        {
            printf("0");
        }
    }
}
void printBinary_32bit(const uint32_t registry)
{
    size_t size = sizeof(registry) * CHAR_BIT;
    for (size_t i = 0; i < size; i++)
    {
        uint32_t mask = (1 << (size - (i + 1)));
        if ((registry & mask) == mask)
        {
            printf("1");
        }
        else
        {
            printf("0");
        }
    }
}
