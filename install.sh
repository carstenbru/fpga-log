#!/bin/sh

SPMC_SOC_KIT=spmc-soc-kit-online
LIB_DEPS="peri/uart_light_receive_nb.o peri/uart_light_send.o peri/spi_activate.o peri/spi_deactivate.o peri/spi_set_div.o peri/spi_enable.o peri/spi_set_cpha.o peri/spi_write.o"
CONFIG_TOOL_BUILD_DIR=build-config-tool-Desktop-Release

fpga_log_dir=$(pwd)

if [ -n "$SPARTANMC_ROOT" ] 
then
  #link fpga-log source into spmc library folders
  rm $SPARTANMC_ROOT/spartanmc/lib_obj/src/fpga-log 2> /dev/null
  ln -s $fpga_log_dir/source/src $SPARTANMC_ROOT/spartanmc/lib_obj/src/fpga-log
  rm $SPARTANMC_ROOT/spartanmc/include/fpga-log 2> /dev/null
  ln -s $fpga_log_dir/source/include $SPARTANMC_ROOT/spartanmc/include/fpga-log
  
  #remove uart_light and timer_compare, they will be replaced with patched versions later
  rm -rf $SPARTANMC_ROOT/spartanmc/hardware/uart_light
  rm -rf $SPARTANMC_ROOT/spartanmc/hardware/timer_compare
  #remove older versions of fpga-log's hardware components
  rm -rf $SPARTANMC_ROOT/spartanmc/hardware/clk_div
  rm -rf $SPARTANMC_ROOT/spartanmc/hardware/pwm
  rm -rf $SPARTANMC_ROOT/spartanmc/hardware/sdcard
  rm -rf $SPARTANMC_ROOT/spartanmc/hardware/timestamp_gen
  rm -rf $SPARTANMC_ROOT/spartanmc/hardware/motor_dummy_control
  
  #link new peripherals
  ln -sf $fpga_log_dir/spartanmc/include/peripherals/* $SPARTANMC_ROOT/spartanmc/include/peripherals/
  #ln -sf $fpga_log_dir/spartanmc/hardware/* $SPARTANMC_ROOT/spartanmc/hardware/
  #new JConfig version has problems with links, so copy the files..
  cp -R $fpga_log_dir/spartanmc/hardware/* $SPARTANMC_ROOT/spartanmc/hardware/
  
  #link new targets/devices
  ln -sf $fpga_log_dir/$SPMC_SOC_KIT/lib/targets/* $SPARTANMC_ROOT/lib/targets/
  ln -sf $fpga_log_dir/$SPMC_SOC_KIT/lib/devices/* $SPARTANMC_ROOT/lib/devices/
  
  #add fpga-log library to build system
  if  ! grep -q "LIBS+=fpga-log" $SPARTANMC_ROOT/spartanmc/lib_obj/Makefile ;
  then
    sed -i '/^LIBS:=/a\LIBS+=fpga-log' $SPARTANMC_ROOT/spartanmc/lib_obj/Makefile
  fi
  if  ! grep -q "DEPS_fpga-log:=" $SPARTANMC_ROOT/spartanmc/lib_obj/Makefile ;
  then
    sed -i "/^DEPS_startup_loader:=/a\DEPS_fpga-log:=$LIB_DEPS" $SPARTANMC_ROOT/spartanmc/lib_obj/Makefile
  else
    sed -i "/^DEPS_fpga-log:=/c\DEPS_fpga-log:=$LIB_DEPS" $SPARTANMC_ROOT/spartanmc/lib_obj/Makefile
  fi
  
  #add spmc-force-cc-branches flag for library build
  if  ! grep -q "_CFLAGS+=-spmc-force-cc-branches=yes" $SPARTANMC_ROOT/spartanmc/lib_obj/objbuild.mk ;
  then #TODO compile only fpga-log library with this flag and not all
    sed -i '/^_CFLAGS+=/a\_CFLAGS+=-spmc-force-cc-branches=yes' $SPARTANMC_ROOT/spartanmc/lib_obj/objbuild.mk #for older spmc versions
    sed -i '/^_CFLAGS_$(D_BUILD):=/a\_CFLAGS_$(D_BUILD)+=-spmc-force-cc-branches=yes' $SPARTANMC_ROOT/spartanmc/lib_obj/objbuild.mk #for newer spmc versions
    
  fi
  
  #compile library
  cd $SPARTANMC_ROOT/spartanmc/lib_obj/
  #make
  
  #build config-tool
  mkdir $fpga_log_dir/$CONFIG_TOOL_BUILD_DIR
  cd $fpga_log_dir/$CONFIG_TOOL_BUILD_DIR
  qmake ../config-tool/config-tool.pro
  make
else
  echo "error: variable \$SPARTANMC_ROOT not set! Please check your SpartanMC installation."
fi