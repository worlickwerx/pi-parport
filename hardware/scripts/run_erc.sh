#!/bin/bash

kicad-cli sch erc \
    -o sch_erc.rpt \
    --exit-code-violations \
    --severity-all \
    $ERC_INPUT
result=$?
test $result -eq 0 || cat sch_erc.rpt >&2
exit $result
