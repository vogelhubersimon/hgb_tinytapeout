![](../../workflows/gds/badge.svg) ![](../../workflows/docs/badge.svg) ![](../../workflows/test/badge.svg)
# HGB TinyTapeout

## Overview

This is the repository for the HGB TinyTapeout project. The goal of this project is to create a custom IC using the [TinyTapeout](https://tinytapeout.com/) Project. 

## Installation

All the following were tested on Ubuntu. Try at your own risk :)


[**OSS CAD Suite**](https://github.com/YosysHQ/oss-cad-suite-build)  
This is needed for Synthesis. Download the latest release and add the `bin` folder to your PATH. This suite includes Yosys(Open Synthesis tool) and a special build of GHDL(Yosys plugin for VHDL support).

**Github Actions**  
This repository uses github actions to run tests, generate documentation and build the gds file (The file that is used by the foundry to produce the chip).

You can create your own branch and push changes to it. The actions will run automatically. If the actions pass you can look at the generated gds file in the artifacts of the gds build action.

**Building VHDL files**  
run `make` inside the `src` folder to convert VHDL files to Verilog files. Its important to use the toplevel [entity](src/vhdl/project.vhd) provided in the Makefile (the external pin layout is specified by TinyTapeout).

*This process of converting vhdl into verilog has not been tested thoroughly.*

If you add source files please make sure to update the [`info.yaml`](info.yaml).

## Contributing

If you are part of the hgb_tinytapout team you can create you own branch and try out the process. After pushing your changes the github actions will run automatically (pls do not push into main as of right now).
