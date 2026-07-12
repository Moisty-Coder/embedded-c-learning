/*
Day 12: The Battery Cell Voltage Monitor

Scenario:
You're writing firmware for a Battery Management System (BMS) protecting a 4-cell lithium-ion pack. 
Each cell's voltage is read by an ADC (Analog-to-Digital Converter) and stored as a raw 12-bit integer value (0–4095), representing a voltage range of 0.0V to 5.0V.
Your firmware must convert each raw ADC reading to an actual voltage, assess each cell's health status, and flag pack-level warnings. 
This is the core monitoring loop of any real BMS.
------------------------------------------------------------------------------------------------------------------------------------------
Task:
Use <stdint.h> and define your data using fixed-width types.
Define a struct called CellData with these fields:

-   uint16_t raw_adc — the raw 12-bit ADC reading (0–4095)
-   float voltage — the converted voltage value
-   uint8_t status — 0 = OK, 1 = LOW, 2 = CRITICAL, 3 = OVERVOLTAGE

Then write 4 functions:

1.  A function that converts raw ADC to voltage — uses the formula:
    voltage = (raw_adc / 4095.0f) * 5.0f. Takes a pointer to a CellData struct and writes the result through it.

2.  A function that assesses cell status — writes the appropriate status code through a pointer based on these thresholds:

Voltage             Status Code
Below 3.0V          2 — CRITICAL
3.0V to 3.2V        1 — LOW
3.2V to 4.2V        0 — OK
Above 4.2V          3 — OVERVOLTAGE

3.  A function that scans the full pack — takes an array of 4 CellData structs and returns the index of the most 
    critical cell (OVERVOLTAGE takes priority over CRITICAL, CRITICAL over LOW, LOW over OK). If all cells are OK, return -1.
4.  A function that prints the full pack report — shows each cell's raw ADC, voltage, and status label, 
    then prints which cell is most critical or confirms the pack is healthy.

In main(), collect 4 raw ADC values from the user (validate: 0–4095), run the full pipeline, and print the report.
------------------------------------------------------------------------------------------------------------------------------------------
New Knowledge applied: 
-   #include <stdint.h> and fixed-width types (uint8_t, uint16_t)
-   ADC-to-voltage conversion formula (real BMS math)
-   Priority-based fault detection across a cell array
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct CellData
{
    uint16_t raw_adc;
    int16_t voltage; // Fixed-point, scaled by SCALE (100). e.g. 325 = 3.25V
    uint8_t status;

} CellData;

typedef enum Status
{
    STATUS_OK,
    STATUS_LOW,
    STATUS_CRITICAL,
    STATUS_OVERVOLTAGE
} Status;

#define SCALE 100
#define CRITICAL_LIMIT 300
#define LOW_LIMIT      320
#define NORMAL_LIMIT   420

// Prototype Functions
void getInput(CellData *reading, size_t idx);
void convertADC(CellData *reading);
void checkVoltageStatus(CellData *reading);
int8_t findCriticalCell(CellData **array, size_t size);
void printStatus(uint8_t status);
void printReport(CellData **array, size_t size, int8_t critical_cell_check);

int main()
{
    // Declared variables
    CellData reading_1 = {0};
    CellData reading_2 = {0};
    CellData reading_3 = {0};
    CellData reading_4 = {0};

    CellData *readings[] = {&reading_1, &reading_2, &reading_3, &reading_4};
    size_t size = sizeof(readings) / sizeof(readings[0]);

    
    printf("BATTERY CELL VOLTAGE MONITOR\n");
    printf("============================\n\n");

    printf("~~~RAW ADC VALUE INPUT~~~\n");
    printf("[NOTE]: Valid range = (0-4095)\n\n");

    for (size_t i = 0; i < size; i++)
    {
        getInput(readings[i], i + 1);
    }

    for (size_t i = 0; i < size; i++)
    {
        convertADC(readings[i]);
    }

    for (size_t i = 0; i < size; i++)
    {
        checkVoltageStatus(readings[i]);
    }

    int8_t critical_cell_check = findCriticalCell(readings, size);

    printReport(readings, size, critical_cell_check);

    return 0;
}

void getInput(CellData *reading, size_t idx)
{
    while(1)
    {
        printf("Reading %zu:\n", idx);
        printf("Raw ADC value: ");

        char buffer[50];
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("[ERROR] Input must be a valid number!\n\n");
            continue;
        }

        char *endptr;
        long check = strtol(buffer, &endptr, 10);

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

        if (check < 0 || check > 4095)
        {
            printf("[ERROR] Input must be within the specified range.\n\n");
            continue;
        }
        else
        {
            reading->raw_adc = check;
            break;
        }
    }
}

void convertADC(CellData *reading)
{
    uint16_t divisor = 4095; 

    // Calculate the scaled voltage first (keeps 2 decimal places)
    // We multiply by 5 and SCALE (100) before dividing to preserve accuracy
    uint16_t scaled_voltage = ((uint32_t)reading->raw_adc * 5 * SCALE + divisor / 2) / divisor;

    //uint8_t voltage_whole = (uint8_t)(scaled_voltage / SCALE); - Gets the non-decimal part of the value (325 / 100 = 3)
    //uint8_t voltage_decimal = (uint8_t)(scaled_voltage % SCALE); - Gets the remainder (325 % 100 = 25)

    reading->voltage = scaled_voltage;
}

void checkVoltageStatus(CellData *reading)
{

    if (reading->voltage < CRITICAL_LIMIT) 
    {
        reading->status = STATUS_CRITICAL; // Critical
    }
    else if (reading->voltage <= LOW_LIMIT)
    {
        reading->status = STATUS_LOW; // Low
    }
    else if (reading->voltage <= NORMAL_LIMIT)
    {
        reading->status = STATUS_OK; // Normal
    }
    else
    {
        reading->status = STATUS_OVERVOLTAGE; // Overvoltage
    }
}

int8_t findCriticalCell(CellData **array, size_t size)
{
    for (size_t i = 0; i < size; i++) // Checking for the first Overvoltage 
    {
        if (array[i]->status == STATUS_OVERVOLTAGE)
        {
            return i + 1;
        }
    }

    for (size_t i = 0; i < size; i++)
    {
        if (array[i]->status == STATUS_CRITICAL)
        {
            return i + 1;
        }
    }

    for (size_t i = 0; i < size; i++)
    {
        if (array[i]->status == STATUS_LOW)
        {
            return i + 1;
        }
    }

    return -1;
}

void printStatus(uint8_t status)
{
    switch (status)
    {
    case STATUS_OK:
        printf("Status: OK\n\n");
        break;
    case STATUS_LOW:
        printf("Status: LOW\n\n");
        break;
    case STATUS_CRITICAL:
        printf("Status: CRITICAL\n\n");
        break;
    case STATUS_OVERVOLTAGE:
        printf("Status: OVERVOLTAGE\n\n");
        break;
    default:
        break;
    }
}

void printReport(CellData **array, size_t size, int8_t critical_cell_check)
{
    printf("\n\n~~~REPORT~~~\n\n");
    
    for (size_t i = 0; i < size; i++)
    {
        printf("Reading %zu:\n", i + 1);
        printf("Raw ADC: %d\n", array[i]->raw_adc);
        printf("Cell Voltage: %d.%02dV\n", array[i]->voltage / SCALE, array[i]->voltage % SCALE);
        printStatus(array[i]->status);
    }

    if (critical_cell_check != -1)
    {
        printf("Most Critical: Cell %d", critical_cell_check);
    }
    else
    {
        printf("Most critical: [OK] Pack is healthy.");
    }
}

/*
Width       Signed Type     Unsigned Type   Value Range (Signed)                                        Value Range (Unsigned)
8-bit       int8_t          uint8_t         -128 to 127                                                 0 to 255    
16-bit      int16_t         uint16_t        -32,768 to 32,767                                           0 to 65,535
32-bit      int32_t         uint32_t        -2,147,483,648 to 2,147,483,647                             0 to 4,294,967,295
64-bit      int64_t         uint64_t        -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807     0 to 18,446,744,073,709,551,615
*/