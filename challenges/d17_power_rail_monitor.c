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
    RAIL_FIRST = 0,
    RAIL_A = 0,
    RAIL_B,
    RAIL_C,
    RAIL_D,
    RAIL_LAST = RAIL_D,
    RAIL_COUNT
} RailIndex;

typedef enum Field
{
    FIELD_FIRST = 0,
    FIELD_STATUS = 0,
    FIELD_CURRENT_LOAD,
    FIELD_VOLTAGE_BAND,
    FIELD_LAST = FIELD_VOLTAGE_BAND,
    FIELD_COUNT
} Field_t;

typedef enum StatusLabel
{
    STATUS_FIRST = 0,
    STATUS_OK = 0,
    STATUS_WARN,
    STATUS_FAULT,
    STATUS_CRITICAL,
    STATUS_LAST = STATUS_CRITICAL,
} StatusLabel_t;

typedef enum CurrentLoadLabel
{
    CURRENTLOAD_FIRST = 0,
    CURRENTLOAD_IDLE = 0,
    CURRENTLOAD_LOW_CURRENT,
    CURRENTLOAD_MED_CURRENT,
    CURRENTLOAD_HIGH_CURRENT,
    CURRENTLOAD_LAST = CURRENTLOAD_HIGH_CURRENT
} CurrentLoadLabel_t;

typedef enum VoltageBandLabel
{
    VOLTAGEBAND_FIRST = 0,
    VOLTAGEBAND_NOMINAL = 0,
    VOLTAGEBAND_HIGH,
    VOLTAGEBAND_LOW_VOLTAGE,
    VOLTAGEBAND_INVALID,
    VOLTAGEBAND_LAST = VOLTAGEBAND_INVALID
} VoltageBandLabel_t;

// Constants
static const uint8_t RESERVED_BIT_MASK = 0xC0; // 1100 0000

// Lookup Tables
const uint32_t minValues[] =
{
    STATUS_FIRST,
    CURRENTLOAD_FIRST,
    VOLTAGEBAND_FIRST
};

const uint32_t maxValues[] =
{
    STATUS_LAST,
    CURRENTLOAD_LAST,
    VOLTAGEBAND_LAST
};

// #Defines
#define STATUS_SHIFT    0U
#define STATUS_MASK     0x03U

#define LOAD_SHIFT      2U
#define LOAD_MASK       0x03U

#define VOLTAGE_SHIFT   4U
#define VOLTAGE_MASK    0x03U

// Prototypes
void printRailLabel(const RailIndex selected_rail);
void printValidInputs(const Field_t field_selected);
uint32_t inputParsing(Field_t field_selected, const uint32_t min, const uint32_t max);
void buildRegistry(PowerRailRegister *registry);

void extractRailFields(const PowerRailRegister *registry, RailStatus field[]);

StatusLabel_t checkStatus(const RailStatus rails[]);

void printBinary_32bit(const uint32_t registry);

const char *getStatusLabelName(const StatusLabel_t status);
const char *getCurrentLabelName(const CurrentLoadLabel_t current_load);
const char *getVoltageLabelName(const VoltageBandLabel_t voltage_band);
void printReport(const RailStatus rails[], const StatusLabel_t worst_case);

//---------------------------------------------------------

int main(void)
{
    printf("====================\n");
    printf("\nPOWER RAIL MONITOR\n");
    printf("====================\n\n");

    //* Declarations
    PowerRailRegister registry = {0};

    //* Input loop for rail A,B,C,D
    printf("~~~USER INPUT~~~\n\n");

    //* Function 1
    buildRegistry(&registry);

    //* Function 2
    extractRailFields(&registry, registry.rails);

    //* Function 3
    const StatusLabel_t worst_case = checkStatus(registry.rails); // & not needed because the function checks the array directly

    printf("~~~REPORT~~~\n\n");

    //* Function 4
    printBinary_32bit(registry.rail_register);

    //* Function 5
    printReport(registry.rails, worst_case);

    return 0;
}

void printRailLabel(const RailIndex selected_rail)
{
    switch (selected_rail)
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

void printValidInputs(const Field_t field_selected)
{
    switch (field_selected)
    {
    case FIELD_STATUS:
        printf("[STATUS] 0-OK, 1-WARN, 2-FAULT, 3-CRITICAL\n");
        printf("PLEASE INPUT A VALID VALUE: ");
        break;
    case FIELD_CURRENT_LOAD:
        printf("[CURRENT LOAD] 0-IDLE, 1-LOW, 2-MED, 3-HIGH\n");
        printf("PLEASE INPUT A VALID VALUE: ");
        break;
    case FIELD_VOLTAGE_BAND:
        printf("[VOLTAGE BAND] 0-NOMINAL, 1-HIGH, 2-LOW, 3-INVALID\n");
        printf("PLEASE INPUT A VALID VALUE: ");
        break;
    default:
        break;
    }
}

//* Input Parsing Function
uint32_t inputParsing(Field_t field_selected, const uint32_t min, const uint32_t max)
{
    while (true)
    {
        printValidInputs(field_selected);
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
        else
        {
            printf("\n");
            return (uint32_t)check;
        }
    }
}

//* Function 1
void buildRegistry(PowerRailRegister *registry)
{
    for (RailIndex selected_rail = RAIL_FIRST; selected_rail < RAIL_COUNT; selected_rail++)
    {   
        printRailLabel(selected_rail);
        uint32_t rail_buffer = 0U;
        for (Field_t field = FIELD_FIRST; field < FIELD_COUNT; field++)
        {   
            uint32_t field_buffer = inputParsing(field, minValues[field], maxValues[field]);
            switch (field)
            {
            case FIELD_STATUS:
                rail_buffer = rail_buffer | (field_buffer << STATUS_SHIFT);
                break;
            case FIELD_CURRENT_LOAD:
                rail_buffer = rail_buffer | (field_buffer << LOAD_SHIFT);
                break;
            case FIELD_VOLTAGE_BAND:
                rail_buffer = rail_buffer | (field_buffer << VOLTAGE_SHIFT);
                break;
            default:
                break;
            }
        }
        uint32_t clear_mask = ~((uint32_t)0xFFU << (selected_rail * CHAR_BIT)); // Mask to clear any existing set bits to the targeted byte (good practice)
        registry->rail_register = (registry->rail_register & clear_mask | (uint32_t)(rail_buffer << (selected_rail * CHAR_BIT)));
    }
}

// *Function 2
void extractRailFields(const PowerRailRegister *registry, RailStatus field[])
{
    for (RailIndex selected_rail = RAIL_FIRST; selected_rail < RAIL_COUNT; selected_rail++)
    {
        uint8_t rail_registry_buffer = (uint8_t)(registry->rail_register >> (selected_rail * CHAR_BIT)); // Extracting out individual rail fields

        // Checking if reserved bits are 00 first
        if ((rail_registry_buffer & RESERVED_BIT_MASK) != 0U)
        {
            printf("\n\n[ERROR] RESERVED BITS ARE SET. ENDING PROGRAM.\n\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            field[selected_rail].status = ((rail_registry_buffer >> STATUS_SHIFT) & STATUS_MASK);
            field[selected_rail].current_load = ((rail_registry_buffer >> LOAD_SHIFT) & LOAD_MASK);
            field[selected_rail].voltage_band = ((rail_registry_buffer >> VOLTAGE_SHIFT) & VOLTAGE_MASK);
        }
    }
}

// *Function 3
StatusLabel_t checkStatus(const RailStatus rails[])
{
    StatusLabel_t check = STATUS_OK;
    for (RailIndex selected_rail = RAIL_FIRST; selected_rail < RAIL_COUNT; selected_rail++)
    {
        if ((StatusLabel_t)rails[selected_rail].status > check)
        {
            check = (StatusLabel_t)rails[selected_rail].status;
        }
    }
    return check;
}

//* Function 4
void printBinary_32bit(const uint32_t registry)
{
    printf("Rail Register Binary: ");
    const size_t size = sizeof(registry) * CHAR_BIT;
    for (size_t i = 0U; i < size; i++)
    {
        uint32_t mask = (1U << (size - (i + 1U)));
        if ((registry & mask) == mask)
        {
            printf("1");
        }
        else
        {
            printf("0");
        }
        if (((i+1U) % CHAR_BIT) == 0 && (i != 0U)) // Added for spacing between different rails 
        {
            printf(" ");
        }
    }
    printf("\nHexadecimal: %X\n\n", registry);
}

// Helper Functions for Field Printing
const char *getStatusLabelName(const StatusLabel_t status) // A constant pointer to string literals
{
    switch (status)
    {
    case STATUS_OK:
        return "STATUS: OK(00)";
    case STATUS_WARN:
        return "STATUS: WARN(01)";
    case STATUS_FAULT:
        return "STATUS: FAULT(10)";
    case STATUS_CRITICAL:
        return "STATUS: CRITICAL(11)";
    default:
        return "STATUS: UNKNOWN STATUS";
    }
}
const char *getCurrentLabelName(const CurrentLoadLabel_t current_load)
{
    switch (current_load)
    {
    case CURRENTLOAD_IDLE:
        return "LOAD: IDLE(00)";
    case CURRENTLOAD_LOW_CURRENT:
        return "LOAD: LOW(01)";
    case CURRENTLOAD_MED_CURRENT:
        return "LOAD: MED(10)";
    case CURRENTLOAD_HIGH_CURRENT:
        return "LOAD: HIGH(11)";
    default:
        return "LOAD: UNKNOWN STATUS";
    }
}
const char *getVoltageLabelName(const VoltageBandLabel_t voltage_band)
{
    switch (voltage_band)
    {
    case VOLTAGEBAND_NOMINAL:
        return "VBAND: NOMINAL(00)";
    case VOLTAGEBAND_HIGH:
        return "VBAND: HIGH(01)";
    case VOLTAGEBAND_LOW_VOLTAGE:
        return "VBAND: LOW(10)";
    case VOLTAGEBAND_INVALID:
        return "VBAND: INVALID(11)";
    default:
        return "VBAND: UNKNOWN STATUS";
    }
}

//* Function 5
void printReport(const RailStatus rails[], const StatusLabel_t worst_case)
{
    for (RailIndex selected_rail = RAIL_FIRST; selected_rail < RAIL_COUNT; selected_rail++)
    {
        printRailLabel(selected_rail);
        printf("%s\n", getStatusLabelName(rails[selected_rail].status));
        printf("%s\n", getCurrentLabelName(rails[selected_rail].current_load)); 
        printf("%s\n\n",  getVoltageLabelName(rails[selected_rail].voltage_band));
    }
    switch (worst_case)
    {
    case STATUS_OK:
        printf("Worst status: OK");
        break;
    case STATUS_WARN:
        printf("Worst status: WARN");
        break;
    case STATUS_FAULT:
        printf("Worst status: FAULT");
        break;
    case STATUS_CRITICAL:
        printf("Worst status: CRITICAL");
        break;
    default:
        break;
    }
}

// *Review:
// Learned how to use lookup array tables.
// Learned that you can pass an arr[] parameter function to loop through individual indexes INSIDE the function.
// Using enums instead of magic numbers where possible.
// Learned how to handle nested structs with a array of structs.
// Learned to clear bit fields using ~ and & operators.
// General knowledge and skill for bitwise operators usage and embedded coding.
