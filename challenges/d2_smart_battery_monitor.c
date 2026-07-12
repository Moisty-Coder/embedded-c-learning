
/*
Day 2 Challenge: Write a C program that:

Asks the user to enter the current battery percentage (a whole number from 0 to 100)
Reads multiple readings in a loop — keep asking for battery readings until the user enters -1 to stop (this simulates continuous monitoring)
After each reading, prints a status message based on these rules:

Battery Level       Status to Print
81% – 100%          "[OK] Battery is full."
51% – 80%           "[OK] Battery is good."
21% – 50%           "[WARN] Battery is low. Consider charging."
1% – 20%            "[WARN] Battery is critically low!"
0%                  "[CRITICAL] Battery empty. Shutting down."

Count how many warnings were triggered (any [WARN] or [CRITICAL] reading counts as one warning)
When the user exits (enters -1), print the total number of readings taken and the total number of warnings triggered

Constraints:

Use a loop to keep reading values
Use if/else if/else for the status logic
Use simple integer variables for counting — no arrays needed
If the user enters a number below -1 or above 100, print "[ERROR] Invalid input. Enter 0-100 or -1 to quit." and do not count it as a reading

Knowledge applied: if/else conditions, basic loop applications (while loop)

*/

#include <stdio.h>

int main()
{
    printf("SMART BATTERY MONITOR\n");
    printf("=====================\n");

    int count = 0;
    int warning = 0;
    int battery;
    while (1) // can be while (1) or while (true)
    {
        printf("Input current battery percentage: ");
        scanf("%d", &battery);

        if (battery < -1 || battery > 100)
        {
            printf("[ERROR] Invalid input. Enter 0-100 or -1 to quit.\n");
        }

        else if (battery == -1)
        {
            printf("EXITING PROGRAM.\n");
            printf("=====================\n");
            printf("Number of readings taken: %d\n", count);
            printf("Total warnings triggered: %d\n", warning);
            break;
        }

        else if (battery == 0)
        {
            printf("[CRITICAL] Battery empty. Shutting down.\n");
            count ++;
            warning ++;
        }

        else if (battery <= 20)
        {
            printf("[WARN] Battery is critically low!\n");
            count ++;
            warning++;
        }
        
        else if (battery <= 50)
        {
            printf("[WARN] Battery is low. Consider charging.\n");
            count ++;
            warning ++;
        }

        else if (battery <= 80)
        {
            printf("[OK] Battery is good.\n");
            count ++;
        }

        else if (battery <= 100)
        {
            printf("[OK] Battery is full.\n");
            count ++;
        }
    }

    return 0;
}

/*
REVIEW
- Use while(1)/ while(true) for infinite loops that rely on "break;" to end the loop.
- Good use of if else conditions using intervals of (<=), more systematic.
- Use brackets for blocks, even if its only one line (for bug prevention in future projects).
- Use || (or) for conditions that can be combined, avoid splitting to enhance readability.
*/