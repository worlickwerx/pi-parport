### v4 hardware

v4 mainly corrects some minor flaws in the v3 design.  Bypass
capacitors are added, a few traces are rerouted to avoid passing under
resistors, and the footprints for the EEPROM resistors are changed
from 0603 to 0805.  Also, a few CAD-related inconsistencies were
fixed, like unclosed board outline and footprints not being consistent
between the schematic and board layout files.

[Schematic](v4/pi-parport.pdf) and [Bill of Materials](v4/BOM.md).

### v3 hardware

v3 adds an EEPROM to be a proper HAT.  It also converts the design files 
from Eagle to KiCad as a step to making additional board variations and 
modifications more accessible.  Unfortunately, bypass capacitors were 
left out of the redesign.

This board is shared on [OSHPARK](https://oshpark.com/shared_projects/gevwv2YP).
See also:  [Schematic](v3/pi-parport.pdf) and [Bill of Materials](v3/BOM.md).

### v2 hardware

v2 addresses the line termination and data direction limitations of v1,
using a single, purpose-built chip, the
[SN74LVC161284 19-bit bus interface with 3-state outputs](http://www.ti.com/product/SN74LVC161284).

This board was designed with Eagle, and is shared on
[OSHPARK](https://oshpark.com/shared_projects/Padn3qhP).
See also: [Schematic](v2/schematic.pdf) and [Bill of Materials](v2/BOM.md).

It works!  However, it doesn't conform to the HAT specification because
of the lack of EEPROM, and the IDC connector footprint is for a vertical
connector, but right angle would have helped the Pi fit into smaller cases.
(RA works, but it hangs over the board edge).

### v1 hardware

Three
[74AHCT541 octal buffers/drivers with 3-state outputs](http://www.ti.com/product/SN74AHCT541)
chips convert to/from the Pi's 3V3 line level and the parallel port's 5V.

This board was designed with Eagle.  See also: [Schematic](v1/schematic.pdf).

This version worked for my intended application (SBIG parallel port camera),
but due to missing line termination, only with a very short cable.  This
design also does not implement bidirectional DATA.
