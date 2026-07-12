/*
Day 8 Challenge: The Sensor Calibration System

Scenario:
You're writing a calibration routine for a dual-sensor node that measures temperature and pressure. 
Before the device goes into the field, each sensor needs to be calibrated — its raw output is adjusted by an offset value determined during calibration. 
The calibration routine also needs to validate the adjusted readings and report calibration status.
------------------------------------------------------------------------------------------------------------------------------------------
Task:
Define a struct called Sensor with three fields: raw (int), offset (int), and calibrated (int).
Then write 4 functions:

1.  A function that takes a pointer to a [single Sensor] and computes its calibrated value — sets sensor->calibrated = sensor->raw + sensor->offset. No return value needed.
2.  A function that takes two Sensor pointers and finds which has the higher calibrated value — stores the result by writing through a third pointer parameter (Sensor *result). 
    No return value. (findDominant)
3.  A function that validates a calibrated reading — temperature valid range: –40 to 125. Pressure valid range: 0 to 200. (validation)
    Takes the calibrated value, the min, the max, and a pointer to an int flag — sets the flag to 1 if valid, 0 if not.
4.  A function that prints a full status report for both sensors.
5.  In main(), declare two Sensor variables (one for temperature, one for pressure), 
    ask the user to input raw values and offsets for each, run calibration, validate both, find the dominant sensor, and print the report.

Constraints:
-   Practice pointer dereferencing (*ptr, ->) explicitly
-   Function 2 must use a pointer parameter to return a result rather than a return value
-   Function 3 must use a pointer parameter to set the flag
-   No global variables


Knowledge applied: Structs, Arrays, Pointers, Double pointers
*/

#include <stdio.h>

typedef struct Sensor
{
    int raw;
    int offset;
    int calibrated;    
} Sensor;


// Prototype Functions
void getInput(Sensor *temperature, Sensor *pressure);
void calibration(Sensor *reading);
void validation(const Sensor *reading, int min, int max, int *flag);
void findDominant(const Sensor *temperature, const Sensor *pressure, const Sensor **result, int temp_flag, int pressure_flag);
void report(const Sensor *temperature, const Sensor *pressure, const Sensor **result, int temp_flag, int pressure_flag);

int main()
{
    printf("SENSOR CALIBRATION SYSTEM\n");
    Sensor temperature;
    Sensor *ptr_temperature = &temperature; // Pointer to temperature variable

    Sensor pressure;
    Sensor *ptr_pressure = &pressure;

    const Sensor *ptr_result = NULL;
    const Sensor **pptr_result = &ptr_result; // Take the address of the pointer

    int temp_flag;
    int *ptr_tempflag = &temp_flag;

    int pressure_flag;
    int *ptr_pressureflag = &pressure_flag;
    
    
    getInput(ptr_temperature, ptr_pressure);

    calibration(ptr_temperature);
    calibration(ptr_pressure);

    validation(ptr_temperature, -40, 125, ptr_tempflag);
    validation(ptr_pressure, 0, 200, ptr_pressureflag);
    findDominant(ptr_temperature, ptr_pressure, pptr_result, temp_flag, pressure_flag);

    printf("==============================\n");
    printf("ANALYSIS:\n\n");
    report(ptr_temperature, ptr_pressure, pptr_result, temp_flag, pressure_flag);

    return 0;
}

void getInput(Sensor *temperature, Sensor *pressure)
{
    printf("==============================\n");
    printf("READING INPUT\n");
    printf("[NOTE]: Valid temperature range [(-40) - (125) celcius]\n");
    printf("[NOTE]: Valid pressure range [(0) - (200) Pa] \n\n");

    while(1)
    {
        int temp_check;
        printf("Please input raw temperature reading: ");
        if (scanf("%d", &temp_check) != 1)
        {
            printf("[ERROR] Input must be a valid number. Please try again\n\n");
            while (getchar() != '\n');
            continue;
        }
        else
        {
            temperature->raw = temp_check; 
            break;
        }
    }
    while(1)
    {
        int temp_offset_check;
        printf("Please input offset temperature reading: ");
        if (scanf("%d", &temp_offset_check) != 1)
        {
            printf("[ERROR] Input must be a valid number. Please try again\n\n");
            while (getchar() != '\n');
            continue;
        }
        else
        {
            temperature->offset = temp_offset_check; 
            break;
        }
    }
    while(1)
    {
        int pressure_check;
        printf("Please input raw pressure reading: ");
        if (scanf("%d", &pressure_check) != 1)
        {
            printf("[ERROR] Input must be a valid number. Please try again\n\n");
            while (getchar() != '\n');
            continue;
        }
        else
        {
            pressure->raw = pressure_check; 
            break;
        }
    }
    while(1)
    {
        int pressure_offset_check;
        printf("Please input offset pressure reading: ");
        if (scanf("%d", &pressure_offset_check) != 1)
        {
            printf("[ERROR] Input must be a valid number. Please try again\n\n");
            while (getchar() != '\n');
            continue;
        }
        else
        {
            pressure->offset = pressure_offset_check; 
            break;
        }
    }
}

void calibration(Sensor *reading)
{   
    reading->calibrated = reading->raw + reading->offset;
}

void validation(const Sensor *reading, int min, int max, int *flag)
{
    if (reading->calibrated < min || reading->calibrated > max)
    {
        *flag = 0;
    }
    else
    {
        *flag = 1;
    }
}

void findDominant(const Sensor *temperature, const Sensor *pressure, const Sensor **result, int temp_flag, int pressure_flag)
// const Sensor **result prevents the modification of the underlying struct data, but allows the modification of the address its pointing to
{

    if (temp_flag == 1 && pressure_flag == 1)
    {
        if (temperature->calibrated > pressure->calibrated)
        {
            *result = temperature; // result now points to the address of temperature sensor
        }
        else if (pressure->calibrated > temperature->calibrated)
        {
            *result = pressure;
        }
        else if (temperature->calibrated == pressure->calibrated)
        {
            *result = NULL;
        }
    }
    else if (temp_flag == 1 && pressure_flag == 0)
    {
        *result = temperature;
    }
    else if (temp_flag == 0 && pressure_flag == 1)
    {
        *result = pressure;
    }
    else
    {
        *result = NULL;
    }
}

void report(const Sensor *temperature, const Sensor *pressure, const Sensor **result, int temp_flag, int pressure_flag)
{
    printf("Temp raw: %d, offset: %d -> calibrated: %d ", temperature->raw, temperature->offset, temperature->calibrated);
    (temp_flag == 1) ? (printf("[VALID]\n")) : (printf("[INVALID]\n"));

    printf("Pressure raw: %d, offset: %d -> calibrated: %d ", pressure->raw, pressure->offset, pressure->calibrated);
    (pressure_flag == 1) ? (printf("[VALID]\n")) : (printf("[INVALID]\n"));

    printf("Dominant sensor: ");
    if (*result == temperature)
    {
        printf("Temperature");
    }
    else if (*result == pressure)
    {
        printf("Pressure");
    }
    else
    {   
        if (temperature->calibrated == pressure->calibrated)
        {
            printf("No dominant sensor found, both readings are equal.");
        }
        else
        {
            printf("No dominant sensor found, both readings are invalid.");
        }
    }
}

/*
REVIEW: WEATHER STATION LOGGER CODE
- Isolate inputs into separate while(1) loops to prevent out-of-order retries when a single multi-step input fails.
- Set pointer-to-pointer variables (e.g., ptr_result) as pointers to 'const' data from their creation if they point to read-only outputs.
- Match function declarations (prototypes) and definitions identically to prevent subtle parameter mismatch bugs.
- Pass pointers to 'const' structures in evaluation functions (validation, report) to prevent accidental side-effect mutations.
- Keep primitive pass-by-value flags (like temp_flag) mutable in signatures unless you explicitly intend to lock the local copy.
*/

/*
POINTER APPLICATION REVIEW
- Use single pointers (e.g., Sensor *reading) correctly to allow functions to directly modify the fields of structs allocated in main().
- Implement double pointers (e.g., const Sensor **result) accurately to modify where a pointer in main() looks from within a sub-function.
- Initialize tracking pointers to NULL immediately upon creation to prevent undefined behavior from wild pointer reads.
- Match address-of operators (&) to target pointers carefully to ensure the proper level of indirection is established.
- Keep pointer names descriptive (like ptr_temperature and pptr_result) to clearly convey the level of indirection to anyone reading the code.
*/


/* ============================================================================
 * C CONST POINTER & DOUBLE POINTER REFERENCE GUIDE
 * ============================================================================
 * Read pointer declarations from RIGHT to LEFT to easily understand them:
 * "int * const p" -> "p is a const pointer to an int"
 * ============================================================================ */

/* --- SINGLE POINTERS --- */

/* 
 * Type: Pointer to CONST Data
 * Syntax: const int *p;  (or: int const *p;)
 * Function: Protects the memory value. 
 * Can change 'p' (the address)?   YES -> p = &other_var;
 * Can change '*p' (the value)?    NO  -> *p = 5; // ❌ Compiler Error
 */

/* 
 * Type: CONST Pointer to Mutable Data
 * Syntax: int * const p = &var;
 * Function: Locks the pointer to one address permanently.
 * Can change 'p' (the address)?   NO  -> p = &other_var; // ❌ Compiler Error
 * Can change '*p' (the value)?    YES -> *p = 5;
 */

/* 
 * Type: CONST Pointer to CONST Data
 * Syntax: const int * const p = &var;
 * Function: Completely immutable. Both address and value are locked.
 * Can change 'p' (the address)?   NO  -> ❌ Compiler Error
 * Can change '*p' (the value)?    NO  -> ❌ Compiler Error
 */


/* --- DOUBLE POINTERS (Pointers to Pointers) --- */

/* 
 * Type: Pointer to a Pointer to CONST Data
 * Syntax: const int **pp;
 * Function: Commonly used for arrays of read-only data (like strings/records).
 * Can change 'pp' (top level)?    YES -> pp = &other_ptr;
 * Can change '*pp' (middle lvl)?  YES -> *pp = &other_var;
 * Can change '**pp' (deep data)?  NO  -> **pp = 5; // ❌ Compiler Error
 */

/* 
 * Type: Pointer to a CONST Pointer to Data
 * Syntax: int * const *pp;
 * Function: Locks the intermediate address, but the final data can be changed.
 * Can change 'pp' (top level)?    YES -> pp = &other_ptr;
 * Can change '*pp' (middle lvl)?  NO  -> *pp = &other_var; // ❌ Compiler Error
 * Can change '**pp' (deep data)?  YES -> **pp = 5;
 */

/* 
 * Type: CONST Pointer to a Pointer to Data
 * Syntax: int ** const pp;
 * Function: Locks the top-level pointer. It cannot look at another pointer.
 * Can change 'pp' (top level)?    NO  -> pp = &other_ptr; // ❌ Compiler Error
 * Can change '*pp' (middle lvl)?  YES -> *pp = &other_var;
 * Can change '**pp' (deep data)?  YES -> **pp = 5;
 */

/* 
 * Type: FULLY CONST Double Pointer
 * Syntax: const int * const * const pp;
 * Function: Absolute lockdown. Nothing in the entire chain can be modified.
 * Can change 'pp'?                NO  -> ❌ Compiler Error
 * Can change '*pp'?               NO  -> ❌ Compiler Error
 * Can change '**pp'?              NO  -> ❌ Compiler Error
 */


