This repository contains some work on porting parts of Yamagi's Quake II to PS3

Notable parts:
 - `backends/ps3/system.c`
 - `backends/ps3/network_loopback.c` - platform independent loopback placeholder
 - `client/input/input.c` - SDL-less PS3's pad handling
 - `source/common/shared/shared.c` - line 1184: precooking 'in folder' paths for fopen
