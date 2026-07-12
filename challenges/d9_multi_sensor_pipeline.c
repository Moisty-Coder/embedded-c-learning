/*
Day 9: The Multi-Sensor Pipeline 

Scenario:
You're writing firmware for an industrial monitoring pipeline — a system that reads from three different sensors (temperature, pressure, humidity), 
processes the raw data through a calibration and filtering stage, and outputs a final system health verdict. 
In real embedded systems, data pipelines like this pass readings through multiple processing stages, each transforming the data before passing it forward.
The key challenge today: each processing stage communicates exclusively through pointers — no returning values directly.
------------------------------------------------------------------------------------------------------------------------------------------
Task:
Define a struct called SensorNode with these fields: raw (int), calibrated (float), filtered (float), and valid (int).
Then write 5 functions:

1.  A function that calibrates a single SensorNode — applies a scale factor and offset using the formula: calibrated = (raw * scale) + offset. 
    Takes a pointer to a SensorNode, a float scale, and a float offset. Writes the result directly through the pointer.
2.  A function that filters a calibrated reading — smooths the value using a simple running average formula: filtered = (calibrated * 0.7) + (previous * 0.3). 
    Takes a pointer to a SensorNode and the previous filtered value as a float. Writes the result through the pointer.
3.  A function that validates a node — takes a pointer to a SensorNode, a float min, a float max, and writes 1 or 0 to the node's valid field through the pointer.
4.  A function that finds the most critical sensor — defined as the valid sensor whose filtered value is furthest from its midpoint (min + max) / 2. 
    Takes three SensorNode pointers and writes the result through a SensorNode **result parameter.
5.  A function that prints the full pipeline report — shows raw, calibrated, filtered, and valid status for all three sensors, plus which sensor is most critical.

In main(), declare three SensorNode variables (temperature, pressure, humidity), collect raw inputs, run each through the full pipeline in order, then print the report.
------------------------------------------------------------------------------------------------------------------------------------------
Valid Ranges & Calibration Parameters:

Sensor          Scale       Offset      Min         Max
Temperature     0.5         -10.0       -40.0       125.0
Pressure        2.0         5.0         0.0         200.0
Humidity        0.25        20.0        0.0         100.0

For filtering, use the previous sensor's filtered value as previous — temperature uses 0.0 as its initial previous value, pressure uses temperature's filtered value, 
humidity uses pressure's filtered value.

Knowledge applied: Structs, Arrays, Pointers, Double pointers
*/

#include <stdio.h>

typedef struct SensorNode
{
    float raw;
    float calibrated;
    float filtered;
    int valid;
} SensorNode;

// Prototype Functions
void getInput(SensorNode *temperature, SensorNode *pressure, SensorNode *humidity);
void calibration(SensorNode *reading, float scale, float offset);
void filter(SensorNode *reading, float previous);
void validate(SensorNode *reading, float min, float max);
int criticalSensor(SensorNode *temperature, SensorNode *pressure, SensorNode *humidity, SensorNode **pptr_result);
void printReport(SensorNode *temperature, SensorNode *pressure, SensorNode *humidity, SensorNode **pptr_result, int status);

int main()
{
    printf("MULTI SENSOR PIPELINE\n");
    
    SensorNode temperature;
    SensorNode *ptr_temperature = &temperature;

    SensorNode pressure;
    SensorNode *ptr_pressure = &pressure;
    
    SensorNode humidity;
    SensorNode *ptr_humidity = &humidity;

    SensorNode *ptr_result = NULL;
    SensorNode **pptr_result = &ptr_result;

    getInput(ptr_temperature, ptr_pressure, ptr_humidity);

    calibration(ptr_temperature, 0.5, -10.0);
    calibration(ptr_pressure, 2.0, 5.0);
    calibration(ptr_humidity, 0.25, 20.0);

    filter(ptr_temperature, 0.0);
    filter(ptr_pressure, ptr_temperature->filtered);
    filter(ptr_humidity, ptr_pressure->filtered);   

    validate(ptr_temperature, -40.0, 125.0);
    validate(ptr_pressure, 0.0, 200.0);
    validate(ptr_humidity, 0.0, 100.0);

    int status = criticalSensor(ptr_temperature, ptr_pressure, ptr_humidity, pptr_result);

    printReport(ptr_temperature, ptr_pressure, ptr_humidity, pptr_result, status);

    return 0;
}

void getInput(SensorNode *temperature, SensorNode *pressure, SensorNode *humidity)
{
    printf("==============================\n");
    printf("READING INPUT\n");

    while(1)
    {
        float check;
        printf("Please input valid raw temperature reading: ");
        if (scanf("%f", &check) != 1)
        {
            printf("[ERROR] Input must be a valid number. Please try again\n\n");
            while (getchar() != '\n');
            continue;
        }
        else
        {
            temperature->raw = check;
            break;
        }
    }
    while(1)
    {
        float check;
        printf("Please input valid raw pressure reading: ");
        if (scanf("%f", &check) != 1)
        {
            printf("[ERROR] Input must be a valid number. Please try again\n\n");
            while (getchar() != '\n');
            continue;
        }
        else
        {
            pressure->raw = check;
            break;
        }
    }
    while(1)
    {
        float check;
        printf("Please input valid raw humidity reading: ");
        if (scanf("%f", &check) != 1)
        {
            printf("[ERROR] Input must be a valid number. Please try again\n\n");
            while (getchar() != '\n');
            continue;
        }
        else
        {
            humidity->raw = check;
            break;
        }
    }
}

void calibration(SensorNode *reading, float scale, float offset)
{
    reading->calibrated = (reading->raw * scale) + offset;
}   

void filter(SensorNode *reading, float previous) // previous is the last calibrated filter reading
{
    reading->filtered = (reading->calibrated * 0.7) + (0.3 * previous);
}

void validate(SensorNode *reading, float min, float max)
{
    if (reading->filtered < min || reading->filtered > max)
    {
        reading->valid = 0; // Invalid
    }
    else
    {
        reading->valid = 1; // Valid
    }
}

int criticalSensor(SensorNode *temperature, SensorNode *pressure, SensorNode *humidity, SensorNode **pptr_result)
{
    float temp_radius = 0;
    float pressure_radius = 0;
    float humidity_radius = 0;
    

    if (temperature->valid == 1)
    {
        float midpoint = (-40.0f + 125.0f) / 2.0f;
        temp_radius = (temperature->filtered < midpoint) ? (midpoint - temperature->filtered) : (temperature->filtered - midpoint);
        // using the <math.h>, fabsf() can be used as fabsf(temperature->filtered - midpoint); to get the absolute value
    }
    if (pressure->valid == 1)
    {
        float midpoint = (0.0f + 200.0f) / 2.0f;
        pressure_radius = (pressure->filtered < midpoint) ? (midpoint - pressure->filtered) : (pressure->filtered - midpoint);
    }
    if (humidity->valid == 1)
    {
        float midpoint = (0.0f + 100.0f) / 2.0f;
        humidity_radius = (humidity->filtered < midpoint) ? (midpoint - humidity->filtered) : (humidity->filtered - midpoint);
    }

    // Check if all sensors are invalid or exactly at midpoint 0
    if (temp_radius == 0 && pressure_radius == 0 && humidity_radius == 0)
    {
        *pptr_result = NULL;
        return 0; // No single critical sensor
    }

    // Check for unique strict maximums
    if (temp_radius > pressure_radius && temp_radius > humidity_radius)
    {
        *pptr_result = temperature;
        return 4; // Code for single sensor found
    }
    if (pressure_radius > temp_radius && pressure_radius > humidity_radius)
    {
        *pptr_result = pressure;
        return 4;
    }
    if (humidity_radius > temp_radius && humidity_radius > pressure_radius)
    {
        *pptr_result = humidity;
        return 4;
    }

    // If we reached here, there's a tie for the maximum value
    *pptr_result = NULL; // Clear pointer since it's a tie

    if (temp_radius == pressure_radius && temp_radius > humidity_radius) return 1; // Temp & Pressure tie
    if (temp_radius == humidity_radius && temp_radius > pressure_radius) return 2; // Temp & Humidity tie
    if (pressure_radius == humidity_radius && pressure_radius > temp_radius) return 3; // Pressure & Humidity tie

    return 5; // All three are equal and greater than 0
}

void printReport(SensorNode *temperature, SensorNode *pressure, SensorNode *humidity, SensorNode **pptr_result, int status)
{
    printf("==============================\n");
    printf("REPORT\n\n");
    printf("Raw Inputs: Temp = %.2f, Pressure = %.2f, Humidity = %.2f\n\n", temperature->raw, pressure->raw, humidity->raw);

    printf("Temp:       calibrated = %.2f, filtered = %.2f, valid = %d\n", temperature->calibrated, temperature->filtered, temperature->valid);
    printf("Pressure:   calibrated = %.2f, filtered = %.2f, valid = %d\n", pressure->calibrated, pressure->filtered, pressure->valid);
    printf("Humidity:   calibrated = %.2f, filtered = %.2f, valid = %d\n\n", humidity->calibrated, humidity->filtered, humidity->valid);

    printf("Most critical: ");

    if (status == 4 && *pptr_result != NULL)
    {
        if (*pptr_result == temperature) printf("Temperature Sensor\n");
        else if (*pptr_result == pressure) printf("Pressure Sensor\n");
        else if (*pptr_result == humidity) printf("Humidity Sensor\n");
    }
    else if (status == 1) printf("Temperature & Pressure Sensor (Tie)\n");
    else if (status == 2) printf("Temperature & Humidity Sensor (Tie)\n");
    else if (status == 3) printf("Pressure & Humidity Sensor (Tie)\n");
    else if (status == 5) printf("All Sensors are equally critical (Tie)\n");
    else printf("No valid critical sensor\n");
}

/*
REVIEW:
-   '=' is for assignment, '==' is for comparison.
-   fabsf() can be used as fabsf(temperature->filtered - midpoint); to get the absolute value.
-   Try to avoid magic numbers, use typedef enums to handle them (in this case its fine)
-   Learn how to avoid using floats, relying on fixed-point units instead. 
*/