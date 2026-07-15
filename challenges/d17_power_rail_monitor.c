/*
Day 17: The Power Rail Monitor

Scenario:
In green power systems — solar inverters, BMS controllers, MPPT chargers — multiple power rails supply different subsystems at different voltages.
A dedicated monitoring circuit watches each rail and packs the status of all rails into a single 32-bit status register for the main controller to read.
Your firmware must decode this packed register, extract per-rail status fields, and make system-level decisions from the result.
This is a step up from Day 16 — instead of single-bit flags, you're now working with multi-bit fields packed into a single register.
This is how real power management ICs like the Texas Instruments TPS65xx family report rail status.

The Register Layout
You are working with a 32-bit Power Rail Status Register (uint32_t). It is divided into four 8-bit fields, one per power rail:
Bits        Rail        Description
7:0         RAIL_A      3.3V logic rail
15:8        RAIL_B      5.0V peripheral rail
23:16       RAIL_C      12V gate drive rail
31:24       RAIL_D      48V main power rail

Each 8-bit rail field is itself divided into sub-fields:
Bits within field   Name            Width       Description
1:0                 STATUS          2 bits      00=OK, 01=WARN, 10=FAULT, 11=CRITICAL
3:2                 CURRENT_LOAD    2 bits      00=IDLE, 01=LOW, 10=MED, 11=HIGH
5:4                 VOLTAGE_BAND    2 bits      00=NOMINAL, 01=HIGH, 10=LOW, 11=INVALID
7:6                 RESERVED        2 bits      Must always be 00

Your Task
Define a struct called RailStatus with these fields:
-   uint8_t status — 2-bit STATUS field (0–3)
-   uint8_t current_load — 2-bit CURRENT_LOAD field (0–3)
-   uint8_t voltage_band — 2-bit VOLTAGE_BAND field (0–3)

Define a struct called PowerRailRegister with:
-   uint32_t rail_register — the full 32-bit packed register
-   RailStatus rails[4] — decoded status for each of the 4 rails

Then write 5 functions:

1.  A function that packs a single rail's fields into its 8-bit slot — takes a RailStatus pointer,
    validates that RESERVED bits are never set, builds the 8-bit value by combining STATUS, CURRENT_LOAD, and VOLTAGE_BAND using shifts and OR,
    then writes it into the correct 8-bit position of rail_register using a shift determined by the rail index (0–3).

2.  A function that extracts a single rail's fields from the register — takes a PowerRailRegister pointer and a rail index (0–3).
    Isolates the correct 8-bit field using a mask and shift, then extracts each 2-bit sub-field into the corresponding RailStatus field.

3.  A function that scans all 4 rails and returns the highest severity STATUS found — takes a const PowerRailRegister pointer and
    writes the result through a uint8_t * output parameter. Priority: CRITICAL(3) > FAULT(2) > WARN(1) > OK(0).

4.  A function that prints the full register in binary — prints the 32-bit register MSB-first with a space separator between each 8-bit group for readability.

5.  A function that prints the decoded report — for each rail prints its STATUS, CURRENT_LOAD, and VOLTAGE_BAND as human-readable labels,
    then prints the system-level worst status.

In main(), ask the user to enter the STATUS, CURRENT_LOAD, and VOLTAGE_BAND for each of the 4 rails (validate all inputs 0–3 and reject RESERVED combinations),
pack them into the register, extract them back out, find the worst status, and print the full report.

Field Labels for Printing:
STATUS: OK, WARN, FAULT, CRITICAL
CURRENT_LOAD: IDLE, LOW, MED, HIGH
VOLTAGE_BAND: NOMINAL, HIGH, LOW, INVALID

Core C Concepts Being Tested:
-   Multi-bit field extraction using masks and shifts
-   Bit field packing — writing multi-bit values into a register
-   Building and applying masks for fields wider than 1 bit
-   Combining bit manipulation with struct-based register modeling
-   Fixed-width types throughout
*/