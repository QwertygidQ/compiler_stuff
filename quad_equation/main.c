#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

enum solution_type
{
    NONE = 0, ONE = 1, TWO = 2, INFINITE = 3
};

struct solution
{
    enum solution_type solutions_count;
    double x1, x2;
};

void get_input(const char *msg, double *var);
bool is_not_zero(const double a);
struct solution* solve_quad_equation(const double a, const double b, const double c);
const double solve_linear_equation(const double a, const double b); // ax+b = 0, a != 0
void error(const char *msg);

int main()
{
    printf("\nQuadratic equation solver\nQwertygid, 2016\n\n");

    printf("ax^2 + bx + c = 0\n");
    
    double a = 0.0, b = 0.0, c = 0.0;

    get_input("a = ", &a);
    get_input("b = ", &b);
    get_input("c = ", &c);

    struct solution *s = solve_quad_equation(a, b, c);
    if (s == NULL)
        error("Solution struct is somehow equal to NULL\n");
        
    switch (s -> solutions_count)
    {
    case NONE:
        printf("No solutions found\n");
        break;
    case ONE:
        printf("x = %lf\n", s -> x1);
        break;
    case TWO:
        printf("x1 = %lf\nx2 = %lf\n", s -> x1, s -> x2);
        break;
    case INFINITE:
        printf("Infinitely many solutions found\n");
        break;
    default:
        error("Undefined solution type\n");
    }

    return EXIT_SUCCESS;
}

void get_input(const char *msg, double *var)
{
    const int INPUT_THRESHOLD = 5;
    
    for (int i = 0; i < INPUT_THRESHOLD; i++)
    {
        printf(msg);
        if (scanf("%lf", var) != 1)
            scanf("%*s");
        else
            return;
    }
    
    error("Too many tries\n");
}

bool is_not_zero(const double a)
{
    return fabs(a) > FLT_EPSILON;
}

struct solution* solve_quad_equation(const double a, const double b, const double c)
{
    static struct solution s;

            
    if (is_not_zero(a))
    {
        if (is_not_zero(c))
        {
            const double discriminant = b * b - 4 * a * c;
            if (discriminant < 0)
                s.solutions_count = NONE;
            else
            {
                const double discr_sqrt = sqrt(discriminant);
                if (!is_not_zero(discriminant))
                {
                    s.solutions_count = ONE;
                    s.x1 = (-b + discr_sqrt) / (2 * a);
                }
                else
                {
                    s.solutions_count = TWO;
                    s.x1 = (-b + discr_sqrt) / (2 * a);
                    s.x2 = (-b - discr_sqrt) / (2 * a);
                }
            }
        }
        else if (is_not_zero(b))
        {
            s.solutions_count = TWO;
            s.x1 = 0;
            s.x2 = solve_linear_equation(a, b);
        }
        else
        {
            s.solutions_count = ONE;
            s.x1 = 0;
        }
    }
    else
    {
        if (is_not_zero(b) && is_not_zero(c))
        {
            s.solutions_count = ONE;
            s.x1 = solve_linear_equation(b, c);
        }
        else if (is_not_zero(b))
        {
            s.solutions_count = ONE;
            s.x1 = 0;
        }
        else if (is_not_zero(c))
            s.solutions_count = NONE;
        else
            s.solutions_count = INFINITE;
    }
    
    return &s;
}

const double solve_linear_equation(const double a, const double b) // ax+b = 0, a != 0
{
    if (a == 0)
        error("a == 0 in solve_linear_equation()\n");
    
    return -b / a;
        
}

void error(const char *msg)
{
    fprintf(stderr, msg);
    exit(EXIT_FAILURE);
}
