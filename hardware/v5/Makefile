all:

check: check_erc check_drc

check_erc: hat.kicad_sch
	../scripts/run_erc.sh $<
check_drc: hat.kicad_pcb
	../scripts/run_drc.sh $<

clean:
	rm -f *.rpt
