all:

cam.zip: hat.kicad_pcb
	../scripts/plot.sh $< $@

bom.csv: hat.kicad_sch
	../scripts/make_bom.sh $< $@
pos_top.csv: hat.kicad_pcb
	../scripts/make_pos.sh $< $@ front

check: check_erc check_drc

check_erc: hat.kicad_sch
	../scripts/run_erc.sh $<
check_drc: hat.kicad_pcb
	../scripts/run_drc.sh $<

clean:
	rm -f *.rpt
	rm -f bom.csv
	rm -f pos_top.csv
	rm -f cam.zip
