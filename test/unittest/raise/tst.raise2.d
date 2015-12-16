/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/* @@trigger: raise-tst-raise2 */
/* @@trigger-timing: after */
/* @@runtest-opts: $_pid */

/*
 * Copyright 2006 Oracle, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#pragma ident	"%Z%%M%	%I%	%E% SMI"

/*
 * ASSERTION:
 * 	Positive test for raise
 *
 * SECTION: Actions and Subroutines/raise()
 */

#pragma D option destructive

BEGIN
{
	/*
	 * Wait no more than five seconds for the process to call ioctl().
	 */
	timeout = timestamp + 5000000000;
}

syscall::ioctl:return
/pid == $1/
{
	trace("raised");
	raise(SIGINT);
	/*
	 * Wait no more than three seconds for the process to die.
	 */
	timeout = timestamp + 3000000000;
}

syscall::exit_group:entry
/pid == $1/
{
	exit(0);
}

profile:::tick-4
/timestamp > timeout/
{
	trace("timed out");
	exit(1);
}
