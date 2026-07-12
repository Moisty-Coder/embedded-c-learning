
/*Day 1 Challenge: The Temperature Safety Monitor
Imagine you're programming a microcontroller connected to a temperature sensor inside an industrial machine.
The sensor reads the current temperature and your job is to write a program that monitors the temperature and reports the system status to an operator display.

Write a C program that:

Takes a temperature value as input (in degree C, as an integer)
Classifies the system status based on these thresholds:

Knowledge applied: if/else conditions and basic syntax for printf and scanf

*/

#include <stdio.h>

int main()
{
    printf("TEMPERATURE SAFETY MONITOR\n");
    printf("===================================\n");

    int temp;
    printf("Please input temperature value (degrees C): ");
    scanf("%d", &temp );

    if (temp < 0)
    printf("ERROR: Sensor fault - reading too low\n");

    else if (temp <= 39)
    printf("Status: NORMAL - System running cool\n");

    else if (temp <= 69)
    printf("Status: WARNING - Temperature elevated\n");

    else if (temp <= 99)
    printf("Status: DANGER - Overheating detected\n");
    
    else
    printf("Status: SHUTDOWN - Critical temperature\n");

    printf("Reading: %d degrees C\n", temp);

    return 0;
}

/*
REVIEW
- Use int main() for standard good practice, avoid using void().
- Using scanf stores the value into the adress of the variable, so "&" is required.
- Try to keep if else statements into one block to improve readability, avoid splitting.
*/