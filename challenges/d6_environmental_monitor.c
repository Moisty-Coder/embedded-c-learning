/*
Day 6 Challenge: The Environmental Monitor

Scenario:
You're building firmware for an environmental monitoring station — the kind used in greenhouses, server rooms, or weather stations. 
The device collects a batch of humidity readings (as percentages), analyzes them, and flags any readings that fall outside a safe operating range.
Same logging concept as Day 5, but with a different sensor type and a new analysis requirement — detecting and reporting out-of-range readings individually.
------------------------------------------------------------------------------------------------------------------------------------------
Task:
Write a C program using at least 4 functions alongside main():

1.  A function that collects humidity readings into an array — asks the user to enter exactly 6 readings (valid range: 0 to 100%). Invalid inputs should be rejected and re-asked.
2.  A function that finds and prints the highest and lowest reading — this time, also print which reading number (1–6) they occurred at.
3.  A function that calculates and prints the average humidity.
4.  A function that scans for out-of-range readings — any reading below 30% or above 70% is considered unsafe. 
    Print each unsafe reading and its position number. If none exist, print "[OK] All readings within safe range."
5. In main(), call each function in order after collection.

Constraints:
-   Integer array of size 6
-   Pass the array into each function — no global variables
-   Each function does one job
-   Apply what you learned in Day 5 — void returns where appropriate, correct loop style, float division for averages

Knowledge applied: Arrays, Pointers


*/

#include <stdio.h>

void readings(int *humidity, size_t size)
{
    size_t i = 0;
    while (i < size)
    {
        int check;
        printf("[Reading (%zu)] Please enter humidity percentage (%%): ", i + 1);
        
        if (scanf("%d", &check) != 1)
        {
            printf("[ERROR] Input must be a valid number. Please try again\n");

            while (getchar() != '\n'); // Clear invalid characters out of the input buffer
            continue;
        }

        if (check < 0 || check > 100)
        {
            printf("[ERROR] Invalid humidity reading input (0%% - 100%%), please try again.\n"); 
        }

        else
        {
            humidity[i] = check;
            i++;
        }
    }  
}

void maxminFunction (int *humidity, size_t size)
{
    int max = humidity[0];
    size_t recordedmax = 1; // Assign it to 1 first so that its not undefined if humidity[0] is the biggest or smallest

    int min = humidity[0];
    size_t recordedmin = 1;

    for (size_t i = 1; i < size; i++)
    {
        if (max < humidity[i])
        {
            max = humidity[i];
            recordedmax = i + 1;
        }

        if (min > humidity[i])
        {
            min = humidity[i];
            recordedmin = i + 1;
        }
    }

    printf("Highest Recorded Humidity: %d%%, recorded at reading (%zu)\n", max, recordedmax);
    printf("Lowest Recorded Humidity: %d%%, recorded at reading (%zu)\n", min, recordedmin);
}

void average(int *humidity, size_t size)
{
    float sum = 0;
    for (size_t i = 0; i < size; i++)
    {
        sum = sum + humidity[i];
    }

    float average = sum / size;

    printf("Average percentage reading: %.2f%%\n", average);

}

void safetyChecker(int *humidity, size_t size)
{
    int check = 0;
    for (size_t i = 0; i < size; i++)
    {
        if (humidity[i] < 30 || humidity[i] > 70)
        {
            printf("[WARNING] Unsafe humidity of %d%% detected at reading (%zu).\n", humidity[i], i + 1);
            check = 1;
        } 
    }

    if (check == 0)
    {
        printf("[OK] All readings within safe range.\n");
    }
}

int main()
{
    int humidity[6] = {0};
    size_t size = sizeof(humidity) / sizeof(humidity[0]);

    printf("ENVIRONMENTAL MONITOR\n");
    printf("-----------------------------------------------------\n");

    readings(humidity, size);
    printf("-----------------------------------------------------\n");
    printf("ANALYSIS:\n");

    maxminFunction(humidity, size);
    average(humidity, size);
    safetyChecker(humidity, size);

    return 0;
}