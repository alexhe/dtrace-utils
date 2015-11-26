#!/bin/bash
#
# CDDL HEADER START
#
# The contents of this file are subject to the terms of the
# Common Development and Distribution License (the "License").
# You may not use this file except in compliance with the License.
#
# You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
# or http://www.opensolaris.org/os/licensing.
# See the License for the specific language governing permissions
# and limitations under the License.
#
# When distributing Covered Code, include this CDDL HEADER in each
# file and include the License file at usr/src/OPENSOLARIS.LICENSE.
# If applicable, add the following below this CDDL HEADER, with the
# fields enclosed by brackets "[]" replaced with your own identifying
# information: Portions Copyright [yyyy] [name of copyright owner]
#
# CDDL HEADER END
#

#
# Copyright 2014 Oracle, Inc.  All rights reserved.
# Use is subject to license terms.

#
# This script tests that dropping a breakpoint on a process that is constantly
# exec()ing works, that the breakpoint is hit, and that the exec() is detected.
#

# Some machines have very slow exec(), and this exec()s five thousand times.
# @@timeout: 200

test/triggers/libproc-execing-bkpts breakdance test/triggers/libproc-execing-bkpts-victim
