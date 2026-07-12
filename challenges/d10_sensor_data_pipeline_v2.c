/*
Day 10: The Sensor Data Pipeline v2

Scenario:
You're writing a multi-stage sensor data processing pipeline for a three-channel environmental monitor. 
Each channel collects 4 raw readings, applies calibration, computes statistics, and ranks channels by criticality. 
The pipeline stages communicate exclusively through pointer parameters — no stage returns a value directly.
------------------------------------------------------------------------------------------------------------------------------------------
Task:
Define a struct called Channel with these fields: raw[4] (int array), calibrated[4] (float array), mean (float), deviation (float), and critical (int).

Then write 5 functions:

1.  A function that collects raw inputs — fills the raw[4] array for a single Channel through a pointer. Valid range: 0 to 1023 (typical 10-bit ADC range). 
    Reject invalid inputs.
2.  A function that calibrates a channel — applies calibrated[i] = (raw[i] * scale) + offset for each element. 
    Takes a Channel pointer, float scale, float offset. Writes through the pointer.
3.  A function that computes statistics — takes a Channel pointer and writes the mean and peak deviation (the calibrated value furthest from the mean) directly into the 
    struct's fields through the pointer. No return value.
4.  A function that ranks channels by criticality — takes three Channel pointers and a Channel **ranked[3] parameter. 
    Writes the three channels into ranked in order from most to least critical (highest deviation = most critical). 
    Communicates result exclusively through the pointer parameter.
5.  A function that prints the full report — shows for each channel: all 4 raw values, all 4 calibrated values, mean, deviation, and the 
    final criticality ranking.

In main(), declare three Channel variables, collect inputs, run the full pipeline, then print the report.
------------------------------------------------------------------------------------------------------------------------------------------
Calibration Parameters

Channel     Scale       Offset
Channel A   0.1         0.0
Channel B   0.2        -5.0 
Channel C   0.15        2.5

Knowledge applied: Structs, Arrays, Pointers, Double pointers
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

// Scale
const float channel_A_scale = 0.10;
const float channel_B_scale = 0.20;
const float channel_C_scale = 0.15;

// Offset
const float channel_A_offset = 0.00;
const float channel_B_offset = -5.00;
const float channel_C_offset = 2.5;

typedef struct Channel
{
    int raw[4];
    float calibrated[4];
    float mean;
    float deviation;
    int critical;
} Channel;

typedef enum Channel_index
{
    channel_A_type = 'A',
    channel_B_type,
    channel_C_type
} Channel_index;

// Prototype Functions
void rawInput(Channel *channel, size_t size, Channel_index channel_type);
void calibration(Channel *channel, size_t size, float scale, float offset);
void statistics(Channel *channel, size_t size);
void ranking(Channel *channel_A, Channel *channel_B, Channel *channel_C, Channel **ranked);
void printReport(Channel *channel_A, Channel *channel_B, Channel *channel_C, size_t size, Channel **ranked);

int main()
{
    // Defining Variables
    Channel channel_A;
    Channel *ptr_Channel_A = &channel_A;

    Channel channel_B;
    Channel *ptr_Channel_B = &channel_B;

    Channel channel_C;
    Channel *ptr_Channel_C = &channel_C;

    Channel *ranked[3] = {NULL}; // one * because arrays decay into level 1 pointers

    size_t size = sizeof(channel_A.raw) / sizeof(channel_A.raw[0]);

    printf("SENSOR DATA PIPELINE\n");

    rawInput(ptr_Channel_A, size, channel_A_type);
    rawInput(ptr_Channel_B, size, channel_B_type);
    rawInput(ptr_Channel_C, size, channel_C_type);

    calibration(ptr_Channel_A, size, channel_A_scale, channel_A_offset);
    calibration(ptr_Channel_B, size, channel_B_scale, channel_B_offset);
    calibration(ptr_Channel_C, size, channel_C_scale, channel_C_offset);

    statistics(ptr_Channel_A, size);
    statistics(ptr_Channel_B, size);
    statistics(ptr_Channel_C, size);

    ranking(ptr_Channel_A, ptr_Channel_B, ptr_Channel_C, ranked);

    printReport(ptr_Channel_A, ptr_Channel_B, ptr_Channel_C, size, ranked);
    
    return 0;
}

void rawInput(Channel *channel, size_t size, Channel_index channel_index) // Improved input reading function (not using scanf)
{
    printf("==============================\n");
    printf("CHANNEL %c\n", channel_index);
    printf("[NOTE] Valid range: 0 - 1023\n\n");

    size_t i = 0;

    while(i < size)
    {
        char buffer[100];
        printf("READING %zu\n", i + 1);
        printf("Please input valid raw reading: ");
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) // fgets(input_string, size of character array, standard input)
        {
            printf("[ERROR] Input must be a valid number. Please try again\n\n");
            continue;
        }

        char *endptr;
        long check = strtol(buffer, &endptr, 10); // strtol(input_string, address of endptr, base decimal(10))
        // strtol checks each character in the string input for a valid digit from left to right, if it detects non digit values, it stops and points endptr
        // towards the invalid character

        // if the first character is a non value (excluding negative sign)
        if (buffer == endptr)
        {
            printf("[ERROR] Input must be a valid number. Please try again\n\n");
            continue;
        }

        while (*endptr != '\0' && isspace((unsigned char)*endptr))  // Checks if the character pointed too is not a '\0' (end of the string) and is a whitespace '\n'
        {
            endptr++;
        }

        if (*endptr != '\0') // If the end of the string is not '\0', invalid input
        {
            printf("[ERROR] Trailing characters detected. Please input numbers only\n\n");
            continue;
        }

        if (check < 0 || check > 1023)
        {
            printf("[ERROR] Input must be within the specified range. Please try again\n\n");
            continue;
        }
        else
        {
            channel->raw[i] = check;
            i++;
        }
    }
}

void calibration(Channel *channel, size_t size, float scale, float offset)
{
    for (size_t i = 0; i < size; i++)
    {
        channel->calibrated[i] = (channel->raw[i] * scale) + offset;
    }
}

void statistics(Channel *channel, size_t size)
{
    float sum = 0;
    for (size_t i = 0; i < size; i++)
    {
        sum = sum + channel->calibrated[i];
    }

    channel->mean = (float)sum / (float)size;

    float furthest_from_mean = 0;

    for (size_t i = 0; i < size; i++)
    {
        float check = 0;
        check = fabsf(channel->calibrated[i] - channel->mean); // absolute float value from the midpoint
        
        if (check > furthest_from_mean)
        {
            furthest_from_mean = check;
        }
    }

    channel->deviation = furthest_from_mean; // Final deviation value
}

void ranking(Channel *channel_A, Channel *channel_B, Channel *channel_C, Channel **ranked)
{
    if (channel_A->deviation > channel_B->deviation && channel_A->deviation > channel_C->deviation)
    {
        ranked[0] = channel_A;
        if (channel_B->deviation > channel_C->deviation)
        {
            ranked[1] = channel_B;
            ranked[2] = channel_C;
        }
        else
        {
            ranked[1] = channel_C;
            ranked[2] = channel_B;
        }
    }
    else if (channel_B->deviation > channel_A->deviation && channel_B->deviation > channel_C->deviation)
    {
        ranked[0] = channel_B;
        if (channel_A->deviation > channel_C->deviation)
        {
            ranked[1] = channel_A;
            ranked[2] = channel_C;
        }
        else
        {
            ranked[1] = channel_C;
            ranked[2] = channel_A;
        }
    }
    else if (channel_C->deviation > channel_A->deviation && channel_C->deviation > channel_B->deviation)
    {
        ranked[0] = channel_C;
        if (channel_A->deviation > channel_B->deviation)
        {
            ranked[1] = channel_A;
            ranked[2] = channel_B;
        }
        else
        {
            ranked[1] = channel_B;
            ranked[2] = channel_A;
        }
    }
    else
    {
        ranked[0] = channel_A;
        ranked[1] = channel_B;
        ranked[2] = channel_C;
    }
}

void printReport(Channel *channel_A, Channel *channel_B, Channel *channel_C, size_t size, Channel **ranked)
{
    printf("\n==============================\n");
    printf("REPORT\n\n");

    printf("Channel A raw: ");
    for (size_t i = 0; i < size; i++)
    {
        if (i < size - 1)
        {
            printf("%d, ", channel_A->raw[i]);
        }

        if (i == size - 1)
        {
            printf("%d\n", channel_A->raw[i]);
        }
    }

    printf("Channel B raw: ");
    for (size_t i = 0; i < size; i++)
    {
        if (i < size - 1)
        {
            printf("%d, ", channel_B->raw[i]);
        }

        if (i == size - 1)
        {
            printf("%d\n", channel_B->raw[i]);
        }
    }

    printf("Channel C raw: ");
    for (size_t i = 0; i < size; i++)
    {
        if (i < size - 1)
        {
            printf("%d, ", channel_C->raw[i]);
        }

        if (i == size - 1)
        {
            printf("%d\n\n", channel_C->raw[i]);
        }
    }

    printf("Channel A calibrated: ");
    for (size_t i = 0; i < size; i++)
    {
        if (i < size - 1)
        {
            printf("%.2f, ", channel_A->calibrated[i]);
        }

        if (i == size - 1)
        {
            printf("%.2f ", channel_A->calibrated[i]);
        }
    }

    printf("| mean: %.2f | deviation: %.2f\n", channel_A->mean, channel_A->deviation);

    
    printf("Channel B calibrated: ");
    for (size_t i = 0; i < size; i++)
    {
        if (i < size - 1)
        {
            printf("%.2f, ", channel_B->calibrated[i]);
        }

        if (i == size - 1)
        {
            printf("%.2f ", channel_B->calibrated[i]);
        }
    }

    printf("| mean: %.2f | deviation: %.2f\n", channel_B->mean, channel_B->deviation);

    printf("Channel C calibrated: ");
    for (size_t i = 0; i < size; i++)
    {
        if (i < size - 1)
        {
            printf("%.2f, ", channel_C->calibrated[i]);
        }

        if (i == size - 1)
        {
            printf("%.2f ", channel_C->calibrated[i]);
        }
    }

    printf("| mean: %.2f | deviation: %.2f\n\n", channel_C->mean, channel_C->deviation);
        
    

    if (ranked[0] == channel_A)
    {
        if (ranked[1] == channel_B)
        {
            printf("Ranking: A (%.2f) -> B (%.2f) -> C (%.2f)", channel_A->deviation, channel_B->deviation, channel_C->deviation);
        }
        else
        {
            printf("Ranking: A (%.2f) -> C (%.2f) -> B (%.2f)", channel_A->deviation, channel_C->deviation, channel_B->deviation);
        }
    }
    if (ranked[0] == channel_B)
    {
        if (ranked[1] == channel_A)
        {
            printf("Ranking: B (%.2f) -> A (%.2f) -> C (%.2f)", channel_B->deviation, channel_A->deviation, channel_C->deviation);
        }
        else
        {
            printf("Ranking: B (%.2f) -> C (%.2f) -> A (%.2f)", channel_B->deviation, channel_C->deviation, channel_A->deviation);
        }
    }
    if (ranked[0] == channel_C)
    {
        if (ranked[1] == channel_A)
        {
            printf("Ranking: C (%.2f) -> A (%.2f) -> B (%.2f)", channel_C->deviation, channel_A->deviation, channel_B->deviation);
        }
        else
        {
            printf("Ranking: C (%.2f) -> B (%.2f) -> A (%.2f)", channel_C->deviation, channel_B->deviation, channel_A->deviation);
        }
    }
}

/*
REVIEW:
-   Arrays decay into pointers when called, so an double pointer is in the form of *arr[]
-   enum support 'A', 'B' iteration automatically, but cant support strings
-   fgets() is used to take user input as an array of characters
-   strtol() is used to take sign values +/- (if present), and digits only, it stops at the first non-digit and points the pointer towards that address.
-   isspace() is used to check for whitespaces
-   An array of pointers is possible through struct MyStruct *arr[SIZE], where it can point towards the address of each block of struct variable.
*/