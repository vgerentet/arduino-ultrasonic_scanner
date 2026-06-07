# arduino-ultrasonic_scanner
This project is a 2D ultrasonic rotating scanner capable of measuring distances, reconstructing the shape of a room from angular scans, and exporting data for MATLAB visualization. This project was done using an Arduino card, programmed in embedded C++

The project is composed of the following files : 
- Scanner/Scanner.ino : Embedded C++ code oppenned with Ardino IDE software.
- libraires/... : Embedded code headders and libraries
- Scanner2D.m : data acquisition using Matlab, exported in 'scan.csv' file.
- dataplot/m : data processing and plotting
