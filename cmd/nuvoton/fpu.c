// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

/*
 * Misc functions
 */
#include <common.h>
#include <command.h>
#include <console.h>


union uf
{
	unsigned int u;
	float f;
};

static float
u2f (unsigned int v)
{
	union uf u;
	u.u = v;
	return u.f;
}

static unsigned int
f2u (float v)
{
	union uf u;
	u.f = v;
	return u.u;
}

static int ok = 1;

static void
tstmul (unsigned int ux, unsigned int uy, unsigned int ur)
{
	float x = u2f (ux);
	float y = u2f (uy);

	if (f2u (x * y) != ur)
	/* Set a variable rather than aborting here, to simplify tracing when
	   several computations are wrong.  */
		ok = 0;
}

/* We don't want to make this const and static, or else we risk inlining
   causing the test to fold as constants at compile-time.  */
struct
{
  unsigned int p1, p2, res;
} static volatile expected[] =
{
	{0xfff, 0x3f800400, 0xfff},
	{0xf, 0x3fc88888, 0x17},
	{0xf, 0x3f844444, 0xf}
};

static int fpu_test_math7 (void)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(expected); i++)
	{
		tstmul (expected[i].p1, expected[i].p2, expected[i].res);
		tstmul (expected[i].p2, expected[i].p1, expected[i].res);
	}

	if (!ok) {
		printf ("Error in FPU math7 test\n");
		return -1;
	}
	return 0;
}

static int failed;

#define TEST(c) if ((c) != ok) failed++
#define ORD(a, b) (!__builtin_isunordered ((a), (b)))
#define UNORD(a, b) (__builtin_isunordered ((a), (b)))
#define UNEQ(a, b) (__builtin_isunordered ((a), (b)) || ((a) == (b)))
#define UNLT(a, b) (__builtin_isunordered ((a), (b)) || ((a) < (b)))
#define UNLE(a, b) (__builtin_isunordered ((a), (b)) || ((a) <= (b)))
#define UNGT(a, b) (__builtin_isunordered ((a), (b)) || ((a) > (b)))
#define UNGE(a, b) (__builtin_isunordered ((a), (b)) || ((a) >= (b)))
#define LTGT(a, b) (__builtin_islessgreater ((a), (b)))

static float pinf;
static float ninf;
static float NaN;

static void iuneq (float x, float y, int ok)
{
	TEST (UNEQ (x, y));
	TEST (!LTGT (x, y));
	TEST (UNLE (x, y) && UNGE (x,y));
}

static void ieq (float x, float y, int ok)
{
	TEST (ORD (x, y) && UNEQ (x, y));
}

static void iltgt (float x, float y, int ok)
{
	TEST (!UNEQ (x, y)); /* Not optimizable. */
	TEST (LTGT (x, y)); /* Same, __builtin_islessgreater does not trap. */
	TEST (ORD (x, y) && (UNLT (x, y) || UNGT (x,y)));
}

static void ine (float x, float y, int ok)
{
	TEST (UNLT (x, y) || UNGT (x, y));
}

static void iunlt (float x, float y, int ok)
{
	TEST (UNLT (x, y));
	TEST (UNORD (x, y) || (x < y));
}

static void ilt (float x, float y, int ok)
{
	TEST (ORD (x, y) && UNLT (x, y)); /* Not optimized */
	TEST ((x <= y) && (x != y));
	TEST ((x <= y) && (y != x));
	TEST ((x != y) && (x <= y)); /* Not optimized */
	TEST ((y != x) && (x <= y)); /* Not optimized */
}

static void iunle (float x, float y, int ok)
{
	TEST (UNLE (x, y));
	TEST (UNORD (x, y) || (x <= y));
}

static void ile (float x, float y, int ok)
{
	TEST (ORD (x, y) && UNLE (x, y)); /* Not optimized */
	TEST ((x < y) || (x == y));
	TEST ((y > x) || (x == y));
	TEST ((x == y) || (x < y)); /* Not optimized */
	TEST ((y == x) || (x < y)); /* Not optimized */
}

static void iungt (float x, float y, int ok)
{
	TEST (UNGT (x, y));
	TEST (UNORD (x, y) || (x > y));
}

static void igt (float x, float y, int ok)
{
	TEST (ORD (x, y) && UNGT (x, y)); /* Not optimized */
	TEST ((x >= y) && (x != y));
	TEST ((x >= y) && (y != x));
	TEST ((x != y) && (x >= y)); /* Not optimized */
	TEST ((y != x) && (x >= y)); /* Not optimized */
}

static void iunge (float x, float y, int ok)
{
	TEST (UNGE (x, y));
	TEST (UNORD (x, y) || (x >= y));
}

static void ige (float x, float y, int ok)
{
	TEST (ORD (x, y) && UNGE (x, y)); /* Not optimized */
	TEST ((x > y) || (x == y));
	TEST ((y < x) || (x == y));
	TEST ((x == y) || (x > y)); /* Not optimized */
	TEST ((y == x) || (x > y)); /* Not optimized */
}

int fpu_test_math6 (void)
{
	pinf = __builtin_inf ();
	ninf = -__builtin_inf ();
	NaN = __builtin_nan ("");

	iuneq (ninf, pinf, 0);
	iuneq (NaN, NaN, 1);
	iuneq (pinf, ninf, 0);
	iuneq (1, 4, 0);
	iuneq (3, 3, 1);
	iuneq (5, 2, 0);

	ieq (1, 4, 0);
	ieq (3, 3, 1);
	ieq (5, 2, 0);

	iltgt (ninf, pinf, 1);
	iltgt (NaN, NaN, 0);
	iltgt (pinf, ninf, 1);
	iltgt (1, 4, 1);
	iltgt (3, 3, 0);
	iltgt (5, 2, 1);

	ine (1, 4, 1);
	ine (3, 3, 0);
	ine (5, 2, 1);

	iunlt (NaN, ninf, 1);
	iunlt (pinf, NaN, 1);
	iunlt (pinf, ninf, 0);
	iunlt (pinf, pinf, 0);
	iunlt (ninf, ninf, 0);
	iunlt (1, 4, 1);
	iunlt (3, 3, 0);
	iunlt (5, 2, 0);

	ilt (1, 4, 1);
	ilt (3, 3, 0);
	ilt (5, 2, 0);

	iunle (NaN, ninf, 1);
	iunle (pinf, NaN, 1);
	iunle (pinf, ninf, 0);
	iunle (pinf, pinf, 1);
	iunle (ninf, ninf, 1);
	iunle (1, 4, 1);
	iunle (3, 3, 1);
	iunle (5, 2, 0);

	ile (1, 4, 1);
	ile (3, 3, 1);
	ile (5, 2, 0);

	iungt (NaN, ninf, 1);
	iungt (pinf, NaN, 1);
	iungt (pinf, ninf, 1);
	iungt (pinf, pinf, 0);
	iungt (ninf, ninf, 0);
	iungt (1, 4, 0);
	iungt (3, 3, 0);
	iungt (5, 2, 1);

	igt (1, 4, 0);
	igt (3, 3, 0);
	igt (5, 2, 1);

	iunge (NaN, ninf, 1);
	iunge (pinf, NaN, 1);
	iunge (ninf, pinf, 0);
	iunge (pinf, pinf, 1);
	iunge (ninf, ninf, 1);
	iunge (1, 4, 0);
	iunge (3, 3, 1);
	iunge (5, 2, 1);

	ige (1, 4, 0);
	ige (3, 3, 1);
	ige (5, 2, 1);

	if (failed) {
		printf ("Error in FPU math6 test\n");
		return -1;
	}
	return 0;
}

int fpu_test_math1 (void)
{
	volatile double a;
	double c, d;
	volatile double b;

	d = 1.0;

	do
	{
		c = d;
		d = c * 0.5;
		b = 1 + d;
	} while (b != 1.0);

	a = 1.0 + c;

	if (a == 1.0) {
		printf ("Error in FPU math1 test\n");
		return -1;
	}

	return 0;
}

float rintf (float x)
{
	volatile float TWO23 = 8388608.0;

	if (__builtin_fabs (x) < TWO23)
	{
		if (x > 0.0)
		{
			x += TWO23;
			x -= TWO23;
		}
		else if (x < 0.0)
		{
			x = TWO23 - x;
			x = -(x - TWO23);
		}
	}

	return x;
}

int fpu_test_math2 (void)
{
	if (rintf (-1.5) != -2.0) {
		printf ("Error in FPU math2 test\n");
		return -1;
	}
	return 0;
}

int fpu_test_math3 (void)
{
#if 0  /* "long double" GCC aarch64 compiler do not support "long double" as 128bits */
	volatile long double dfrom = 1.1;
	volatile long double m1;
	volatile long double m2;
	volatile unsigned long mant_long;
#else
	volatile double dfrom = 1.1;
	volatile double m1;
	volatile double m2;
	volatile unsigned long mant_long;
#endif
	m1 = dfrom / 2.0;

	m2 = m1 * 4294967296.0L;

	mant_long = ((unsigned long) m2) & 0xffffffff;

	if (mant_long != 0x8ccccccc) {
		printf ("Error in FPU math3 test\n");
		return -1;
	}
	return 0;
}

int fpu_test_math4 (void)
{
	volatile float reale = 1.0f;
	volatile float oneplus;
	int i;

	if (sizeof (float) != 4)
		return 0;

	for (i = 0; ; i++)
	{
		oneplus = 1.0f + reale;
		if (oneplus == 1.0f)
			break;
		reale = reale / 2.0f;
	}
	/* Assumes ieee754 accurate arithmetic above.  */
	if (i != 24) {
		printf ("Error in FPU math4 test\n");
		return -1;
	}
	return 0;
}

double func (const double *array)
{
	double d = *array;

	if (d == 0.0)
		return d;
	else
		return d + func (array + 1);
}

int fpu_test_math5 (void)
{
	double values[] = { 0.1e-100, 1.0, -1.0, 0.0 };

	if (func (values) != 0.1e-100) {
		printf ("Error in FPU math5 test\n");
		return -1;
	}
	return 0;
}

static int do_fpu(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;

	ret += fpu_test_math1();
	if (ret == 0)
	   printf ("fpu_test_math1 - Pass\n");
	else
	   printf ("fpu_test_math1 - Fail\n");
	   
	ret += fpu_test_math2();
	if (ret == 0)
	   printf ("fpu_test_math2 - Pass\n");
	else
	   printf ("fpu_test_math2 - Fail\n");

	ret += fpu_test_math3();
	if (ret == 0)
	   printf ("fpu_test_math3 - Pass\n");
	else
	   printf ("fpu_test_math3 - Fail\n");

	ret += fpu_test_math4();
	if (ret == 0)
	   printf ("fpu_test_math4 - Pass\n");
	else
	   printf ("fpu_test_math4 - Fail\n");
	   
	ret += fpu_test_math5();
	if (ret == 0)
	   printf ("fpu_test_math5 - Pass\n");	
	else
	   printf ("fpu_test_math5 - Fail\n");
	   
	ret += fpu_test_math6();
	if (ret == 0)
	   printf ("fpu_test_math6 - Pass\n");
	else
	   printf ("fpu_test_math6 - Fail\n");
	   
	ret += fpu_test_math7();
	if (ret == 0)
	   printf ("fpu_test_math7 - Pass\n");
	else
	   printf ("fpu_test_math7 - Fail\n");

	if (ret != 0) {
		printf ("Error in FPU math test\n");
		return -1;
	}
	return 0;
}


U_BOOT_CMD(
	fpu,	1,	1,	do_fpu,
	"Test the simd and fpu functions",
	""
);

