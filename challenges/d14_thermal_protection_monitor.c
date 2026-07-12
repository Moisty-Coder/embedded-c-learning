/*
Day 14: The Thermal Protection Monitor

Scenario:
In real green power hardware, thermal protection is one of the most safety-critical firmware modules in any BMS or MPPT controller. 
Lithium-ion cells and MOSFET switching circuits generate heat during charge and discharge cycles — if temperature exceeds safe limits, 
the firmware must throttle output power, trigger warnings, and initiate emergency shutdown before physical damage occurs.
You're writing the thermal protection module for a 4-channel battery pack thermal monitor. Each channel corresponds to a temperature sensor (NTC thermistor) 
placed at a different location on the pack — cell group, BMS PCB, charge MOSFET, and discharge MOSFET. 
The ADC reads each sensor and your firmware decides what protective action to take.
------------------------------------------------------------------------------------------------------------------------------------------
Task:
Define a struct called ThermalChannel with these fields:

uint16_t raw_adc — raw 12-bit ADC reading (0–4095)
int16_t temp_dc — temperature in decidegrees Celsius (fixed-point: 253 = 25.3°C)
uint8_t channel_id — channel identifier (1–4)
uint8_t state — thermal state (defined below)

Define the thermal states as a typedef enum:

State               Code         Meaning
THERMAL_OK          0            Normal operation
THERMAL_WARM        1            Throttle charge current
THERMAL_HOT         2            Suspend charging
THERMAL_CRITICAL    3            Emergency shutdown
THERMAL_COLD        4            Suspend discharge

Then write 4 functions:

1.  A function that converts raw ADC to temperature in decidegrees — uses this linear approximation formula:
    temp_dc = ((raw_adc * 1500) / 4095) - 400
    This maps 0–4095 ADC counts to –40.0°C to +110.0°C in decidegrees (–400 to 1100).

2.  A function that assigns thermal state — writes the appropriate state through a pointer based on these thresholds (all in decidegrees):

    Condition                           State
    temp_dc < -200 (below –20°C)        THERMAL_COLD
    temp_dc < 450 (below 45°C)          THERMAL_OK
    temp_dc < 600 (below 60°C)          THERMAL_WARM
    temp_dc < 800 (below 80°C)          THERMAL_HOT
    temp_dc >= 800 (80°C and above)     THERMAL_CRITICAL

3.  A function that scans all 4 channels and determines the system-level action — takes an array of ThermalChannel pointers and a uint8_t *system_state output parameter. 
    Writes the highest severity state found across all channels through the pointer. 
    Priority: CRITICAL > HOT > COLD > WARM > OK. Note that COLD and HOT are independent fault types — if both exist simultaneously across different channels write CRITICAL.

4.  A function that prints the full thermal report — shows each channel's raw ADC, temperature (formatted as e.g. 25.3°C), and state label. 
    Then prints the system-level action with a message:

    System State            Action Message
    THERMAL_OK              "[OK] All channels nominal."
    THERMAL_WARM            "[WARN] Throttling charge current."
    THERMAL_HOT             "[WARN] Suspending charging."
    THERMAL_CRITICAL        "[CRITICAL] Emergency shutdown initiated."
    THERMAL_COLD            "[WARN] Suspending discharge."

5.  In main(), collect 4 raw ADC values, run the full pipeline, and print the report.
------------------------------------------------------------------------------------------------------------------------------------------
Knowledge applied: 
-   Fixed-width integer types (uint16_t, int16_t, uint8_t) from <stdint.h>
-   Fixed-point arithmetic for temperature conversion (no floats)
-   Structs with pointer-based pipeline stages
-   State machine logic — each channel has a thermal state that determines system behavior
-   Fault accumulation across multiple channels (building on your Day 13 faultCheck() lesson)
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum State
{
    THERMAL_OK = 0,
    THERMAL_WARM = 1,
    THERMAL_HOT = 2,
    THERMAL_CRITICAL = 3,
    THERMAL_COLD = 4
} State;

typedef struct ThermalChannel
{
    uint16_t raw_adc;
    int16_t temp_dc;
    uint8_t channel_id;
    State state;
} ThermalChannel;

// Constants Declaration
#define READING_COUNT 4

static const uint8_t MIN_ADC_READING = 0;
static const uint16_t MAX_ADC_READING = 4095U;
static const uint8_t DECI_SCALE = 10U;
static const uint8_t MAX_TEMP = 110U;
static const int8_t MIN_TEMP = -40;

static const int16_t COLD_LIMIT = -200;
static const uint16_t OK_LIMIT = 450;
static const uint16_t WARM_LIMIT = 600;
static const uint16_t HOT_LIMIT = 800;

// Prototype Functions
void getInput(ThermalChannel *readings, const uint8_t size);
void convertADC(ThermalChannel *readings, const uint8_t size);
void thermalState(ThermalChannel *readings, const uint8_t size);
void getSystemState (State *systemState, ThermalChannel *readings, const uint8_t size);
void printStatus(const State state);
void printSystemStatus(const State system_state);
void printReport(const State system_state, const ThermalChannel *readings, const uint8_t size);


int main()
{
    printf("THERMAL PROTECTION MONITORING SYSTEM\n");
    printf("====================================\n");

    ThermalChannel readings[READING_COUNT] = {0};
    uint8_t size = sizeof(readings) / sizeof(readings[0]);

    for (uint8_t i = 0; i < size; i++)
    {
        readings[i].channel_id = i + 1; // Channel ID Initialization
    }

    State system_state = THERMAL_OK; // Default OK

    getInput(readings, size);
    convertADC(readings, size);
    thermalState(readings, size);
    getSystemState(&system_state, readings, size);
    printReport(system_state, readings, size);

    return 0;
}

void getInput(ThermalChannel *readings, const uint8_t size)
{
    printf("~~~DATA COLLECTION~~~\n\n");
    printf("[NOTE] RAW ADC VALUE RANGE (0-4095), NO DECIMALS\n\n");

    for (uint8_t i = 0; i < size; i++)
    {
        while (1)
        {
            printf("READING %u:\n", readings[i].channel_id);
            printf("RAW ADC VALUE: ");

            char buffer[50];
            if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            {
                printf("[ERROR] INPUT MUST BE A VALID NUMBER.\n\n");
                continue;
            }

            char *endptr;
            const long check = strtol(buffer, &endptr, 10);

            if (buffer == endptr)
            {
                printf("[ERROR] INPUT MUST BE A VALID NUMBER.\n\n");
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

            if (check < MIN_ADC_READING || check > MAX_ADC_READING)
            {
                printf("[ERROR] INPUT IS OUT OF RANGE.\n\n");
                continue;
            }

            else
            {
                readings[i].raw_adc = check;
                printf("\n");
                break;
            }
        }
    }
}

void convertADC(ThermalChannel *readings, const uint8_t size)
{
    const uint8_t max_range = abs(MAX_TEMP - MIN_TEMP);
    const int32_t scaled_offset = (int32_t)MIN_TEMP * DECI_SCALE;
    
    for (uint8_t i = 0; i < size; i++)
    {
        const int16_t buffer = (((uint32_t)readings[i].raw_adc * max_range * DECI_SCALE + (MAX_ADC_READING / 2U)) / MAX_ADC_READING) + scaled_offset;
        readings[i].temp_dc = buffer;
    }
}

void thermalState(ThermalChannel *readings, const uint8_t size)
{
    for (uint8_t i = 0; i < size; i++)
    {
        if (readings[i].temp_dc < COLD_LIMIT)
        {
            readings[i].state = THERMAL_COLD;
        }
        else if (readings[i].temp_dc < OK_LIMIT)
        {
            readings[i].state = THERMAL_OK;
        }
        else if (readings[i].temp_dc < WARM_LIMIT)
        {
            readings[i].state = THERMAL_WARM;
        }
        else if (readings[i].temp_dc < HOT_LIMIT)
        {
            readings[i].state = THERMAL_HOT;
        }
        else
        {
            readings[i].state = THERMAL_CRITICAL;
        }
    }
}

void getSystemState (State *system_state, ThermalChannel *readings, const uint8_t size)
{
    uint8_t cold_check = 0;
    uint8_t hot_check = 0;
    uint8_t warm_check = 0;

    for (uint8_t i = 0; i < size; i++)
    {
        switch (readings[i].state)
        {
        case THERMAL_CRITICAL:
            *system_state = THERMAL_CRITICAL;
            return;
        case THERMAL_COLD:
            cold_check = 1;
            break;
        case THERMAL_HOT:
            hot_check = 1;
            break;
        case THERMAL_WARM:
            warm_check = 1;
            break;
        default:
            break;
        }
    }

    if (cold_check == 1 && hot_check == 1)
    {
       *system_state = THERMAL_CRITICAL;
    }
    else if (cold_check == 1)
    {
        *system_state = THERMAL_COLD;
    }
    else if (hot_check == 1)
    {
        *system_state = THERMAL_HOT;
    }
    else if (warm_check == 1)
    {
        *system_state = THERMAL_WARM;
    }
    else
    {
        *system_state = THERMAL_OK;
    }
}

void printStatus(const State state)
{
    switch (state)
    {
    case THERMAL_OK:
        printf("[OK]\n\n");
        break;
    case THERMAL_WARM:
        printf("[WARM]\n\n");
        break;
    case THERMAL_HOT:
        printf("[HOT]\n\n");
        break;
    case THERMAL_CRITICAL:
        printf("[CRITICAL]\n\n");
        break;
    case THERMAL_COLD:
        printf("[COLD]\n\n");
        break;      
    default:
        break;
    }
}

void printSystemStatus(const State system_state)
{
    switch (system_state)
    {
    case THERMAL_OK:
        printf("[OK] All channels nominal.\n\n");
        break;
    case THERMAL_WARM:
        printf("[WARN] Throttling charge current.\n\n");
        break;
    case THERMAL_HOT:
        printf("[WARN] Suspending charging.\n\n");
        break;
    case THERMAL_CRITICAL:
        printf("[CRITICAL] Emergency shutdown initiated.\n\n");
        break;
    case THERMAL_COLD:
        printf("[WARN] Suspending discharge.\n\n");
        break;      
    default:
        break;
    }
}

void printReport(const State system_state, const ThermalChannel *readings, const uint8_t size)
{
    printf("~~~REPORT~~~\n");
    printf("============\n");

    for (uint8_t i = 0; i < size; i++)
    {
        const int16_t total = readings[i].temp_dc;
        printf("Reading %u:\n", readings[i].channel_id);
        printf("Raw ADC: %u\n", readings[i].raw_adc);
        printf("Temperature: %c%d.%01u*C\n", (total < 0 && total > -10) ? ('-') : ('\0'), total / DECI_SCALE, (unsigned int)abs(total % DECI_SCALE));
        printf("Channel State: ");
        printStatus(readings[i].state);
    }
    printf("System State: ");
    printSystemStatus(system_state);
}





