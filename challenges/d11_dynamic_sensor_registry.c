/*
Day 11: The Dynamic Sensor Registry 

Scenario:
You're building a sensor registry for a configurable embedded device. Unlike previous challenges where the number of sensors was fixed at compile time, 
this device supports a variable number of sensors determined at startup — like a plug-and-play industrial controller where 
operators add or remove sensor modules at boot time.
------------------------------------------------------------------------------------------------------------------------------------------
Task:
Define a struct called SensorEntry with these fields: id (int), name (char array of 32), value (float), and active (int).

Then write 5 functions:

1.  A function that creates the registry — takes an int count and returns a SensorEntry * pointing to a dynamically allocated array of count entries. 
    Initialize all fields to zero/empty. Returns NULL if allocation fails.
2.  A function that populates a single entry — takes a SensorEntry * to one entry, an id, a name string, and a value. 
    Writes all fields through the pointer and sets active = 1.
3.  A function that deactivates a sensor by ID — takes the registry pointer, the count, and a target id. Finds the matching entry and sets active = 0. 
    If no match is found, prints a warning.
4.  A function that computes the average value of all active sensors — takes the registry pointer and count, writes the result through a float * output parameter. 
    If no active sensors exist, writes 0.0 and prints a warning.
4.  A function that prints the full registry — shows all entries with their id, name, value, and active status, followed by the active-only average.

In main(), ask the user how many sensors to register (1–10), allocate the registry, populate it, deactivate one sensor by ID, compute the average, print the report, 
then free the memory.
------------------------------------------------------------------------------------------------------------------------------------------
Constraints

-   Registry array must be dynamically allocated — no fixed-size arrays for the registry itself
-   Check malloc/calloc return value for NULL
-   Free all allocated memory before the program exits
-   No global variables
-   Apply all previous good habits: size_t for sizes, float division, void where appropriate

Knowledge applied: Dynamic memory allocation, structs, pointers
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

typedef struct SensorEntry
{
    int id;
    char name[32];
    float value;
    int active;
} SensorEntry;

// Prototype Functions
SensorEntry* allocation(int sensorCount);
void entry(SensorEntry *sensor, int sensorCount);
int deactivateSensor(SensorEntry *sensor, int sensorCount, SensorEntry **deactivatedSensor);
float *averageCalculator(SensorEntry *sensor, float *average, int sensorCount);
void printReport(SensorEntry *sensor, SensorEntry **deactivatedSensor, int deactivate_target_ID, int sensorCount, float *ptr_average);

int main()
{
    printf("\nDYNAMIC SENSOR REGISTRY\n");
    printf("==============================\n\n");
    
    int sensorCount = 0;
    while(1)
    {
        printf("How many sensor/s to register? (1-10): ");

        char buffer[100];
        if (fgets(buffer, sizeof(buffer), stdin) == NULL )
        {
            printf("[ERROR] Please input a valid number!\n\n");
            continue;
        }

        char *endptr;
        long check = strtol(buffer, &endptr, 10);

        if (buffer == endptr) // Buffer array decays into pointer
        {
            printf("[ERROR] Please input a valid number!\n\n");
            continue;
        }

        while (*endptr != '\0' && isspace((unsigned char)*endptr))
        {
            endptr++;
        }
        if (*endptr != '\0')
        {
            printf("[ERROR] Trailing characters detected, please input valid values only!\n\n");
            continue;
        }
        if (check < 1 || check > 10)
        {
            printf("[ERROR] Please input a valid range!\n\n");
            continue;
        }
        else
        {
            sensorCount = check;
            break;
        }
    }

    SensorEntry *sensor = allocation(sensorCount);

    if (sensor == NULL)
    {
        exit(EXIT_FAILURE);
    } // Exit program safely
    
    printf("-------------------\n");
    printf("\nSENSOR ENTRIES\n");
    for (int i = 0; i < sensorCount; i++)
    {
        entry(&sensor[i], i + 1); // basically ptr_sensor, but using & to avoid declaring unnecessary pointer variables
    }

    SensorEntry *ptr_deactivated_sensor = NULL;
    SensorEntry **pptr_deactivated_sensor = &ptr_deactivated_sensor;
    int deactivate_target_ID = deactivateSensor(sensor, sensorCount, pptr_deactivated_sensor);

    float average;
    float *ptr_average = &average;

    ptr_average = averageCalculator(sensor, ptr_average, sensorCount);

    printReport(sensor, pptr_deactivated_sensor, deactivate_target_ID, sensorCount, ptr_average);

    // Freeing sensor memory
    free(sensor);
    sensor = NULL;

    return 0;
}

SensorEntry *allocation(int sensorCount)
{
    SensorEntry *temporary = calloc(sensorCount, sizeof(SensorEntry));

    if (temporary == NULL)
    {
        printf("[ERROR]Memory allocation failed, exiting program.\n\n");
        return NULL;
    }

    return temporary;
}

void entry(SensorEntry *sensor, int index)
{
    while(1)
    {
        // ID
        printf("Entry %d\n#ID: ", index);

        char bufferId[100];
        if (fgets(bufferId, sizeof(bufferId), stdin) == NULL )
        {
            printf("[ERROR] Please input a valid number!\n\n");
            continue;
        }

        char *endptr;
        long check = strtol(bufferId, &endptr, 10);

        if (bufferId == endptr) // Buffer array decays into pointer
        {
            printf("[ERROR] Please input a valid number!\n\n");
            continue;
        }

        while (*endptr != '\0' && isspace((unsigned char)*endptr))
        {
            endptr++;
        }
        if (*endptr != '\0')
        {
            printf("[ERROR] Trailing characters detected, please input values only!\n\n");
            continue;
        }
        else
        {
            sensor->id = check;
            break;
        }
    }

    while(1)
    {
        // NAME
        printf("Sensor name: ");
        char bufferName[32];
        if (fgets(bufferName, sizeof(bufferName), stdin) == NULL )
        {
            printf("[ERROR] Please input valid characters!\n\n");
            continue;
        }

        bufferName[strcspn(bufferName, "\n")] = '\0'; // Changes the target string to '\0'

        char *startptr = &bufferName[0];
        while (isspace((unsigned char)*startptr)) // Get rid of any leading spaces
        {
            startptr++;
        }

        if (*startptr == '\0')
        {
            printf("[ERROR] Input cannot be empty!\n\n");
            continue;
        }
        
        int valid = 1;
        for (int i = 0; startptr[i] != '\0'; i++)
        {
            if (!isalpha((unsigned char)startptr[i]) && !isspace((unsigned char)startptr[i])) // isalpha used to check for alphabetical values only 'A' or 'a'
            {
                valid = 0;
            }
        }
        if (valid == 0)
        {
            printf("[ERROR] Name can only contain letters and spaces!\n\n");
            continue;
        }

        strncpy(sensor->name, startptr, sizeof(sensor->name)); // Copying the whole string data into sensor->name
        break;
    }

    
    while(1)
    {
        // VALUE
        printf("Sensor value: ");

        char bufferValue[100];
        if (fgets(bufferValue, sizeof(bufferValue), stdin) == NULL )
        {
            printf("[ERROR] Please input a valid number!\n\n");
            continue;
        }

        char *endptr;
        double check = strtod(bufferValue, &endptr); // strtod is used to count for decimals (floats)

        if (bufferValue == endptr) // Buffer array decays into pointer
        {
            printf("[ERROR] Please input a valid number!\n\n");
            continue;
        }

        while (*endptr != '\0' && isspace((unsigned char)*endptr))
        {
            endptr++;
        }
        if (*endptr != '\0')
        {
            printf("[ERROR] Trailing characters detected, please input values only!\n\n");
            continue;
        }
        else
        {
            sensor->value = check;
            break;
        }
    }
    // If entry is completed, sensor is active
    sensor->active = 1;
    printf("\n");
}

int deactivateSensor(SensorEntry *sensor, int sensorCount, SensorEntry **pptr_deactivated_sensor)
{
    while(1)
    {
        // Target ID
        printf("Please input #ID of sensor to deactivate: ");

        char bufferTargetID[100];
        if (fgets(bufferTargetID, sizeof(bufferTargetID), stdin) == NULL )
        {
            printf("[ERROR] Please input a valid number!\n\n");
            continue;
        }

        char *endptr;
        long targetID = strtol(bufferTargetID, &endptr, 10);

        if (bufferTargetID == endptr) // Buffer array decays into pointer
        {
            printf("[ERROR] Please input a valid number!\n\n");
            continue;
        }

        while (*endptr != '\0' && isspace((unsigned char)*endptr))
        {
            endptr++;
        }
        if (*endptr != '\0')
        {
            printf("[ERROR] Trailing characters detected, please input values only!\n\n");
            continue;
        }
        
        for (int i = 0; i < sensorCount; i++) // Check if target ID exists first
        {
            if (sensor[i].id == targetID)
            {
                sensor[i].active = 0;
                *pptr_deactivated_sensor = &sensor[i];
                return targetID;
            }
        }

        // If loop fails to return, this block executes
        *pptr_deactivated_sensor = NULL;
        return targetID;
    }
}

float *averageCalculator(SensorEntry *sensor, float *ptr_average, int sensorCount)
{
    float sum = 0;
    for (int i = 0; i < sensorCount; i++)
    {
        if (sensorCount == 1 && sensor[0].active == 0)
        {
            *ptr_average = 0.0;
            return NULL;
        }

        else 
        {   
            if (sensor[i].active == 1)
            {
                sum = sum + (float)sensor[i].value;
            }
        }
    }

    int activeCount = 0; // Number of active sensors
    for (int i = 0; i < sensorCount; i++)
    {
        if (sensor[i].active == 1)
        {
            activeCount++;
        }
    }

    printf("%d\n", activeCount);

    *ptr_average = (float)sum / (float)(activeCount); 
    return ptr_average;
    
}

void printReport(SensorEntry *sensor, SensorEntry **pptr_deactivated_sensor, int deactivate_target_ID, int sensorCount, float *ptr_average)
{
    printf("\n==============================\n");
    printf("REPORT\n\n");

    // Sensor Count Print
    printf("Count: %d\n", sensorCount);
    printf("Sensors: ");

    // Sensor Count Entries Print
    int i = 0;
    while (i < sensorCount)
    {
        if (i < sensorCount - 1)
        {
            printf("{%d, ""%s"", %.2f}, ", sensor[i].id, sensor[i].name, sensor[i].value);
            i++;
        }

        if (i == sensorCount - 1)
        {
            printf("{%d, ""%s"", %.2f}\n\n", sensor[i].id, sensor[i].name, sensor[i].value); // Ending
            i++;
        }
    }

    // Deactivated Sensor Print
    if (pptr_deactivated_sensor != NULL && *pptr_deactivated_sensor != NULL)
    {
        printf("Deactivated ID: %d\n", (*pptr_deactivated_sensor)->id ); // Dereferences to the memory address of the pointer variable, which is then dereferenced again by ->
    }
    else
    {
        printf("[WARNING] Sensor #ID %d not found.\n", deactivate_target_ID);
    }
    
    // Active Average Print
    if (ptr_average != NULL)
    {
        printf("Active average: %.2f", *ptr_average);
    }
    else
    {
        printf("Active average: [WARNING] No active sensors.");
    }
}

/*
REVIEW:
-   Freeing temporary dynamic memory is not necessary in allocation function because the main array will take over the memory in main.
-   arr[strcspn(arr, target character)] = (new character): changes all target character into new character in the array.
-   isalpha((unsigned char)*ptr_arr[i]) used to check for alphabetical values only 'A' or 'a' in a loop.
-   strncpy(sensor->name, startptr, sizeof(sensor->name)); is used to copy the string into the targeted array.
-   double check = strtod(arr, **ptr): used for user float inputs, double (float value).
-   Only pointers can be NULL, other data types cannot be NULL.
-   For you to amend a variable (pointer/ non-pointer) that was declared in main through another function, you must always have a * for it to work.
*/
