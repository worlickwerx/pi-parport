#!/bin/sh -e

# generate pick and place file for JLCPCB assembly per
# https://jlcpcb.com/help/article/How-to-generate-the-BOM-and-Centroid-file-from-KiCAD

if test $# -ne 3; then
    echo "Usage $(basename $0) pcbfile posfile front|back" >&2
    exit 1
fi
pcbfile=$(realpath $1)
posfile=$2
side=$3


kicad-cli pcb export pos \
	--output=$posfile \
	--format=csv \
	--smd-only \
	--exclude-dnp \
	--units=mm \
	--side=$side \
	$pcbfile

sed --in-place \
	-e 's/Ref,/Designator,/' \
	-e 's/PosX,/Mid X,/' \
	-e 's/PosY,/Mid Y,/' \
	-e 's/Rot,/Rotation,/' \
	-e 's/Side$/Layer/' \
	$posfile
