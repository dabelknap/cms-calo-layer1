# oRSC backend SPI test

HW=../orsc_be_hw_spi
BSP=../orsc_be_bsp_spi

IPBUS=$(SOFTIPBUS)

# In JTAG chain, needed for make upload
DEVICENR=1

INCLUDES=-I$(IPBUS)/include -I../spistream -I../tracer

# Make a single megalibrary of all user code.
SRCS=$(wildcard src/*.c) $(wildcard ../spistream/*.c) \
     $(wildcard $(SOFTIPBUS)/src/*buffer*.c) \
     ../tracer/tracer.c

include ../Makefile

# No logging
LOG_LEVEL=0
DEBUG=-g3
OPT=-O2
