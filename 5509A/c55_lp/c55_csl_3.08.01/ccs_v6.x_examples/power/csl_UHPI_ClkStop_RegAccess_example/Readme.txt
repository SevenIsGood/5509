Example to verify UHPI r/w recovery after clock-gating

POWER - UHPI REG R/W RECOVERY AFTER CLOCK GATING

TEST DESCRIPTION:
	This test starts reading default after-reset values in the
UHPI register space and validates them to be as expected by the 
datasheet. In between the clock to the UHPI peripheral is shut off 
and restarted. The UHPI register space r/w check is resumed. The 
r/w shouldn't be affected by the shutdown-restart activity on the
clock to the UHPI module.

TEST PROCEDURE:

1. Configure the clock to the UHPI module.
2. Start reading the default after-reset values of the UHPI registers.
3. Compare them with the expected values as mentioned in the datasheet.
4. Request for gating UHPI clock by setting the corresponding bit in 
   CLKSTOP1 and wait till ack is received in corresponding bit, again
   in CLKSTOP1.
5. Shut down the clock to UHPI peripheral by setting the corresponding
   bit in PCGCR1.
6. After a small delay, restart the clock by clearing the same bit.
7. Resume reg r/w and ensure there are no mismatches.

TEST RESULT:
  All the CSL APIs should return success.
  Reg r/w shouldn't throw any mismatch.
