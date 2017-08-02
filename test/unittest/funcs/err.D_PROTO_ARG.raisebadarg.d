/*
 * Oracle Linux DTrace.
 * Copyright © 2006, Oracle and/or its affiliates. All rights reserved.
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

/*
 * ASSERTION:
 *  raise() should generate an error if any arguments are sent.
 *
 * SECTION: Actions and Subroutines/raise()
 *
 */


BEGIN
{
	raise("badarg");
	exit(0);
}

