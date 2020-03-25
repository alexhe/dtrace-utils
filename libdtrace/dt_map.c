/*
 * Oracle Linux DTrace.
 * Copyright (c) 2006, 2019, Oracle and/or its affiliates. All rights reserved.
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>

#include <dt_impl.h>
#include <dt_pcb.h>
#include <dt_probe.h>
#include <dt_printf.h>

static void
dt_datadesc_hold(dtrace_datadesc_t *ddp)
{
	ddp->dtdd_refcnt++;
}

static void
dt_datadesc_release(dtrace_hdl_t *dtp, dtrace_datadesc_t *ddp)
{
	if (--ddp->dtdd_refcnt > 0)
		return;

	dt_free(dtp, ddp->dtdd_recs);
	dt_free(dtp, ddp);
}

dtrace_datadesc_t *
dt_datadesc_create(dtrace_hdl_t *dtp)
{
	dtrace_datadesc_t	*ddp;

	ddp = dt_zalloc(dtp, sizeof(dtrace_datadesc_t));
	if (ddp == NULL) {
		dt_set_errno(dtp, EDT_NOMEM);
		return NULL;
	}

	dt_datadesc_hold(ddp);

	return ddp;
}

int
dt_datadesc_finalize(dtrace_hdl_t *dtp, dtrace_datadesc_t *ddp)
{
	dt_pcb_t	*pcb = dtp->dt_pcb;

	/*
	 * If the number of allocated data records is greater than the actual
	 * number needed, we create a copy with the right number of records and
	 * free the oversized one.
	 */
	if (pcb->pcb_nrecs < pcb->pcb_maxrecs) {
		dtrace_recdesc_t	*nrecs;

		nrecs = dt_calloc(dtp, pcb->pcb_nrecs,
				  sizeof(dtrace_recdesc_t));
		if (nrecs == NULL)
			return dt_set_errno(dtp, EDT_NOMEM);

		memcpy(nrecs, ddp->dtdd_recs, pcb->pcb_nrecs);
		dt_free(dtp, ddp->dtdd_recs);
		ddp->dtdd_recs = nrecs;
		pcb->pcb_maxrecs = pcb->pcb_nrecs;
	}

	ddp->dtdd_nrecs = pcb->pcb_nrecs;

	return 0;
}

#if 0
static int
dt_epid_add(dtrace_hdl_t *dtp, dtrace_epid_t id)
{
	dtrace_id_t max;
	int rval, i, maxformat;
	dtrace_datadesc_t *dd, *ndd;
	dtrace_probedesc_t *probe;

	while (id >= (max = dtp->dt_maxprobe) || dtp->dt_pdesc == NULL) {
		dtrace_id_t new_max = max ? (max << 1) : 1;
		size_t nsize = new_max * sizeof (void *);
		dtrace_probedesc_t **new_pdesc;
		dtrace_datadesc_t **new_ddesc;

		if ((new_pdesc = malloc(nsize)) == NULL ||
		    (new_ddesc = malloc(nsize)) == NULL) {
			free(new_pdesc);
			return (dt_set_errno(dtp, EDT_NOMEM));
		}

		memset(new_pdesc, 0, nsize);
		memset(new_ddesc, 0, nsize);

		if (dtp->dt_pdesc != NULL) {
			size_t osize = max * sizeof (void *);

			memcpy(new_pdesc, dtp->dt_pdesc, osize);
			free(dtp->dt_pdesc);

			memcpy(new_ddesc, dtp->dt_ddesc, osize);
			free(dtp->dt_ddesc);
		}

		dtp->dt_pdesc = new_pdesc;
		dtp->dt_ddesc = new_ddesc;
		dtp->dt_maxprobe = new_max;
	}

	if (dtp->dt_pdesc[id] != NULL)
		return (0);

	if ((dd = malloc(sizeof (dtrace_datadesc_t))) == NULL)
		return (dt_set_errno(dtp, EDT_NOMEM));

	memset(dd, 0, sizeof (dtrace_datadesc_t));
	dd->dtdd_epid = id;
	dd->dtdd_nrecs = 1;

	if (dt_ioctl(dtp, DTRACEIOC_EPROBE, dd) == -1) {
		rval = dt_set_errno(dtp, errno);
		free(dd);
		return (rval);
	}

	if (DTRACE_SIZEOF_EPROBEDESC(dd) != sizeof (*dd)) {
		/*
		 * There must be more than one action.  Allocate the
		 * appropriate amount of space and try again.
		 */
		if ((ndd = malloc(DTRACE_SIZEOF_EPROBEDESC(dd))) != NULL)
			memcpy(ndd, dd, sizeof (*dd));

		free(dd);

		if ((dd = ndd) == NULL)
			return (dt_set_errno(dtp, EDT_NOMEM));

		rval = dt_ioctl(dtp, DTRACEIOC_EPROBE, dd);

		if (rval == -1) {
			rval = dt_set_errno(dtp, errno);
			free(dd);
			return (rval);
		}
	}

	if ((probe = malloc(sizeof (dtrace_probedesc_t))) == NULL) {
		free(dd);
		return (dt_set_errno(dtp, EDT_NOMEM));
	}

	probe->id = dd->dtdd_probeid;

	if (dt_ioctl(dtp, DTRACEIOC_PROBES, probe) == -1) {
		rval = dt_set_errno(dtp, errno);
		goto err;
	}

	for (i = 0; i < dd->dtdd_nrecs; i++) {
		dtrace_fmtdesc_t fmt;
		dtrace_recdesc_t *rec = &dd->dtdd_rec[i];

		if (!DTRACEACT_ISPRINTFLIKE(rec->dtrd_action))
			continue;

		if (rec->dtrd_format == 0)
			continue;

		if (rec->dtrd_format <= dtp->dt_maxformat &&
		    dtp->dt_formats[rec->dtrd_format - 1] != NULL)
			continue;

		memset(&fmt, 0, sizeof (fmt));
		fmt.dtfd_format = rec->dtrd_format;
		fmt.dtfd_string = NULL;
		fmt.dtfd_length = 0;

		if (dt_ioctl(dtp, DTRACEIOC_FORMAT, &fmt) == -1) {
			rval = dt_set_errno(dtp, errno);
			goto err;
		}

		if ((fmt.dtfd_string = malloc(fmt.dtfd_length)) == NULL) {
			rval = dt_set_errno(dtp, EDT_NOMEM);
			goto err;
		}

		if (dt_ioctl(dtp, DTRACEIOC_FORMAT, &fmt) == -1) {
			rval = dt_set_errno(dtp, errno);
			free(fmt.dtfd_string);
			goto err;
		}

		while (rec->dtrd_format > (maxformat = dtp->dt_maxformat)) {
			int new_max = maxformat ? (maxformat << 1) : 1;
			size_t nsize = new_max * sizeof (void *);
			size_t osize = maxformat * sizeof (void *);
			void **new_formats = malloc(nsize);

			if (new_formats == NULL) {
				rval = dt_set_errno(dtp, EDT_NOMEM);
				free(fmt.dtfd_string);
				goto err;
			}

			memset(new_formats, 0, nsize);
			memcpy(new_formats, dtp->dt_formats, osize);
			free(dtp->dt_formats);

			dtp->dt_formats = new_formats;
			dtp->dt_maxformat = new_max;
		}

		dtp->dt_formats[rec->dtrd_format - 1] =
		    rec->dtrd_action == DTRACEACT_PRINTA ?
		    dtrace_printa_create(dtp, fmt.dtfd_string) :
		    dtrace_printf_create(dtp, fmt.dtfd_string);

		free(fmt.dtfd_string);

		if (dtp->dt_formats[rec->dtrd_format - 1] == NULL) {
			rval = -1; /* dt_errno is set for us */
			goto err;
		}
	}

	dtp->dt_pdesc[id] = probe;
	dtp->dt_ddesc[id] = dd;

	return (0);

err:
	/*
	 * If we failed, free our allocated probes.  Note that if we failed
	 * while allocating formats, we aren't going to free formats that
	 * we have already allocated.  This is okay; these formats are
	 * hanging off of dt_formats and will therefore not be leaked.
	 */
	free(dd);
	free(probe);
	return (rval);
}
#else
/*
 * Associate a probe data description and probe description with an enabled
 * probe ID.  This means that the given ID refers to the program matching the
 * probe data description being attached to the probe that matches the probe
 * description.
 */
dtrace_epid_t
dt_epid_add(dtrace_hdl_t *dtp, dtrace_datadesc_t *ddp, dtrace_id_t prid)
{
	dtrace_id_t		max = dtp->dt_maxprobe;
	dtrace_epid_t		epid;

	epid = dtp->dt_nextepid++;
	if (epid >= max || dtp->dt_ddesc == NULL) {
		dtrace_id_t		nmax = max ? (max << 1) : 2;
		dtrace_datadesc_t	**nddesc;
		dtrace_probedesc_t	**npdesc;

		nddesc = dt_calloc(dtp, nmax, sizeof(void *));
		npdesc = dt_calloc(dtp, nmax, sizeof(void *));
		if (nddesc == NULL || npdesc == NULL) {
			dt_free(dtp, nddesc);
			dt_free(dtp, npdesc);
			return dt_set_errno(dtp, EDT_NOMEM);
		}

		if (dtp->dt_ddesc != NULL) {
			size_t	osize = max * sizeof(void *);

			memcpy(nddesc, dtp->dt_ddesc, osize);
			dt_free(dtp, dtp->dt_ddesc);
			memcpy(npdesc, dtp->dt_pdesc, osize);
			dt_free(dtp, dtp->dt_pdesc);
		}

		dtp->dt_ddesc = nddesc;
		dtp->dt_pdesc = npdesc;
		dtp->dt_maxprobe = nmax;
	}

	if (dtp->dt_ddesc[epid] != NULL)
		return epid;

	dt_datadesc_hold(ddp);
	dtp->dt_ddesc[epid] = ddp;
	dtp->dt_pdesc[epid] = (dtrace_probedesc_t *)dtp->dt_probes[prid]->desc;

	return epid;
}
#endif

int
dt_epid_lookup(dtrace_hdl_t *dtp, dtrace_epid_t epid, dtrace_datadesc_t **ddp,
	       dtrace_probedesc_t **pdp)
{
	assert(epid < dtp->dt_maxprobe);
	assert(dtp->dt_ddesc[epid] != NULL);
	assert(dtp->dt_pdesc[epid] != NULL);

	*ddp = dtp->dt_ddesc[epid];
	*pdp = dtp->dt_pdesc[epid];

	return (0);
}

void
dt_epid_destroy(dtrace_hdl_t *dtp)
{
	size_t i;

	assert((dtp->dt_pdesc != NULL && dtp->dt_ddesc != NULL &&
	    dtp->dt_maxprobe > 0) || (dtp->dt_pdesc == NULL &&
	    dtp->dt_ddesc == NULL && dtp->dt_maxprobe == 0));

	if (dtp->dt_pdesc == NULL)
		return;

	for (i = 0; i < dtp->dt_maxprobe; i++) {
		if (dtp->dt_ddesc[i] == NULL) {
			assert(dtp->dt_pdesc[i] == NULL);
			continue;
		}

		dt_datadesc_release(dtp, dtp->dt_ddesc[i]);
		assert(dtp->dt_pdesc[i] != NULL);
	}

	free(dtp->dt_pdesc);
	dtp->dt_pdesc = NULL;

	free(dtp->dt_ddesc);
	dtp->dt_ddesc = NULL;
	dtp->dt_nextepid = 0;
	dtp->dt_maxprobe = 0;
}

int
dt_rec_add(dtrace_hdl_t *dtp, dtrace_actkind_t kind, uint32_t size,
	   uint32_t offset, uint16_t alignment, const char *fmt, uint64_t arg)
{
	dt_pcb_t		*pcb = dtp->dt_pcb;
	dtrace_datadesc_t	*ddp = pcb->pcb_stmt->dtsd_ddesc;
	dtrace_recdesc_t	*rec;
	int			cnt, max;

	cnt = pcb->pcb_nrecs + 1;
	max = pcb->pcb_maxrecs;
	if (cnt >= max) {
		int			nmax = max ? (max << 1) : cnt;
		dtrace_recdesc_t	*nrecs;

		nrecs = dt_calloc(dtp, cnt, sizeof(dtrace_recdesc_t));
		if (nrecs == NULL)
			return dt_set_errno(dtp, EDT_NOMEM);

		if (ddp->dtdd_recs != NULL) {
			size_t	osize = max * sizeof(dtrace_recdesc_t);

			memcpy(nrecs, ddp->dtdd_recs, osize);
			dt_free(dtp, ddp->dtdd_recs);
		}

		ddp->dtdd_recs = nrecs;
		pcb->pcb_maxrecs = nmax;
	}

	rec = &ddp->dtdd_recs[pcb->pcb_nrecs++];
	rec->dtrd_action = kind;
	rec->dtrd_size = size;
	rec->dtrd_offset = offset;
	rec->dtrd_alignment = alignment;
	rec->dtrd_format = 0;	/* FIXME */
	rec->dtrd_arg = arg;

	return 0;
}

void *
dt_format_lookup(dtrace_hdl_t *dtp, int format)
{
	if (format == 0 || format > dtp->dt_maxformat)
		return (NULL);

	if (dtp->dt_formats == NULL)
		return (NULL);

	return (dtp->dt_formats[format - 1]);
}

void
dt_format_destroy(dtrace_hdl_t *dtp)
{
	int i;

	for (i = 0; i < dtp->dt_maxformat; i++) {
		if (dtp->dt_formats[i] != NULL)
			dt_printf_destroy(dtp->dt_formats[i]);
	}

	free(dtp->dt_formats);
	dtp->dt_formats = NULL;
}

static int
dt_aggid_add(dtrace_hdl_t *dtp, dtrace_aggid_t id)
{
	dtrace_id_t max;
	int rval;

	while (id >= (max = dtp->dt_maxagg) || dtp->dt_aggdesc == NULL) {
		dtrace_id_t new_max = max ? (max << 1) : 1;
		size_t nsize = new_max * sizeof (void *);
		dtrace_aggdesc_t **new_aggdesc;

		if ((new_aggdesc = malloc(nsize)) == NULL)
			return (dt_set_errno(dtp, EDT_NOMEM));

		memset(new_aggdesc, 0, nsize);

		if (dtp->dt_aggdesc != NULL) {
			memcpy(new_aggdesc, dtp->dt_aggdesc,
			    max * sizeof (void *));
			free(dtp->dt_aggdesc);
		}

		dtp->dt_aggdesc = new_aggdesc;
		dtp->dt_maxagg = new_max;
	}

	if (dtp->dt_aggdesc[id] == NULL) {
		dtrace_aggdesc_t *agg, *nagg;

		if ((agg = malloc(sizeof (dtrace_aggdesc_t))) == NULL)
			return (dt_set_errno(dtp, EDT_NOMEM));

		memset(agg, 0, sizeof (dtrace_aggdesc_t));
		agg->dtagd_id = id;
		agg->dtagd_nrecs = 1;

		if (dt_ioctl(dtp, DTRACEIOC_AGGDESC, agg) == -1) {
			rval = dt_set_errno(dtp, errno);
			free(agg);
			return (rval);
		}

		if (DTRACE_SIZEOF_AGGDESC(agg) != sizeof (*agg)) {
			/*
			 * There must be more than one action.  Allocate the
			 * appropriate amount of space and try again.
			 */
			if ((nagg = malloc(DTRACE_SIZEOF_AGGDESC(agg))) != NULL)
				memcpy(nagg, agg, sizeof (*agg));

			free(agg);

			if ((agg = nagg) == NULL)
				return (dt_set_errno(dtp, EDT_NOMEM));

			rval = dt_ioctl(dtp, DTRACEIOC_AGGDESC, agg);

			if (rval == -1) {
				rval = dt_set_errno(dtp, errno);
				free(agg);
				return (rval);
			}
		}

		/*
		 * If we have a uarg, it's a pointer to the compiler-generated
		 * statement; we'll use this value to get the name and
		 * compiler-generated variable ID for the aggregation.  If
		 * we're grabbing an anonymous enabling, this pointer value
		 * is obviously meaningless -- and in this case, we can't
		 * provide the compiler-generated aggregation information.
		 */
		if (dtp->dt_options[DTRACEOPT_GRABANON] == DTRACEOPT_UNSET &&
		    agg->dtagd_rec[0].dtrd_uarg != 0) {
			dtrace_stmtdesc_t *sdp;
			dt_ident_t *aid;

			sdp = (dtrace_stmtdesc_t *)(uintptr_t)
			    agg->dtagd_rec[0].dtrd_uarg;
			aid = sdp->dtsd_aggdata;
			agg->dtagd_name = aid->di_name;
			agg->dtagd_varid = aid->di_id;
		} else {
			agg->dtagd_varid = DTRACE_AGGVARIDNONE;
		}

#if 0
		if ((epid = agg->dtagd_epid) >= dtp->dt_maxprobe ||
		    dtp->dt_pdesc[epid] == NULL) {
			if ((rval = dt_epid_add(dtp, epid)) != 0) {
				free(agg);
				return (rval);
			}
		}
#endif

		dtp->dt_aggdesc[id] = agg;
	}

	return (0);
}

int
dt_aggid_lookup(dtrace_hdl_t *dtp, dtrace_aggid_t aggid, dtrace_aggdesc_t **adp)
{
	int rval;

	if (aggid >= dtp->dt_maxagg || dtp->dt_aggdesc[aggid] == NULL) {
		if ((rval = dt_aggid_add(dtp, aggid)) != 0)
			return (rval);
	}

	assert(aggid < dtp->dt_maxagg);
	assert(dtp->dt_aggdesc[aggid] != NULL);
	*adp = dtp->dt_aggdesc[aggid];

	return (0);
}

void
dt_aggid_destroy(dtrace_hdl_t *dtp)
{
	size_t i;

	assert((dtp->dt_aggdesc != NULL && dtp->dt_maxagg != 0) ||
	    (dtp->dt_aggdesc == NULL && dtp->dt_maxagg == 0));

	if (dtp->dt_aggdesc == NULL)
		return;

	for (i = 0; i < dtp->dt_maxagg; i++) {
		if (dtp->dt_aggdesc[i] != NULL)
			free(dtp->dt_aggdesc[i]);
	}

	free(dtp->dt_aggdesc);
	dtp->dt_aggdesc = NULL;
	dtp->dt_maxagg = 0;
}
