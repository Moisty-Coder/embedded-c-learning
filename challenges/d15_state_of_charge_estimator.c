/*
Day 15: State of Charge Estimator

Scenario:
In any Battery Management System, the State of Charge (SoC) estimator is arguably the most critical algorithm — 
it tells the system (and the user) how much energy remains in the battery pack. An inaccurate SoC can lead to overdischarge, 
which permanently damages lithium-ion cells, or premature cutoff, which wastes usable capacity.
You're writing the SoC estimation module for a 48V lithium-ion pack used in a residential solar storage system. 
Your firmware uses the simplest practical method: Coulomb Counting — tracking how much charge flows in and out of the pack over time, and updating the SoC accordingly.
------------------------------------------------------------------------------------------------------------------------------------------
Task:
Define a struct called BatteryPack with these fields:

uint32_t capacity_mas — total pack capacity in milliamp-seconds (mAs)
uint32_t charge_mas — current charge remaining in mAs
uint8_t soc_percent — state of charge as a whole percentage (0–100)
uint8_t state — current pack state (defined below)

Define pack states as a typedef enum:

State               Code         Meaning
PACK_NORMAL         0            Normal operation
PACK_LOW            1            Low charge warning
PACK_CRITICAL       2            Near-empty, suspend discharge
PACK_FULL           3            Fully charged
PACK_FAULT          4            SoC out of valid bounds

Then write 4 functions:

Then write 4 functions:

1.  A function that initializes the pack — takes a BatteryPack pointer, a capacity in mAs, and an initial SoC percentage (0–100). 
    Computes the initial charge_mas from the percentage and capacity. Validates that the initial SoC is within 0–100, sets PACK_FAULT if not.

2.  A function that applies a charge or discharge event — takes a BatteryPack pointer and an 
    int32_t delta_mas (positive = charging, negative = discharging). Adds the delta to charge_mas, then clamps charge_mas to the range [0, capacity_mas] — 
    never allow it to go below zero or above capacity. Recalculates soc_percent from the updated charge_mas.

3.  A function that updates pack state — takes a BatteryPack pointer and writes the correct state based on these thresholds:

    Condition                                           State
    soc_percent > 100 or charge_mas > capacity_mas      PACK_FAULT
    soc_percent == 100                                  PACK_FULL
    soc_percent > 20                                    PACK_NORMAL
    soc_percent > 10                                    PACK_LOW
    soc_percent <= 10                                   PACK_CRITICAL

4.  A function that prints the pack status report — shows capacity, current charge, SoC percentage, and state label with an action message:

    State               Action Message
    PACK_NORMAL         "[OK] Pack operating normally."
    PACK_LOW            "[WARN] Low charge. Consider recharging."
    PACK_CRITICAL       "[CRITICAL] Suspending discharge."
    PACK_FULL           "[OK] Pack fully charged."
    PACK_FAULT          "[FAULT] SoC out of valid bounds. Check sensors."

5.  In main(), initialize a pack, then apply a sequence of 5 charge/discharge events collected from the user (positive or negative integers in mAs), 
    updating and printing the state after each event.
------------------------------------------------------------------------------------------------------------------------------------------
Knowledge applied: 
-   Fixed-width integer types (int32_t, uint32_t, uint8_t) from <stdint.h>
-   Fixed-point arithmetic for charge accumulation
-   Structs with multiple related fields passed by pointer
-   State machine logic — SoC drives system behavior decisions
-   Fault accumulation pattern from Day 13/14 applied to SoC boundary conditions
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>

typedef enum State
{
    PACK_NORMAL = 0,
    PACK_LOW = 1,
    PACK_CRITICAL = 2,
    PACK_FULL = 3,
    PACK_FAULT = 4
} State;

typedef struct BatteryPack
{
    uint32_t capacity_milliAmpsec; // — total pack capacity in milliamp-seconds (mAs)
    uint32_t charge_milliAmpsec; // — current charge remaining in mAs
    uint8_t soc_percent; // — state of charge as a whole percentage (0–100)
    State state; // — current pack state (defined below)
} BatteryPack;

// Constants
static const uint8_t EVENT_COUNT = 5U;
static const uint8_t SOC_FULL_LIMIT = 100U;
static const uint8_t SOC_NORMAL_LIMIT = 20U;
static const uint8_t SOC_LOW_LIMIT = 10U;

// Prototype Functions
void initializePack(BatteryPack *battery);
void updateCharge(BatteryPack *battery, int32_t *delta_milliAmpsec, uint8_t i);
void updateState(BatteryPack *battery);
uint32_t clamp(int32_t value, int32_t max_clamp, int32_t min_clamp);
void printStatus(const State state);
void printEvent(const BatteryPack *battery);


int main(void)
{
    printf("\nSTATE OF CHARGE ESTIMATOR\n");
    printf("=========================\n\n");

    BatteryPack battery = {0};

    initializePack(&battery);
    printEvent(&battery);

    uint8_t i = 0U;
    while (i < EVENT_COUNT)
    {
        int32_t delta_mAs = 0;
        updateCharge(&battery, &delta_mAs, i + 1);
        updateState(&battery);
        printEvent(&battery);
        i++;
    }
    
    return 0;
}

void initializePack(BatteryPack *battery)
{
    // Most robust version of input checking
    while (true)
    {
        printf("BATTERY CAPACITY (mAs): ");
        char buffer[50];
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("[ERROR] INPUT MUST BE A VALID VALUE.\n\n");
            continue;
        }

        if (strchr(buffer, '\n') == NULL && feof(stdin) == false) // Finding if \n exists in the input read, feof returns false if theres still data to read
        {
            printf("[ERROR] INPUT IS TOO LONG.\n\n");
            // Flushing input buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF); // EOF means no more data can be read from the stream
            continue;
        }

        char *endptr;
        errno = 0; // Added error number parsing

        const intmax_t check = strtoimax(buffer, &endptr, 10);

        // Checking for value overflow
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
        
        if (check <= 0)
        {
            printf("[ERROR] VALUE CANNOT BE LESS THAN OR EQUAL TO 0.\n\n");
            continue;
        }

        if (check > UINT32_MAX)
        {
            printf("[ERROR] VALUE EXCEEDS MAXIMUM 32-BIT CAPACITY (%u).\n\n", UINT32_MAX);
            continue;
        }
        else
        {
            battery->capacity_milliAmpsec = (uint32_t)check;
            break;
        }
    }

    while (true)
    {
        printf("CURRENT BATTERY CHARGE (%%): ");
        char buffer[50];
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("[ERROR] INPUT MUST BE A VALID VALUE.\n\n");
            continue;
        }

        char *endptr;
        const long check = strtol(buffer, &endptr, 10);

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
        if (check < 0 || check > 100)
        {
            battery->state = PACK_FAULT;
            battery->soc_percent = (uint8_t)check;
            break;
        }
        else
        {
            battery->soc_percent = (uint8_t)check;
            break;
        }
    }

    battery->charge_milliAmpsec = ((uint32_t)battery->capacity_milliAmpsec * battery->soc_percent) / 100U;
}

uint32_t clamp(int32_t value, int32_t max_clamp, int32_t min_clamp)
{
    if (value > max_clamp)
    {
        return (uint32_t)max_clamp;
    }
    if (value < min_clamp)
    {
        return (uint32_t)min_clamp;
    }
    return (uint32_t)value;
}

void updateCharge(BatteryPack *battery, int32_t *delta_milliAmpsec, uint8_t event )
{
    printf("EVENT %u:\n", event);

    while (true)
    {
        printf("INPUT CHANGE OF CHARGE (mAs): ");

        char buffer[50];
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("[ERROR] INPUT MUST BE A VALID VALUE.\n\n");
            continue;
        }

        char *endptr;
        const long check = strtol(buffer, &endptr, 10);

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
        
        else
        {
            *delta_milliAmpsec = check;
            break;
        }
    }

    // Clamps
    int32_t const max_clamp = (int32_t)battery->capacity_milliAmpsec;
    int32_t const min_clamp = 0;

    int32_t const buffer_charge = (int32_t)battery->charge_milliAmpsec + *delta_milliAmpsec;


    battery->charge_milliAmpsec = clamp(buffer_charge, max_clamp, min_clamp);
    battery->soc_percent = (uint8_t)(((uint32_t)battery->charge_milliAmpsec * 100U) / battery->capacity_milliAmpsec);
    
}

void updateState(BatteryPack *battery)
{
    uint8_t check_sOc = battery->soc_percent;

    if (check_sOc > SOC_FULL_LIMIT || battery->charge_milliAmpsec > battery->capacity_milliAmpsec)
    {
        battery->state = PACK_FAULT;
    }
    else if (check_sOc == SOC_FULL_LIMIT)
    {
        battery->state = PACK_FULL;
    }
    else if (check_sOc > SOC_NORMAL_LIMIT)
    {
        battery->state = PACK_NORMAL;
    }
    else if (check_sOc > SOC_LOW_LIMIT)
    {
        battery->state = PACK_LOW;
    }
    else if (check_sOc <= SOC_LOW_LIMIT)
    {
        battery->state = PACK_CRITICAL;
    }
}

void printStatus(const State state)
{
    switch (state)
    {
    case PACK_NORMAL:
        printf("[OK] Pack operating normally.\n\n");
        break;
    case PACK_LOW:
        printf("[WARN] Low charge. Consider recharging.\n\n");
        break;
    case PACK_CRITICAL:
        printf("[CRITICAL] Suspending discharge.\n\n");
        break;
    case PACK_FULL:
        printf("[OK] Pack fully charged.\n\n");
        break;
    case PACK_FAULT:
        printf("[FAULT] SoC out of valid bounds. Check sensors.\n\n");
        break;
    
    default:
        break;
    }
}

void printEvent(const BatteryPack *battery)
{
    printf("Battery Capacity: %umAs\n", battery->capacity_milliAmpsec);
    printf("Battery SoC: %u%%\n", battery->soc_percent);
    printf("Battery Charge: %umAs\n", battery->charge_milliAmpsec);
    printf("Status: ");
    printStatus(battery->state);
}