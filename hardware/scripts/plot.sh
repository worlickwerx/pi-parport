#!/bin/bash

# Generate gerbers for JLCPCB per
# https://jlcpcb.com/help/article/362-how-to-generate-gerber-and-drill-files-in-kicad-7

die() {
    echo $* >&2
    exit 1
}

if test $# -ne 2; then
    echo "Usage $(basename $0) pcbfile zipfile" >&2
    exit 1
fi
pcbfile=$(realpath $1)
zipfile=$2

tmpdir=$(mktemp -d) || die "could not create output directory"

pushd $tmpdir
    kicad-cli pcb export gerbers \
	--layers=F.Cu,B.Cu,F.Paste,B.Paste,F.Silkscreen,B.Silkscreen,F.Mask,B.Mask,Edge.Cuts,F.Fab,B.Fab \
	--subtract-soldermask \
	--no-x2 \
	$pcbfile
    test $? -eq 0 || die "could not export gerber files"
    rm -f *.gbrjob

    kicad-cli pcb export drill \
	--format=excellon \
	--excellon-separate-th \
	--excellon-oval-format=alternate \
	--map-format=ps \
	--drill-origin=absolute \
	--excellon-units=mm \
	--excellon-zeros-format=decimal \
	$pcbfile
    test $? -eq 0 || die "could not export drill files"
popd

echo Generating $zipfile
find $tmpdir -type f -print | zip --quiet --junk-paths $zipfile -@
test $? -eq 0 || die "error creating zip archive"
rm -rf $tmpdir
