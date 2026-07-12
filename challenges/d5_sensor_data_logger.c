/*
Day 5 Challenge: The Sensor Data Logger

Scenario:
You're writing firmware for a data logging device attached to a temperature sensor. Instead of reacting to one reading at a time, this device records a batch of readings,
then analyzes the whole batch — reporting the highest, lowest, and average temperature, and flagging how many readings were in the danger zone.
This is exactly how real embedded data loggers work — think black boxes, weather stations, or industrial monitors.
------------------------------------------------------------------------------------------------------------------------------------------
Task:
Write a C program using at least 4 functions alongside main():

1.  A function that collects temperature readings into an array — asks the user to enter exactly 5 temperatures (valid range: -40 to 125°C), storing each into an array. 
    Invalid inputs should be rejected and re-asked.
2.  A function that finds the highest and lowest temperature in the array and prints both.
3.  A function that calculates and prints the average temperature across all 5 readings.
4.  A function that counts and prints how many readings exceeded 75°C — the overheat threshold.
5.  In main(), call each function in order after the readings are collected.

Constraints:
-   Use an integer array of size 5 to store readings
-   Pass the array into each function — don't re-declare it in every function
-   No global variables
-   Each function does one job

Knowledge applied:
- Loop Safety: Used 'size_t' for loops to stop compiler warnings when comparing with array sizes.
- Input Control: Put 'i++' inside the 'else' block so the loop only moves forward when data is valid. This prevents bugs from index underflows.
- Decimal Math: Made 'sum' a float so C keeps the decimal points instead of chopping them off during division.
- Array Passing: Passed the array into functions using pointers to modify and read the same data across the program.
- Peak Finding: Used loops to compare array elements one-by-one to isolate the highest and lowest values.

*/

#include <stdio.h>

void readingsFunction(int *readings , size_t size)
{
    printf("[NOTE]: Valid range [(-40) - (125) celcius]\n");

    /*for (size_t i = 0; i < size;) // Not using i++ here so that if else conditions will determine if the increments happen or not
    {
        int check;
        
        printf("Please input temperature reading (%zu): ", i + 1);
        scanf("%d", &check);

        if (check < -40 || check > 125)
        {
            printf("[ERROR] Invalid temperature reading input, please try again.\n"); // No increment occurs
        }
        else
        {
            readings[i] = check;   
            i++; // Increment occurs, this prevents any bugs if i becomes negative because size_t cannot be negative     
        }
    }*/

    // Better option using while loop instead of a for loop
    size_t i = 0;
    while (i < size)
    {
        int check;
        printf("Please input temperature reading (%zu): ", i + 1);

        if (scanf("%d", &check) != 1) // scanf will return a 1 if the input is an integer, otherwise 0 
        {
            printf("[ERROR] Input must be a valid number. Please try again\n");

            while (getchar() != '\n'); // Clear invalid characters out of the input buffer
            continue;
        }

        if (check < -40 || check > 125)
        {
            printf("[ERROR] Invalid temperature reading input, please try again.\n"); // No increment occurs
        }
        else
        {
            readings[i] = check;
            i++;   
        }
    }
}

void findMinMax(int *readings, size_t size)
{
    int max = readings[0];
    int min = readings[0];

    for (size_t i = 1; i < size; i++) //  Changed all (int i) to (size_t i)
    {
        if (readings[i] > max)
        {
            max = readings[i];
        }

        if (readings[i] < min)
        {
            min = readings[i];
        }
    }

    printf("Minimum reading: %d\n", min);
    printf("Maximum reading: %d\n", max);
}

void averageFunction(int *readings, size_t size)
{
    float sum = 0;
    for (size_t i = 0; i < size; i++)
    {
        sum = sum + readings[i];
    }

    float average = sum / size; // Note: at least one side of the expression must be a float for the decimal to maintain the decimal value.
    printf("Average reading: %.2f\n", average);
}

void highReadingCountFunction(int *readings, size_t size)
{
    int highReadingCount = 0;
    for (size_t i = 0; i < size; i++)
    {
        if (readings[i] > 75)
        {
            highReadingCount = highReadingCount + 1;
        }
    }

    printf("Total times readings exceeded (75 degrees): %d\n", highReadingCount);
}

int main()
{
    int readings[5] = {0};
    size_t size = (sizeof(readings) / sizeof(readings[0]));

    printf("SENSOR DATA LOGGER\n");
    printf("-----------------------------------------------------\n");

    readingsFunction(readings, size);

    printf("-----------------------------------------------------\n");
    printf("ANALYSIS:\n");

    findMinMax(readings, size);

    averageFunction(readings, size);

    highReadingCountFunction(readings, size);

    return 0;
}

/*
REVIEW
-   For integer input loops, its better to increment i after checking the value so that the loop is dependant on valid inputs only
-   For a float expression, at least one side must be a float for the decimal to not be destroyed after calculation.
-   Min Max in arrays can be found using simple if conditions in loops.

*/