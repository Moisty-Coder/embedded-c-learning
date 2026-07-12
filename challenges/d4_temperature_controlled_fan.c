
/*
Day 4 Challenge: The Temperature Controlled Fan

Scenario:
You're writing firmware for a temperature-controlled cooling fan inside an electronics enclosure — think of a server rack or a power supply unit.
The microcontroller reads the internal temperature and automatically adjusts the fan speed based on how hot it is. 
It also tracks how long the fan has been running at high speed to flag potential overheating events.

------------------------------------------------------------------------------------------------------------------------------------------
Task:
Write a C program using at least 3 functions alongside main():

1.  A function that reads and validates a temperature input — accepts temperatures between -40 and 125 (the typical range for embedded sensors, in °C). 
    Anything outside this range should print an error and ask again.

2.  A function that determines fan speed — given a temperature, returns a fan speed level as an integer:

    Temperature         Fan Speed
    Below 30°C          0 — Off
    30°C – 49°C         1 — Low
    50°C – 74°C         2 — Medium
    75°C and above      3 — High

3. A function that displays the current system status — given the temperature and fan speed level, prints a formatted status message:

    Speed Level         Message
    0                   "[FAN OFF] Temp: X°C. System cool."
    1                   "[FAN LOW] Temp: X°C. Monitoring."
    2                   "[FAN MED] Temp: X°C. Warming up."
    3                   "[FAN HIGH] Temp: X°C. OVERHEAT WARNING!"

4. Then in main(), keep reading temperatures in a loop. Count how many readings triggered Fan High, and after the user exits, print the total.

Constraints:
No global variables — use parameters and return values
Each function does one job
Reuse the lessons from Day 3 — think about scope and where your counter lives

Knowledge applied: Functions with return values, Loops, Basic syntax

*/

#include <stdio.h>
#include <string.h>

int tempInput()
{
    while (1)
    {
        int temp;
        printf("Please input a valid temperature ([-40] - [125] degrees): ");
        scanf("%d", &temp);

        if (temp < -40 || temp > 125)
        {
            printf("Invalid input, please try again.\n");
            continue; // Skips to the next iteration
        }
        
        else
        {
            return temp;
        }
    }
}

int fanSpeed(int realtemp)
{
    if (realtemp < 30)
    {
        return 0;
    }
    else if (realtemp < 50)
    {
        return 1;
    }
    else if (realtemp < 75)
    {
        return 2;
    }
    else
    {
        return 3;
    } 
}

int systemStatus(int realtemp, int fan_speed, int fan_high_triggered)
{
    if (fan_speed == 0)
    {
        printf("[FAN OFF] Temp: %d degree C. System cool.\n", realtemp);
        return fan_high_triggered;
    }
    else if (fan_speed == 1)
    {
        printf("[FAN LOW] Temp: %d degree C. Monitoring.\n", realtemp);
        return fan_high_triggered;
    }
    else if (fan_speed == 2)
    {
        printf("[FAN MED] Temp: %d degree C. Warming up.\n", realtemp);
        return fan_high_triggered;
    }
    else if (fan_speed == 3)
    {
        printf("[FAN HIGH] Temp: %d degree C. OVERHEAT WARNING!\n", realtemp);
        fan_high_triggered ++; // Increment the existing value
        return fan_high_triggered; // Return the updated value
    }
}

int main()
{
    printf("TEMPERATURE CONTROLLED FAN\n");
    printf("==========================\n");
    int fan_high_triggered = 0; // Start value at 0

    while(1)
    {
        int realtemp = tempInput();

        int fan_speed = fanSpeed(realtemp);

        fan_high_triggered = systemStatus(realtemp, fan_speed, fan_high_triggered); // Function uses existing value of fan_high_triggered and updates it in every iteration.

        char user_input[5];
        printf("Do you wish to continue? (yes/no): ");
        scanf("%s", user_input); // & is not necessary for char arrays

        if (strcmp(user_input, "yes") == 0)
        {
            continue;
        }
        else if (strcmp(user_input, "no") == 0)
        {
            printf("EXITING PROGRAM\n");
            printf("Number of times high fan speed was triggered: %d\n", fan_high_triggered);
            break;
        }
    }

    return 0;
}

/*
REVIEW
-   "return" end the function containing the loop, so break is not needed in these cases.

    Table for Loop Controls:

    Keyword     What it Ends                        Does Code After the Loop Still Run?         Typical Use Case
    return      Exits the entire function           No. Function stops instantly                Finding a target value and exiting early
    break       Exits only the current loop         Yes. Execution moves to the next lines      Stopping a loop without leaving the function
    continue    Exits only the current iteration    Yes. The loop skips to the next item        Skipping unwanted values or invalid data

-   Suggestion to let main handle the increments of fan triggers, but it works for now.

*/

