/*
Day 7 Challenge: The Weather Station Logger

Scenario:
You're building firmware for a personal weather station. 
Every reading cycle, the device captures both temperature (°C) and humidity (%) together as a paired snapshot. 
You'll store a batch of these snapshots and analyze the full dataset.
------------------------------------------------------------------------------------------------------------------------------------------
Task:
Define a struct called SensorReading with two fields: temperature (int) and humidity (int).
Then write 4 functions:

1.  A function that collects readings — fills an array of 5 SensorReading structs. Valid ranges: temperature –40 to 125°C, humidity 0 to 100%. Reject invalid inputs and re-ask.
2.  A function that prints all readings in a formatted table.
3.  A function that finds the hottest and most humid reading — prints which reading number had the highest temperature 
    and which had the highest humidity (they may be different readings).
4.  A function that flags dangerous conditions — a reading is dangerous if temperature exceeds 60°C AND humidity exceeds 60%. 
    Print each dangerous reading and its position, or "[OK] No dangerous conditions detected." if none exist.

Constraints:
-   Use a struct for paired data — no parallel arrays
-   Pass the array of structs into each function
-   No global variables
-   Apply everything you've learned: void where appropriate, size_t for sizes, float division for any averages


Knowledge applied: Structs, Arrays, Pointers
*/

#include <stdio.h>

typedef struct
{
    int temperature;
    int humidity;
    
} SensorReading;

// Prototype Funtions
void readings(SensorReading *reading,  size_t size);
void table(const SensorReading *reading,  size_t size); // const is used to prevent any accidental modification of data in the fucntion that only reads the data
void max(const SensorReading *reading,  size_t size);
void safetyChecker(const SensorReading *reading, size_t size);


int main()
{
    printf("WEATHER STATION LOGGER\n");

    SensorReading reading[5] = {0}; // Initializing
    size_t size = sizeof(reading) / sizeof(reading[0]); 

    readings(reading, size);
    printf("==============================\n");
    printf("ANALYSIS:\n\n");

    table(reading, size);
    printf("==============================\n");
    max(reading, size);
    printf("\n");
    safetyChecker(reading, size);

    return 0;
}

void readings(SensorReading *reading,  size_t size)
{
    printf("==============================\n");
    printf("READING INPUT\n");
    printf("[NOTE]: Valid temperature range [(-40) - (125) celcius]\n");
    printf("[NOTE]: Valid humidity range [0%% - 100%%] \n");

    for (size_t i = 0; i < size; i++)
    {   
        printf("\nReading %zu:\n", i + 1);

        while(1)
        {
            int temp_check;
            printf("Please input temperature reading: ");
            if (scanf("%d", &temp_check) != 1)
            {
                printf("[ERROR] Input must be a valid number. Please try again\n");

                while (getchar() != '\n'); // Clear invalid characters out of the input buffer
                continue;
            }

            if (temp_check < -40 || temp_check > 125)
            {
                printf("[ERROR] Invalid temperature reading input. Please try again.\n"); 
            }
            else
            {
                reading[i].temperature = temp_check;
                break;
            }
        }

        while (1)
        {
            int humidity_check;
            printf("Please input humidity reading: ");
            if (scanf("%d", &humidity_check) != 1)
            {
                printf("[ERROR] Input must be a valid number. Please try again\n");

                while (getchar() != '\n'); // Clear invalid characters out of the input buffer
                continue;
            }

            if (humidity_check < 0 || humidity_check > 100)
            {
                printf("[ERROR] Invalid humidity reading input. Please try again.\n"); 
            }
            else
            {
                reading[i].humidity = humidity_check; 
                break;
            }
        }
    }   
}

void table(const SensorReading *reading,  size_t size)
{
    printf("TABLE OF READINGS:\n");
    printf("%-8s | %-17s | %-15s\n", "Reading", "Temperature (*C)", "Humidity (%)"); // %-(width)(format specifier) = left alligned text

    for (size_t i = 0; i < size; i++)
    {
        printf("%-8zu | %-17d | %-15d\n", i + 1 ,reading[i].temperature, reading[i].humidity);
    }

}

void max(const SensorReading *reading,  size_t size)
{
    int maxtemp = reading[0].temperature;
    int maxhumidity = reading[0].humidity;

    size_t readingtemp = 1;
    size_t readinghumidity = 1;

    for (size_t i = 1; i < size; i++)
    {
        if (reading[i].temperature > maxtemp)
        {
            maxtemp = reading[i].temperature;
            readingtemp = i + 1;
        }

        if (reading[i].humidity > maxhumidity)
        {
            maxhumidity = reading[i].humidity;
            readinghumidity = i + 1;
        }
    }

    printf("Highest Recorded Temperature: %d *C, recorded at reading (%zu)\n", maxtemp, readingtemp);
    printf("Highest Recorded Humidity: %d%%, recorded at reading (%zu)\n", maxhumidity, readinghumidity);
}

void safetyChecker(const SensorReading *reading, size_t size)
{
    int check = 0;
    for (size_t i = 0; i < size; i++)
    {
        if (reading[i].temperature > 60 && reading[i].humidity > 60)
        {
            printf("[WARNING] Reading %zu {%d*C, %d%%}\n", i + 1, reading[i].temperature, reading[i].humidity);
            check = 1;
        } 
    }

    if (check == 0)
    {
        printf("[OK] All readings within safe range.\n");
    }
}

/*
REVIEW
- For multi-step data structures, isolate inputs into separate while(1) loops to prevent out-of-order retries.
- Use the 'const' keyword on pointers in functions that only read data to guarantee immutability.
- Match format specifiers strictly to data types (e.g., %zu for size_t and %d for int) to prevent platform-specific bugs.
- Keep function declarations (prototypes) and definitions identical to maintain a clean codebase.
- Avoid "magic numbers" in logic loops; manage boundaries using preprocessor macros or constants.
*/
