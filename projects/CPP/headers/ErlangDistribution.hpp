#ifndef C79B92EF_2EA5_43C7_A33B_166288190D1E
#define C79B92EF_2EA5_43C7_A33B_166288190D1E
#define _USE_MATH_DEFINES

#include <iostream>
#include <random>
#include <math.h>

namespace Generator
{
    template <class T>
    class ErlangDistribution {
    public:
        static T Distribute(T alpha, T beta = 1.0f) {
            T d = alpha - 1.0f / 3.0f;
            T c = 1.0f / sqrt(9.0f * d);
            T z = 0.0f;
            while (true) {
                T x = 0.0f;
                T v = 0.0f;
                do {
                    x = RandomNormal(0, 1);
                    v = 1.0f + c * x;
                } while (v <= 0);
                v = pow(v, 3.0f);
                T u = RandomUniform(0.0f, 1.0f);
                if (u < 1.0f - 0.0331f * pow(x, 4.0f)) {
                    z = d * v;
                    break;
                }
                if (log(u) < 0.5 * pow(x, 2.0f) + d * (1 - v + log(v))) {
                    z = d * v;
                    break;
                }
            }
            return z / beta;
        }
    private:
        static T RandomUniform(T a, T b) {
            T u = (T)rand() / (T)RAND_MAX;
            return a + u * (b - a);
        }

        static T RandomNormal(T mu, T sigma) {
            T u1 = RandomUniform((T)0, (T)1);
            T u2 = RandomUniform((T)0, (T)1);
            T z0 = sqrt(-2 * log(u1)) * sin(2 * M_PI * u2);
            return mu + z0 * sigma;
        }
    };
}

#endif /* C79B92EF_2EA5_43C7_A33B_166288190D1E */
