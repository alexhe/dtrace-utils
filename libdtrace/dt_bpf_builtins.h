/*
 * Oracle Linux DTrace.
 * Copyright (c) 2019, Oracle and/or its affiliates. All rights reserved.
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#ifndef _DT_BPF_BUILTINS_H
#define _DT_BPF_BUILTINS_H

#ifdef FIXME
#ifdef  __cplusplus
extern "C" {
#endif

#define DT_BPF_MAP_BUILTINS(FN) \
	FN(get_bvar), \
	FN(get_gvar), \
	FN(get_string), \
	FN(get_tvar), \
	FN(memcpy), \
	FN(set_gvar), \
	FN(set_tvar), \
	FN(strnlen)

#define DT_BPF_ENUM_FN(x, y)	DT_BPF_ ## x
enum dt_bpf_builtin_ids {
        DT_BPF_MAP_BUILTINS(DT_BPF_ENUM_FN),
        DT_BPF_LAST_ID,
};
#undef DT_BPF_ENUM_FN

typedef struct dt_bpf_func	dt_bpf_func_t;
typedef struct dt_bpf_builtin	dt_bpf_builtin_t;
struct dt_bpf_builtin {
	const char	*name;
	dt_bpf_func_t	*sym;
};

extern dt_bpf_builtin_t		dt_bpf_builtins[];

#ifdef  __cplusplus
}
#endif
#endif

#endif /* _DT_BPF_FUNCS_H */
