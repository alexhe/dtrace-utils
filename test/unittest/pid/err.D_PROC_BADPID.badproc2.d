/*
 * Oracle Linux DTrace.
 * Copyright © 2006, Oracle and/or its affiliates. All rights reserved.
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

/* @@no-xfail */

/*
 * ASSERTION: Make sure we can't grab pid 0
 *
 * SECTION: User Process Tracing/pid Provider
 *
 */

pid0:::entry
{
}
