/*--------------------------------------------------------------*/
/* hexutil.cmd                                                  */
/* Version 1.0                                                  */
/* 16-Jan-2013                                                  */
/*--------------------------------------------------------------*/

/*  
    The name of this command file, the path to the input file, and the
    name of the map should be specified on the command line as shown below:

        hex55 hexutil.cmd .\Debug\Test.bin -map Test.mxp
    
    The output file will have the same base filename as the input file
    and it will have a ".m00" extension. 
*/
-b                  /*  Binary */
-serial8            /*  Boot mode: serial from 8-bit data
                        memory on EMIF. */

-boot               /*  Generate boot table for use by bootloader. */
-v5505              /*  Use "5535" format for boot table. */
