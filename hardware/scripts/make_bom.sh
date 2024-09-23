#!/bin/sh

# Generate bill of materials for JLCPCB assembly per
# https://jlcpcb.com/help/article/How-to-generate-the-BOM-and-Centroid-file-from-KiCAD

if test $# -ne 2; then
    echo "Usage $(basename $0) schfile bomfile" >&2
    exit 1
fi
schfile=$(realpath $1)
bomfile=$2

kicad-cli sch export bom \
       --output=$bomfile \
       --fields='Comment,Reference,Footprint,LCSC,Mouser' \
       --labels='Comment,Designator,Footprint,LCSC,Mouser' \
       --string-delimiter= \
       --sort-field=Reference \
       --group-by=Comment \
       --ref-delimiter=' ' \
       --ref-range-delimiter= \
       --exclude-dnp \
       $schfile
