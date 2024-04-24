#!/usr/bin/tclsh

# Main proc at the end #

#------------------------------------------------------------------------------
proc vhdl_compil { } {
  do compile.do
  global Path_VHDL
  global Path_TB
  puts "\nVHDL compilation :"

  vcom -2008 $Path_VHDL/neuron_pkg.vhd
  vcom -2008 $Path_VHDL/neuron_combinational.vhd
  vcom -2008 $Path_VHDL/neuron_sequential.vhd
  vcom -2008 $Path_VHDL/neuron_pipeline.vhd

  source $Path_TB/../OsvvmLibraries/Scripts/StartUp.tcl
  if {[file exists VHDL_LIBS] == 0} {
    build  $Path_TB/../OsvvmLibraries
  }

  vcom -2008 $Path_TB/html_report_pkg.vhd
  vcom -2008 $Path_TB/logger_html_pkg.vhd
  vcom -2008 $Path_TB/project_logger_pkg.vhd
  vcom -2008 $Path_TB/neuron_tb_pkg.vhd
  vcom -2008 $Path_TB/neuron_bfm_pkg.vhd
  vcom -2008 $Path_TB/test_neuron_pkg.vhd
  vcom -2008 $Path_TB/neuron_tb.vhd
}

#------------------------------------------------------------------------------
proc sim_start {DATASIZE NBINPUTS DUV TESTCASE COMMA_POS} {
  puts "\nStart simulation :"
  vsim -t 1ns -GDATASIZE=$DATASIZE -GNBINPUTS=$NBINPUTS -GDUV=$DUV -GTESTCASE=$TESTCASE -GCOMMA_POS=$COMMA_POS work.neuron_tb
  add wave -r *
  wave refresh
  run -all
}

#------------------------------------------------------------------------------
proc do_all {DATASIZE NBINPUTS DUV TESTCASE COMMA_POS} {
  vhdl_compil
  sim_start $DATASIZE $NBINPUTS $DUV $TESTCASE $COMMA_POS
}

#------------------------------------------------------------------------------
proc help { } {
    puts "Call this script with one of the following options:"
    puts "    all         : compiles and run, with 5 arguments (see below)"
    puts "    comp_vhdl   : compiles all the VHDL files"
    puts "    sim         : starts a simulation, with 5 arguments (see below)"
    puts "    help        : prints this help"
    puts "    no argument : compiles and run with DATASIZE=8, NBINPUTS= 16, DUV=combinational, TESTCASE=0, ERRNO=0"
    puts ""
    puts "When 5 arguments are required, the order is:"
    puts "    1: DATASIZE, the size of data"
    puts "    2: NBINPUTS, the number of inputs"
    puts "    3: 0 -> combinational design, 1 -> sequential design, 2 -> pipelined design"
    puts "    4: The test to run, 0 -> run all the tests"
    puts "    5: Position of the comma in the inputs"
}

## MAIN #######################################################################

# Compile folder ----------------------------------------------------
if {[file exists work] == 0} {
  mkdir work
  vlib work
  vmap work work
}

quietly set Path_VHDL     "../src_vhdl"
quietly set Path_TB       "../src_tb"

global Path_VHDL
global Path_TB

# start of sequence -------------------------------------------------

if {$argc>0} {
  if {[string compare $1 "all"] == 0} {
    do_all $2 $3 $4 $5 $6
  } elseif {[string compare $1 "comp_vhdl"] == 0} {
    vhdl_compil
  } elseif {[string compare $1 "sim"] == 0} {
    sim_start $2 $3 $4 $5 $6
  } elseif {[string compare $1 "help"] == 0} {
    help
  }
} else {
  do_all 8 16 0 0 0
}