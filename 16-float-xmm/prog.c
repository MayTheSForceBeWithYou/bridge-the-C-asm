#include <stdio.h>
#include "../test/check.h"

float addf(float a, float b)
{
    /* TODO */
    (void)a; (void)b;
    return 0.0f;
}

double muld(double a, double b)
{
    /* TODO */
    (void)a; (void)b;
    return 0.0;
}

double mix(double a, int n)
{
    /* TODO: e.g. a * n */
    (void)a; (void)n;
    return 0.0;
}

int main(void)
{
    float af = addf(1.5f, 2.5f);
    double md = muld(2.0, 3.0);
    double mx = mix(1.5, 4);
    CHECK(af >= 3.99f && af <= 4.01f);
    CHECK(md >= 5.99 && md <= 6.01);
    CHECK(mx >= 5.99 && mx <= 6.01);
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
