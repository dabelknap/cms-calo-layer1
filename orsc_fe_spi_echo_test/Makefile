# oRSC front SPI test

HW=../orsc_fe_hw_spi
BSP=../orsc_fe_bsp_spi

IPBUS=$(SOFTIPBUS)

# In JTAG chain, needed for make upload
DEVICENR=2

INCLUDES=-I$(IPBUS)/include -I../spistream -I../tracer

LAYER1_DIR=..

# Make a single megalibrary of all user code.
SRCS=$(wildcard src/*.c) $(wildcard ../spistream/*.c) \
     $(wildcard $(SOFTIPBUS)/src/*buffer*.c) \
     ../tracer/tracer.c

include ../Makefile

LOG_LEVEL=0x0
DEBUG=-g3
OPT=-O2
