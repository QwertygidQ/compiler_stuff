#include <stdio.h>
#include <math.h>
#include <float.h>

enum solution_type
{
    NONE = 0, ONE = 1, TWO = 2, INFINITE = 3
};

struct solution
{
    enum solution_type solutions_count;
    double x1, x2;
};

const struct solution solve_quad_equation(const double a, const double b, const double c);

int main()
{
    double a = 0.0, b = 0.0, c = 0.0;

    printf("ax^2 + bx + c = 0\n");

    printf("a = ");
    scanf("%lf", &a);

    printf("b = ");
    scanf("%lf", &b);

    printf("c = ");
    scanf("%lf", &c);

    const struct solution s = solve_quad_equation(a, b, c);

    switch (s.solutions_count)
    {
    case NONE:
        printf("No solutions found\n");
        break;
    case ONE:
        printf("x = %lf\n", s.x1);
        break;
    case TWO:
        printf("x1 = %lf\nx2 = %lf\n", s.x1, s.x2);
        break;
    case INFINITE:
        printf("Infinitely many solutions found\n");
    }

    return 0;
}

const struct solution solve_quad_equation(const double a, const double b, const double c)
{
    struct solution s;

    if (fabs(a) > FLT_EPSILON)
    {
        double discriminant = b * b - 4 * a * c;
        if (discriminant < 0)
            s.solutions_count = NONE;
        else if (discriminant == 0)
        {
            s.solutions_count = ONE;
            s.x1 = (-b + sqrt(discriminant)) / (2 * a);
        }
        else
        {
            s.solutions_count = TWO;
            s.x1 = (-b + sqrt(discriminant)) / (2 * a);
            s.x2 = (-b - sqrt(discriminant)) / (2 * a);
        }
    }
    else if (fabs(b) > FLT_EPSILON)
    {
        s.solutions_count = ONE;
        s.x1 = -c / b;
    }
    else if (fabs(c) > FLT_EPSILON)
        s.solutions_count = NONE;
    else
        s.solutions_count = INFINITE;

    return s;
}
