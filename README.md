# GPS_data_extractor
GPS data extractor tool for 70mai DashCAM clips

This tool is based on freezer52000's work and I made it for educating/training myself in C.

The extractor works only with 70mai DashCAM clips, however other recordings weren't tested. This dashcam records the GPS data in binary format embedded in the clips. This tool extracts that data to 3 different outputs. The C source supports a single file, yet.

I found faster/easier to write a shell script for multiple files, so I decided not to implement into the C source that function, yet (so I can focus on other stuffs).

dashGPSext = single file
dashGPSextMult = multiple file

Usage for single file: 

dashGPSext -arg file.movie
  -c : output in CSV
  -g : output in SRT
  -s : output to stdout
  
Usage for multiple files:

It only supports CSV format as output format!

Cd into the folder which contains the dashcam clips and run the dashGPSextMult.sh shell script.
