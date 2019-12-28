## Testing

These tests primarily check that the bits aren't shorted, open,
or misrouted, and the the DTS overlay is properly configured.

### loop - plug A

`loop A` utilizes a DB25 loopback plug wired as below.

| Pin | Name     | Pin | Name     |
| --- | -------- | --- | -------- |
| 3   | DATA1    | 14  | CONTROL1 |
| 4   | DATA2    | 15  | STATUS3  |
| 5   | DATA3    | 17  | CONTROL3 |
| 6   | DATA4    | 13  | STATUS4  |
| 7   | DATA5    | 12  | STATUS5  |
| 8   | DATA6    | 10  | STATUS6  |
| 9   | DATA7    | 11  | STATUS7  |

**phase 1** DATA is configured for output.
Each of 256 possible output values is placed in the DATA register,
then the STATUS register is read.  The bits from DATA[2,4:7] must appear
on STATUS[3:7].

**phase 2** DATA is configured for input.
Each of 16 possible control values is placed in the CONTROL register,
then the DATA register is read.  The bits from CONTROL[1,3] must appear
on DATA[1,3].

### loop - plug B

`loop B` utilizes a DB25 loopback plug wired as below.
It covers the pins missed by plug A.

| Pin | Name     | Pin | Name     |
| --- | -------- | --- | -------- |
| 2   | DATA0    | 1   | CONTROL0 |
| 4   | DATA2    | 16  | CONTROL2 |

DATA is configured for input.
Each of 16 possible control values is placed in the CONTROL register,
then the DATA register is read.  The bits from CONTROL[0,2] must appear
on DATA[0,2].

### strobe

`strobe pin [period]` simply toggles a selected pin at a target period
so it can be scoped.  This is useful for debugging if the loop tests fail.
