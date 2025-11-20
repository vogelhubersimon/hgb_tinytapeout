# generate verilog from VHDL using yosys and ghdl
# yosys -m ghdl -p 'ghdl project.vhd -e tt_um_example; write_verilog project.v'

SRC = $(wildcard vhdl/*.vhd)
TOP = tt_um_example
OUT = $(TOP).v

all: $(OUT)
$(OUT): $(SRC)
	yosys -m ghdl -p 'ghdl $(SRC) -e $(TOP); write_verilog $@'
clean:
	rm -f $(OUT) *.cf 
.PHONY: all clean
