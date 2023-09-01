This is a example Secondary Bootloader (SBL) for C5517 from McSPI flash. 

1. Build this project to generate the sbl.out
2. Use the build_boot.bat to generate the sbl.bin
3. Use the programmer to write sbl.bin into the McSPI flash at address 0x00000000
4. Build the application code subject for bootloading using SBL to get app.out
5. Generate the app.bin from app.out using hex55.exe
6. Use the programer (uncomment the ///#define FLASH_START_ADDRESS 0x00004000 and rebuild the programmer) to write app.bin into the McSPI flash at address 0x00004000
7. On power up reset, the ROM bootloader will boot it from McSPI flash
8. The SBL will in turn to load the app.bin, and then give control to the app.bin
