### Makefile for SpartanMC build process ###
#
# Complete system including firmware
#

### Flags for tools ###
#
# jConfig v2.*
JCFLAGS:=

#################################
### NOTHING TO CUSTOMIZE HERE ###
#################################

ifeq ($(SPARTANMC_ROOT),)
  $(error SPARTANMC_ROOT is not set. This variable should point to your SpartanMC-SoC-Kit directory.)
endif
export SPARTANMC_ROOT

include $(SPARTANMC_ROOT)/src/scripts/make/project/main.mk

$(SPARTANMC_ROOT)/src/scripts/make/project/main.mk:
	@printf "\n*** Please check your setting of variable SPARTANMC_ROOT! The current value of '$(SPARTANMC_ROOT)' seems not to be valid.\n\n"
	@false
