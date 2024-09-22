#!/bin/bash

kicad-cli pcb drc \
    -o pcb_drc.rpt \
    --exit-code-violations \
    --severity-all \
    $DRC_INPUT
result=$?
test $result -eq 0 || cat pcb_drc.rpt >&2
exit $result
