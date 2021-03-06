#include <poor_map.h>
#include <assert.h>

#undef NDEBUG

#if defined __STDC_VERSION__ && __STDC_VERSION__ <= 201710L

#undef static_assert
#define static_assert_m(_expr_, _string_) _Static_assert(_expr_, _string_)
#define static_assert_s(_expr_) _Static_assert(_expr_, "static_assert")
#define static_assert(...) IF_SINGLE_ARG(static_assert_s ,static_assert_m ,__VA_ARGS__)(__VA_ARGS__)

#else

#undef static_assert
#define static_assert(_expr_, ...) assert(_expr_)

#endif //__STDC_VERSION__

#include "args_count.h"

int main(void) {
/* MAP_ variants */
#define do_paste_val(val) char aa1_ ## val [val];
	MAP(do_paste_val, 2,8,5)
	static_assert(sizeof (aa1_2) == 2 );
	static_assert(sizeof (aa1_8) == 8 );
	static_assert(sizeof (aa1_5) == 5 );

#define do_paste_arg_val(arg, val) char aa2_ ## arg ## val [val];
	MAP_ARG(do_paste_arg_val, 3, 2,8,5)
	static_assert(sizeof (aa2_32) == 2);
	static_assert(sizeof (aa2_38) == 8);
	static_assert(sizeof (aa2_35) == 5);

#define do_paste_idx_val(idx, val) char tt_ ## idx ## val [ val ];
	MAP_INDEX(do_paste_idx_val, 2, 4, 6)
	static_assert(sizeof(tt_02) == 2);
	static_assert(sizeof(tt_14) == 4);
	static_assert(sizeof(tt_26) == 6);

#define do_paste_idx_arg_val(idx, arg, val) char tte_ ## idx ## arg ## val [val];
	MAP_ARG_INDEX(do_paste_idx_arg_val, 7, 2, 5, 6)
	static_assert(sizeof (tte_072) == 2);
	static_assert(sizeof (tte_175) == 5);
	static_assert(sizeof (tte_276) == 6);


/* MAP_SEP_ variants */
#define do_paste_val2(val) val
	char aar [ MAP_SEP((+), do_paste_val2, 3, 6, 8) ];
	static_assert(sizeof (aar) == 3 + 6 + 8);

#define do_paste_arg_val2(arg, val) arg * val
	static_assert(MAP_SEP_ARG((+), do_paste_arg_val2, 10, 9, 6, 7) == 10 * (9 + 6 + 7));

#define do_paste_idx_val2(idx, val) [idx] = val
	char er[] = { MAP_SEP_IDX((,), do_paste_idx_val2, 2, 3, 4, 5)  };
	static_assert(sizeof (er) == 4);

#define do_paste_idx_arg_val2(idx, arg, val) char ffe_ ## idx ## arg ## val [val]
	MAP_SEP_ARG_IDX((;), do_paste_idx_arg_val2, 7, 3, 6, 9);
	static_assert(sizeof (ffe_073) == 3);
	static_assert(sizeof (ffe_176) == 6);
	static_assert(sizeof (ffe_279) == 9);


/* Nested MAP */
#define do_paste_val_lvl2(val) char eee_ ## val [ val ];
#define do_paste_val_lvl1(val) MAP2(do_paste_val_lvl2, val ## 7, val ## 8, val ## 9)
	MAP(do_paste_val_lvl1, 1, 2, 3)
	static_assert(sizeof (eee_17) == 17);
	static_assert(sizeof (eee_18) == 18);
	static_assert(sizeof (eee_19) == 19);
	static_assert(sizeof (eee_27) == 27);
	static_assert(sizeof (eee_28) == 28);
	static_assert(sizeof (eee_29) == 29);
	static_assert(sizeof (eee_37) == 37);
	static_assert(sizeof (eee_38) == 38);
	static_assert(sizeof (eee_39) == 39);

/* Nested MAP_INDEX */
#define do_paste_idx_val_l2(idx, val) char ert_ ## val ## idx [ val ];
#define do_paste_idx_val_l1(idx, val) MAP2_INDEX(do_paste_idx_val_l2, \
	4 ## idx ## val, \
	5 ## idx ## val)

	MAP_INDEX(do_paste_idx_val_l1, 3, 6, 7)
	static_assert(sizeof (ert_4030) == 403);
	static_assert(sizeof (ert_4160) == 416);
	static_assert(sizeof (ert_4270) == 427);
	static_assert(sizeof (ert_5031) == 503);
	static_assert(sizeof (ert_5161) == 516);
	static_assert(sizeof (ert_5271) == 527);


/* Nested MAP_LIST_ARG */
#define h_arr2_dim(arg, val) arg + val
#define h_arr1_dim(arg, val) { MAP2_LIST_ARG(h_arr2_dim, arg + val, 1, 2, 3, 4) }
	char a1[][4] = { MAP_LIST_ARG(h_arr1_dim, 100, 10, 20, 30, 40) };
	static_assert(sizeof(a1) == 16);

/* RECURSION() test */
#define h_rec_val(prev, val) prev ## val
	char RECURSION(h_rec_val, rrr, 1);
	(void)rrr1;
	char RECURSION(h_rec_val, rrr, 1, 2);
	(void)rrr12;
	char RECURSION(h_rec_val, rrr, 1, 2, 3);
	(void)rrr123;
	char RECURSION(h_rec_val, rrr, 1, 2, 3, 4);
	(void)rrr1234;
	char RECURSION(h_rec_val, rrr, 1, 2, 3, 4, 5);
	(void)rrr12345;

#define h_rec_arg_val(arg, prev, val) arg ## prev ## val
	char RECURSION_ARG(h_rec_arg_val, y, z, 1);
	(void)yz1;
	char RECURSION_ARG(h_rec_arg_val, y, z, 1, 2);
	(void)yyz12;
	char RECURSION_ARG(h_rec_arg_val, y, z, 1, 2, 3);
	(void)yyyz123;
	char RECURSION_ARG(h_rec_arg_val, y, z, 1, 2, 3, 4);
	(void)yyyyz1234;
	char RECURSION_ARG(h_rec_arg_val, y, z, 1, 2, 3, 4, 5);
	(void)yyyyyz12345;

	return 0;
}
