/*
 * Oracle Linux DTrace.
 * Copyright © 2006, Oracle and/or its affiliates. All rights reserved.
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#pragma D option nspec=1

BEGIN,
BEGIN,
BEGIN,
BEGIN
{
	spec = speculation();
}

BEGIN
{
	exit(0);
}
