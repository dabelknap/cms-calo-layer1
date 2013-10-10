Uploading Code to the oRSC
-----------------

```shell
xmd
XMD%
XMD% connect mb mdm     # Connect to Microblaze with Microblaze Debugger
XMD% dow payload.elf    # upload the payload.elf file
XMD% run                # starts the processor
XMD% read_uart          # Reads from "std" out
XMD% stop               # stop the processor
XMD% rst                # reset the processor
```

For running on the oRSC front-end
```shell
fpga -f /afs/hep/ecad/mathias/orsc_bits/top_fe.bit -debugdevice deviceNr 2
fpga_isconfigured -debugDevice deviceNr 2
connect mb mdm -debugdevice deviceNr 2
dow payload.elf
```
