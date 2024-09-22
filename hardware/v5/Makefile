all:

check: check_erc check_drc

check_erc: hat.kicad_sch
	ERC_INPUT=$< ../scripts/run_erc.sh
check_drc: hat.kicad_pcb
	DRC_INPUT=$< ../scripts/run_drc.sh
