
/*
Day 3 Challenge: The Traffic Light Controller

Scenario:
You're prototyping firmware for a simple traffic light system at a pedestrian crossing.
The microcontroller needs to cycle through light states, respond to a pedestrian button press, and track how many times the pedestrian signal was triggered.
In real hardware, each light state would activate GPIO pins. For now, you'll simulate it in C using the terminal.

------------------------------------------------------------------------------------------------------------------------------------------
Task:
Write a C program that uses AT LEAST 3 functions alongside main():

1.  A function that displays the current light state — given an integer input (1, 2, or 3), it prints the corresponding traffic light status:

    Input       Output
    1           "[RED] Stop. Pedestrian crossing active."
    2           "[YELLOW] Caution. Prepare to move."
    3           "[GREEN] Go."

2.  A function that asks if a pedestrian is waiting — returns 1 if yes, 0 if no. Any input other than 1 or 0 should print an error and ask again.

3.  A function that runs one full light cycle — calls the display function for RED → YELLOW → GREEN in order.
    If a pedestrian is waiting (check at the start of the cycle), display the RED state, then ask the pedestrian to cross, then continue to YELLOW → GREEN.

4.  Then in main(), keep running full cycles until the user decides to stop, and print the total number of pedestrian crossings triggered.
------------------------------------------------------------------------------------------------------------------------------------------
Constraints

No global variables — pass values between functions using parameters and return values
Each function must do one job only
Use your loop and if/else skills from previous days inside the functions

Knowledge applied: Functions with return values, Loops, Basic syntax

*/

#include <stdio.h>
#include <string.h>

// Function 1
void status(int mainResult) // passing argument from status(mainResult)
{
    for (int i = 1; i <= 3; i++ )
    if (i == 1)
    {
        printf("[RED] Stop. Pedestrian crossing active.\n");

        if (mainResult == 1)
        {
            printf("[PEDESTRIAN] PLEASE CROSS NOW.\n");
            continue;
        }

        else
        {
            continue;
        }

    }

    else if (i == 2)
    {
        printf("[YELLOW] Caution. Prepare to move.\n");
    }

    else if (i == 3)
    {
        printf( "[GREEN] Go.\n");
    }

}

// Function 2
int pedestrian()
{
    while(1)
    {
        int result;
        char check[5];
        printf("Is a pedestrian waiting? (yes/no): ");
        scanf("%s", check);

        if (strcmp(check, "yes") == 0)
        {
            result = 1;
            return result;
        }
        else if (strcmp(check, "no") == 0)
        {
            result = 0;
            return result; //returning value of 'result' into 'mainResult'
        }
        else
        {
            printf("[ERROR] Invalid input, please enter (yes/no)\n");
        }
    }
}

//Function 3
int cycle()
{
    int mainResult;
    mainResult = pedestrian();

    int crossingTriggered = 0;

    if (mainResult == 1)
    {
        crossingTriggered ++;
    }

    status(mainResult);

    return crossingTriggered;
}

int main()
{
    printf("TRAFFIC LIGHT CONTROLLER\n");
    printf("========================\n");

    int totalCrossingsTriggered = 0;

    while(1)
    {
        int crossingTriggered = cycle();

        totalCrossingsTriggered = totalCrossingsTriggered + crossingTriggered;

        char input[5];
        printf("Continue? (yes/no): ");
        scanf("%s", input);

        if (strcmp(input, "yes") == 0)
        {
            continue;
        }
        else if (strcmp(input, "no") == 0)
        {
            printf("Total crossings triggered: %d", totalCrossingsTriggered);
            break;
        }
    }

    return 0;
}

/*
REVIEW
-   Difficult due to constraints and several logical inputs.
-   Function that relies on return values, good practice for restricting global variable usage.
-   Functions that return values or strings must have 'int' or 'char', depending on the returned datatype.
-   Returned datatype can be stored as: storeddata = returningfunction();
-   Accumulating value (totalCrossingsTriggered) must be declared outside of the loop first, then for it to increment from crossingsTriggered, it must be in the form of (new value) = (old value) + (returned)
-   strcmp is used to compare strings,  strcmp("yes" == "yes") = 0
                                        strcmp("yes" == "no") != 0
*/