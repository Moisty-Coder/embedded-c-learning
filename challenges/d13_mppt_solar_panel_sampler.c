/*
Day 13: The MPPT Solar Panel Sampler

Scenario:
You're writing firmware for a Maximum Power Point Tracker (MPPT) — the core component of any solar charge controller. 
Before the MPPT algorithm can find the optimal operating point, it needs to sample and validate raw ADC readings from 
two sensors simultaneously: a voltage divider measuring panel voltage, and a shunt resistor measuring panel current.
Your firmware reads 5 consecutive ADC samples for both voltage and current, converts them to physical units using fixed-point arithmetic, 
computes average power, and flags any anomalies — all without using a single float.
------------------------------------------------------------------------------------------------------------------------------------------
Task:
Define a struct called MPPTSample with these fields:

uint16_t raw_voltage[5]  —     raw ADC readings for panel voltage (0–4095)
uint16_t raw_current[5]  —     raw ADC readings for panel current (0–4095)
uint16_t voltage_mv[5]   —     converted voltage in millivolts
uint16_t current_ma[5]   —     converted current in milliamps
uint32_t power_mw[5]     —     computed power in milliwatts per sample
uint32_t avg_power_mw    —     average power across all 5 samples
uint8_t fault            —     0 = OK, 1 = OVERVOLTAGE, 2 = OVERCURRENT, 3 = BOTH

Then write 4 functions:

1.  A function that collects raw ADC samples — fills both raw_voltage[5] and raw_current[5] arrays through a pointer. Valid range: 0–4095 for both. Reject invalid inputs.

2.  A function that converts raw ADC values to physical units — uses these formulas:

-   voltage_mv = (raw_voltage * 48000) / 4095 — maps 0–4095 to 0–48V (in mV)
-   current_ma = (raw_current * 20000) / 4095 — maps 0–4095 to 0–20A (in mA)

Writes results through the struct pointer.

3.  A function that computes power and average — for each sample: power_mw[i] = (voltage_mv[i] * current_ma[i]) / 1000. 
    Then computes avg_power_mw as the average across all 5 samples. Be careful about overflow — think about what integer type you need for intermediate values.

4.  A function that checks for faults — sets the fault field through the pointer:

-   OVERVOLTAGE if any voltage_mv exceeds 40,000 mV (40V)
-   OVERCURRENT if any current_ma exceeds 15,000 mA (15A)
-   BOTH if both conditions are present
-   OK otherwise

In main(), collect samples, run the full pipeline, and print a report showing all 5 converted voltages and currents, per-sample power, average power, and fault status.
------------------------------------------------------------------------------------------------------------------------------------------
Knowledge applied: 
-   Fixed-width integer types (uint16_t, uint32_t) from <stdint.h>
-   Fixed-point arithmetic (building on your Day 12 self-imposed constraint)
-   Passing structs by pointer across a multi-stage pipeline
-   Array averaging and power calculation in integer arithmetic
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

#define SAMPLE_COUNT 5

// Constants
static const uint16_t MIN_ADC_READING = 0U;
static const uint16_t MAX_ADC_READING = 4095U;
static const uint16_t ADC_CONVERSION_DIVISOR = 4095U;
static const uint16_t MAX_VOLTAGE = 48U;
static const uint16_t MAX_CURRENT = 20U;
static const uint16_t VOLTAGE_FLAG_LIMIT = 40000U; // milli
static const uint16_t CURRENT_FLAG_LIMIT = 15000U;
static const uint16_t MILLI_SCALE = 1000U;

typedef enum Status
{
    STATUS_OK,
    STATUS_OVERVOLTAGE,
    STATUS_OVERCURRENT,
    STATUS_BOTH
} Status;

typedef struct MPPTSample
{
    uint16_t raw_voltage[SAMPLE_COUNT];    // Raw ADC readings for panel voltage (0–4095)
    uint16_t raw_current[SAMPLE_COUNT];    // Raw ADC readings for panel current (0–4095)
    uint32_t voltage_mv[SAMPLE_COUNT];     // Converted voltage in millivolts
    uint32_t current_ma[SAMPLE_COUNT];     // Converted current in milliamps
    uint32_t power_mw[SAMPLE_COUNT];       // Computed power in milliwatts per sample
    uint32_t avg_power_mw;      // Average power across all 5 samples
    Status fault;              // 0 = OK, 1 = OVERVOLTAGE, 2 = OVERCURRENT, 3 = BOTH  
} MPPTSample;

// Prototype Functions
void getInput(MPPTSample *reading);
void convertADC(MPPTSample *reading);
void computePower(MPPTSample *ptr_reading);
void faultCheck(MPPTSample *ptr_reading);
void printFault(const Status fault);
void printReport(const MPPTSample *ptr_reading);


int main()
{
    printf("MPPT SOLAR PANEL SAMPLER\n");
    printf("========================\n");

    MPPTSample reading = {0}; // Initialize
    MPPTSample *ptr_reading = &reading;

    getInput(ptr_reading);
    convertADC(ptr_reading);
    computePower(ptr_reading);
    faultCheck(ptr_reading);
    printReport(ptr_reading);

    return 0;
}

void getInput(MPPTSample *ptr_reading)
{

    printf("~~~RAW ADC VALUE INPUT~~~\n");
    printf("[NOTE]: Valid range = (0-4095)\n\n");

    for (size_t i = 0; i < SAMPLE_COUNT; i++)
    {
        printf("SAMPLE %d\n", i + 1);

        while (1) // Voltage Reading
        {
            printf("Raw ADC Voltage (V): ");

            char buffer[50];
            if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            {
                printf("[ERROR] Input must be a valid number!\n\n");
                continue;
            }

            char *endptr;
            const long check = strtol(buffer, &endptr, 10);

            if (buffer == endptr)
            {
                printf("[ERROR] Input must be a valid number!\n\n");
                continue;
            }

            while (*endptr != '\0' && isspace((unsigned char)*endptr))  
            {
                endptr++;
            }

            if (*endptr != '\0') 
            {
                printf("[ERROR] Trailing characters detected. Please input numbers only!\n\n");
                continue;
            }

            if (check < MIN_ADC_READING || check > MAX_ADC_READING)
            {
                printf("[ERROR] Input must be within the specified range.\n\n");
                continue;
            }
            else
            {
                ptr_reading->raw_voltage[i] = check;
                break;
            }
        }

        while (1) // Current Reading
        {
            printf("Raw ADC Current (A): ");

            char buffer[50];
            if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            {
                printf("[ERROR] Input must be a valid number!\n\n");
                continue;
            }

            char *endptr;
            const long check = strtol(buffer, &endptr, 10);

            if (buffer == endptr)
            {
                printf("[ERROR] Input must be a valid number!\n\n");
                continue;
            }

            while (*endptr != '\0' && isspace((unsigned char)*endptr))  
            {
                endptr++;
            }

            if (*endptr != '\0') 
            {
                printf("[ERROR] Trailing characters detected. Please input numbers only!\n\n");
                continue;
            }

            if (check < MIN_ADC_READING || check > MAX_ADC_READING)
            {
                printf("[ERROR] Input must be within the specified range.\n\n");
                continue;
            }
            else
            {
                ptr_reading->raw_current[i] = check;
                printf("\n");
                break;
            }
        }
    }    
}

void convertADC(MPPTSample *ptr_reading)
{
    for (size_t i = 0; i < SAMPLE_COUNT; i++)
    {
        const uint32_t buffer = ((uint64_t)ptr_reading->raw_voltage[i] * MAX_VOLTAGE * MILLI_SCALE + (ADC_CONVERSION_DIVISOR / 2U)) / ADC_CONVERSION_DIVISOR;
        ptr_reading->voltage_mv[i] = buffer;
    }

    for (size_t i = 0; i < SAMPLE_COUNT; i++)
    {
        const uint32_t buffer = ((uint64_t)ptr_reading->raw_current[i] * MAX_CURRENT * MILLI_SCALE + (ADC_CONVERSION_DIVISOR / 2U)) / ADC_CONVERSION_DIVISOR;
        ptr_reading->current_ma[i] = buffer;
    }
}

void computePower(MPPTSample *ptr_reading)
{
    for (size_t i = 0; i < SAMPLE_COUNT; i++)
    {
        const uint32_t buffer_power = ((uint64_t)ptr_reading->voltage_mv[i] * ptr_reading->current_ma[i]) / MILLI_SCALE; // (10^-3 * 10^-3 = 10^-6 - micro) / micro -> milli = 10^-6 = 10^-3(milli) * 10^-3(milli) = 10^-3 / 1000
        ptr_reading->power_mw[i] = buffer_power;
    }
    
    uint32_t sum_power = 0;
    for (size_t i = 0; i < SAMPLE_COUNT; i++)
    {
        sum_power = sum_power + ptr_reading->power_mw[i];
    }
    
    const uint32_t buffer_power_average = sum_power / SAMPLE_COUNT;
    ptr_reading->avg_power_mw = buffer_power_average;
}

void faultCheck(MPPTSample *ptr_reading)
{
    uint8_t ov_detected = 0;
    uint8_t oc_detected = 0;

    for (size_t i = 0; i < SAMPLE_COUNT; i++)
    {
        if (ptr_reading->voltage_mv[i] > VOLTAGE_FLAG_LIMIT)
        {
            ov_detected = 1;
        }
        if (ptr_reading->current_ma[i] > CURRENT_FLAG_LIMIT) 
        {
            oc_detected = 1;  
        }
    }
    if (ov_detected && oc_detected)
    {
        ptr_reading->fault = STATUS_BOTH;
    }
    else if (ov_detected)
    {
        ptr_reading->fault = STATUS_OVERVOLTAGE;
    }
    else if (oc_detected)
    {
        ptr_reading->fault = STATUS_OVERCURRENT;
    }
    else
    {
        ptr_reading->fault = STATUS_OK;
    }
}

void printFault(const Status fault)
{
    switch (fault)
    {
    case STATUS_OK:
        printf("OK");
        break;
    case STATUS_OVERVOLTAGE:
        printf("OVERVOLTAGE");
        break;
    case STATUS_OVERCURRENT:
        printf("OVERCURRENT");
        break;
    case STATUS_BOTH:
        printf("BOTH");
        break;
    default:
        break;
    }
}

void printReport(const MPPTSample *ptr_reading)
{
    printf("\n\n~~~REPORT~~~\n\n");
    for (size_t i = 0; i < SAMPLE_COUNT; i++)
    {
        printf("Sample %d:\n", i + 1);
        printf("Raw Voltage: %u\n", ptr_reading->raw_voltage[i]);
        printf("Raw Current: %u\n", ptr_reading->raw_current[i]);
        printf("Converted Voltage (mV): %lumV\n", ptr_reading->voltage_mv[i]);
        printf("Converted Current (mA): %lumA\n", ptr_reading->current_ma[i]);
        printf("Power (mW): %lumW\n\n", ptr_reading->power_mw[i]);
    }
    printf("Average Power (mW): %lu\n", ptr_reading->avg_power_mw);
    printf("Fault: " );
    printFault(ptr_reading->fault);
}

/*
REVIEW:
-   Use static const for unmutable data that will be reused and only exclusive to the file.
-   const cannot be used in array[], use #define
*/

