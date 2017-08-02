/*
 * Oracle Linux DTrace.
 * Copyright © 2006, Oracle and/or its affiliates. All rights reserved.
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */


/*
 * ASSERTION:
 *	Positive test to make sure that we can invoke sparc
 *	ureg[] aliases.
 *
 * SECTION: User Process Tracing/uregs Array
 *
 * NOTES: This test does no verification - the value of the output
 *	is not deterministic.
 */

#pragma D option quiet

BEGIN
{
	printf("R_G0 = 0x%x\n", uregs[R_G0]);
	printf("R_G1 = 0x%x\n", uregs[R_G1]);
	printf("R_G2 = 0x%x\n", uregs[R_G2]);
	printf("R_G3 = 0x%x\n", uregs[R_G3]);
	printf("R_G4 = 0x%x\n", uregs[R_G4]);
	printf("R_G5 = 0x%x\n", uregs[R_G5]);
	printf("R_G6 = 0x%x\n", uregs[R_G6]);
	printf("R_G7 = 0x%x\n", uregs[R_G7]);
	printf("R_O0 = 0x%x\n", uregs[R_O0]);
	printf("R_O1 = 0x%x\n", uregs[R_O1]);
	printf("R_O2 = 0x%x\n", uregs[R_O2]);
	printf("R_O3 = 0x%x\n", uregs[R_O3]);
	printf("R_O4 = 0x%x\n", uregs[R_O4]);
	printf("R_O5 = 0x%x\n", uregs[R_O5]);
	printf("R_O6 = 0x%x\n", uregs[R_O6]);
	printf("R_O7 = 0x%x\n", uregs[R_O7]);
	printf("R_L0 = 0x%x\n", uregs[R_L0]);
	printf("R_L1 = 0x%x\n", uregs[R_L1]);
	printf("R_L2 = 0x%x\n", uregs[R_L2]);
	printf("R_L3 = 0x%x\n", uregs[R_L3]);
	printf("R_L4 = 0x%x\n", uregs[R_L4]);
	printf("R_L5 = 0x%x\n", uregs[R_L5]);
	printf("R_L6 = 0x%x\n", uregs[R_L6]);
	printf("R_L7 = 0x%x\n", uregs[R_L7]);
	printf("R_I0 = 0x%x\n", uregs[R_I0]);
	printf("R_I1 = 0x%x\n", uregs[R_I1]);
	printf("R_I2 = 0x%x\n", uregs[R_I2]);
	printf("R_I3 = 0x%x\n", uregs[R_I3]);
	printf("R_I4 = 0x%x\n", uregs[R_I4]);
	printf("R_I5 = 0x%x\n", uregs[R_I5]);
	printf("R_I6 = 0x%x\n", uregs[R_I6]);
	printf("R_I7 = 0x%x\n", uregs[R_I7]);
	printf("R_CCR = 0x%x\n", uregs[R_CCR]);
	printf("R_PC = 0x%x\n", uregs[R_PC]);
	printf("R_NPC = 0x%x\n", uregs[R_NPC]);
	printf("R_Y = 0x%x\n", uregs[R_Y]);
	printf("R_ASI = 0x%x\n", uregs[R_ASI]);
	printf("R_FPRS = 0x%x\n", uregs[R_FPRS]);
	exit(0);
}

ERROR
{
	printf("uregs access failed.\n");
	exit(1);
}
