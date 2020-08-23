#ifndef PRIVATE_UTILMACS_H
#define PRIVATE_UTILMACS_H



/* ----- PUBLIC MACROS ----- */

#define MacWrap(expr) do {expr} while (0)

#define Min(a, b) \
	({ \
		typeof(a) Min_a = (a); \
		typeof(b) Min_b = (b); \
		(Min_a < Min_b) ? Min_a : Min_b; \
	})

#define Max(a, b) \
	({ \
		typeof(a) Max_a = (a); \
		typeof(b) Max_b = (b); \
		(Max_a > Max_b) ? Max_a : Max_b; \
	})



#endif /* PRIVATE_UTILMACS_H */
