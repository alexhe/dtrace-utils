/*
 * Oracle Linux DTrace.
 * Copyright © 2006, Oracle and/or its affiliates. All rights reserved.
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

/*
 * ASSERTION:
 *  Test printf() with a bad aggregation format.
 *
 * SECTION: Output Formatting/printf()
 *
 */

BEGIN
{
	printf("hello %@d", 123);
}
