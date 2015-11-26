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
# This script tests that libproc can still track the state of the link map of a
# multithreaded process when it is continuously changing.  We turn LD_AUDIT off
# first to make sure that only one lmid is in use.
#
# The number of threads is arbitrary: it only matters that it is >1.
#

# @@timeout: 50

# @@skip: almost guaranteed to fail due to loss of latching of process pre-libproc-mt

unset LD_AUDIT
export NUM_THREADS=4

exec test/triggers/libproc-consistency test/triggers/libproc-dlmadopen
