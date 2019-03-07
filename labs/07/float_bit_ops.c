#include <stdio.h>
#include <math.h>

/* Returns a floating point number with the specified
   values for its sign, exponent, and mantissa. */
float make_float(unsigned sign, int exp, unsigned mantissa) {
    unsigned bits = /* YOUR CODE HERE */;

    /* Don't touch these lines. Explanation in lab spec! */
    float *ptr_to_result = (float*) &bits;
    return *ptr_to_result;
}

/* Returns log base 2 of: by how much the value of *x 
   would increase if we incremented its mantissa by the 
   smallest value possible. */
int lg_step_size(float *x) {
    unsigned *ptr = (unsigned *) x;		//ptr is a pointer to the bits of x, just like ptr_to_result above!!
    /* YOUR CODE HERE */
    return 0;					//replace this line
}

/*
 * YOU CAN IGNORE THE REST OF THIS FILE
 */

void test_make_float(unsigned sign, unsigned exponent, unsigned mantissa, float expected) {
    float result = make_float(sign, exponent, mantissa);
    if(result!=expected && result==result && expected==expected) {
        printf("make_float(%u, %d, 0x%x): %f, expected %f\n",sign,exponent,mantissa,result,expected);
    } else {
        printf("make_float(%u, %d, 0x%x): %f, correct\n",sign,exponent,mantissa,result);
    }
}

void test_lg_step_size(float *x, int expected) {
    int result = lg_step_size(x);
    if (result != expected) {
        printf("lg_step_size(&%f): %d, expected %d\n",*x,result,expected);
    } else {
        printf("lg_step_size(&%f): %d, correct\n",*x,result);
    }
}

int main(int argc,
         const char * argv[]) {
    float smallest = -1.40129846432481707092372958328991613128026194187651577175706828388979108268586060148663818836212158203125E-45;
    test_make_float(0, -127, 0, 0.0);
    test_make_float(0, 0, 3 << 21, 1.75);
    test_make_float(1, 1, 3 << 21, -3.5);
    printf("lol exercise 3.5\n");
    test_make_float(1, -127, 1, smallest);
    test_make_float(0, 128, 0xface, NAN);
    test_make_float(1, 128, 0, -INFINITY);
    float x = 65535;
    int lg_step = -8;
    for (; x > 0.05; x /= 4) {
        test_lg_step_size(&x, lg_step);
        lg_step -= 2;
    }
    test_lg_step_size(&smallest, -149);
    return 0;
}
