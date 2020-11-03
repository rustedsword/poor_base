#include <poor_map.h>

#include "args_count.h"

int main(void) {
/* MAP_ variants */
#define do_paste_val(val) char TOKEN_CAT_2(aa1_, val) [val];
	MAP(do_paste_val, 2,8,5)
	_Static_assert(sizeof (aa1_2) == 2 );
	_Static_assert(sizeof (aa1_8) == 8 );
	_Static_assert(sizeof (aa1_5) == 5 );

#define do_paste_arg_val(arg, val) char TOKEN_CAT_2(aa2_, TOKEN_CAT_2(arg, val)) [val];
	MAP_ARG(do_paste_arg_val, 3, 2,8,5)
	_Static_assert(sizeof (aa2_32) == 2);
	_Static_assert(sizeof (aa2_38) == 8);
	_Static_assert(sizeof (aa2_35) == 5);

#define do_paste_idx_val(idx, val) char TOKEN_CAT_2(tt_, TOKEN_CAT_2(idx, val)) [ val ];
	MAP_INDEX(do_paste_idx_val, 2, 4, 6)
	_Static_assert(sizeof(tt_02) == 2);
	_Static_assert(sizeof(tt_14) == 4);
	_Static_assert(sizeof(tt_26) == 6);

#define do_paste_idx_arg_val(idx, arg, val) char TOKEN_CAT_2(tte_, TOKEN_CAT_2(idx, TOKEN_CAT_2(arg, val)))[val];
	MAP_ARG_INDEX(do_paste_idx_arg_val, 7, 2, 5, 6)
	_Static_assert(sizeof (tte_072) == 2);
	_Static_assert(sizeof (tte_175) == 5);
	_Static_assert(sizeof (tte_276) == 6);


/* MAP_SEP_ variants */
#define do_paste_val2(val) val
	char aar [ MAP_SEP((+), do_paste_val2, 3, 6, 8) ];
	_Static_assert(sizeof (aar) == 3 + 6 + 8);

#define do_paste_arg_val2(arg, val) arg * val
	_Static_assert(MAP_SEP_ARG((+), do_paste_arg_val2, 10, 9, 6, 7) == 10 * (9 + 6 + 7));

#define do_paste_idx_val2(idx, val) [idx] = val
	char er[] = { MAP_SEP_IDX((,), do_paste_idx_val2, 2, 3, 4, 5)  };
	_Static_assert(sizeof (er) == 4);

#define do_paste_idx_arg_val2(idx, arg, val) char TOKEN_CAT_2(ffe_, TOKEN_CAT_2(idx, TOKEN_CAT_2(arg, val)))[val]
	MAP_SEP_ARG_IDX((;), do_paste_idx_arg_val2, 7, 3, 6, 9);
	_Static_assert(sizeof (ffe_073) == 3);
	_Static_assert(sizeof (ffe_176) == 6);
	_Static_assert(sizeof (ffe_279) == 9);


/* Nested MAP */
#define do_paste_val_lvl2(val) char TOKEN_CAT_2(eee_, val) [ val ];
#define do_paste_val_lvl1(val) MAP2(do_paste_val_lvl2, TOKEN_CAT_2(val, 7), TOKEN_CAT_2(val, 8), TOKEN_CAT_2(val, 9))
	MAP(do_paste_val_lvl1, 1, 2, 3)
	_Static_assert(sizeof (eee_17) == 17);
	_Static_assert(sizeof (eee_18) == 18);
	_Static_assert(sizeof (eee_19) == 19);
	_Static_assert(sizeof (eee_27) == 27);
	_Static_assert(sizeof (eee_28) == 28);
	_Static_assert(sizeof (eee_29) == 29);
	_Static_assert(sizeof (eee_37) == 37);
	_Static_assert(sizeof (eee_38) == 38);
	_Static_assert(sizeof (eee_39) == 39);

/* Nested MAP_INDEX */
#define do_paste_idx_val_l2(idx, val) char TOKEN_CAT_2(ert_, TOKEN_CAT_2(val, idx)) [ val ];
#define do_paste_idx_val_l1(idx, val) MAP2_INDEX(do_paste_idx_val_l2, \
	TOKEN_CAT_2(4, TOKEN_CAT_2(idx, val)), \
	TOKEN_CAT_2(5, TOKEN_CAT_2(idx, val)))

	MAP_INDEX(do_paste_idx_val_l1, 3, 6, 7)
	_Static_assert(sizeof (ert_4030) == 403);
	_Static_assert(sizeof (ert_4160) == 416);
	_Static_assert(sizeof (ert_4270) == 427);
	_Static_assert(sizeof (ert_5031) == 503);
	_Static_assert(sizeof (ert_5161) == 516);
	_Static_assert(sizeof (ert_5271) == 527);


/* Nested MAP_LIST_ARG */
#define h_arr2_dim(arg, val) arg + val
#define h_arr1_dim(arg, val) { MAP2_LIST_ARG(h_arr2_dim, arg + val, 1, 2, 3, 4) }
	char a1[][4] = { MAP_LIST_ARG(h_arr1_dim, 100, 10, 20, 30, 40) };
	_Static_assert(sizeof(a1) == 16);

/* RECURSION() test */
#define h_rec_val(prev, val) TOKEN_CAT_2(prev, val)
	char RECURSION(h_rec_val, rrr, 1);
	rrr1 = 0;
	char RECURSION(h_rec_val, rrr, 1, 2);
	rrr12 = 0;
	char RECURSION(h_rec_val, rrr, 1, 2, 3);
	rrr123 = 0;
	char RECURSION(h_rec_val, rrr, 1, 2, 3, 4);
	rrr1234 = 0;
	char RECURSION(h_rec_val, rrr, 1, 2, 3, 4, 5);
	rrr12345 = 0;

	return 0;
}
