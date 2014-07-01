#!/bin/sh

SPMC_SOC_KIT=spmc-soc-kit-beta

fpga_log_dir=$(pwd)

if [ -n "$SPARTANMC_ROOT" ] 
then
  rm $SPARTANMC_ROOT/spartanmc/lib_obj/src/fpga-log 2> /dev/null
  ln -s $fpga_log_dir/source/src $SPARTANMC_ROOT/spartanmc/lib_obj/src/fpga-log
  rm $SPARTANMC_ROOT/spartanmc/include/fpga-log
  ln -s $fpga_log_dir/source/include $SPARTANMC_ROOT/spartanmc/include/fpga-log
  
  ln -sf $fpga_log_dir/spartanmc/include/peripherals/* $SPARTANMC_ROOT/spartanmc/include/peripherals/
  ln -sf $fpga_log_dir/spartanmc/hardware/* $SPARTANMC_ROOT/spartanmc/hardware/
  
  ln -sf $fpga_log_dir/$SPMC_SOC_KIT/lib/targets/* $SPARTANMC_ROOT/lib/targets/
  ln -sf $fpga_log_dir/$SPMC_SOC_KIT/lib/devices/* $SPARTANMC_ROOT/lib/devices/
  
  if  ! grep -q "LIBS+=fpga-log" $SPARTANMC_ROOT/spartanmc/lib_obj/Makefile ;
  then
    sed -i '/^LIBS:=/a\LIBS+=fpga-log' $SPARTANMC_ROOT/spartanmc/lib_obj/Makefile
  fi
  if  ! grep -q "DEPS_fpga-log:=" $SPARTANMC_ROOT/spartanmc/lib_obj/Makefile ;
  then
    sed -i '/^DEPS_startup_loader:=/a\DEPS_fpga-log:=peri/uart_light_receive_nb.o peri/uart_light_send.o' $SPARTANMC_ROOT/spartanmc/lib_obj/Makefile
  fi
  
  if  ! grep -q "_CFLAGS+=-spmc-force-cc-branches=yes" $SPARTANMC_ROOT/spartanmc/lib_obj/objbuild.mk ;
  then #TODO compile only fpga-log library with this flag and not all
    sed -i '/^_CFLAGS+=/a\_CFLAGS+=-spmc-force-cc-branches=yes' $SPARTANMC_ROOT/spartanmc/lib_obj/objbuild.mk
  fi
else
  echo "error: variable \$SPARTANMC_ROOT not set! Please check your SpartanMC installation."
fi