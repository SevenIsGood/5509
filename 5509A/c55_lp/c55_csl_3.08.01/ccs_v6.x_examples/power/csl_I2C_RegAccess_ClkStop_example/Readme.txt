Example to verify I2C r/w recovery after clock-gating

POWER - I2C REG R/W RECOVERY AFTER CLOCK GATING

TEST DESCRIPTION:
	This test starts reading default after-reset values in the
I2C register space and validates them to be as expected by the 
datasheet. In between the clock to the I2C peripheral is shut off 
and restarted. The I2C register space r/w check is resumed. The 
r/w shouldn't be affected by the shutdown-restart activity on the
clock to the I2C module.

TEST PROCEDURE:

1. Configure the clock to the I2C module.
2. Start reading the default after-reset values of the I2C registers.
3. Compare them with the expected values as mentioned in the datasheet.
4. Shut down the clock to I2C peripheral by setting the corresponding bit in PCGCR1.
5. After a small delay, restart the clock by clearing the same bit.
6. Resume reg r/w and ensure there are no mismatches.

TEST RESULT:
  All the CSL APIs should return success.
  Reg r/w shouldn't throw any mismatch.