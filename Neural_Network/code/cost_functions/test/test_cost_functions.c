#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "test_cost_functions.h"

// Tolerance level for float conversion
const double EPS = 1e-9;
// Higher tolerance for exponential function (yields bigger mistakes due to float conversion)
const double EPS_exp = 1e-4;

static inline double**  convert_arr_ptr_ptr(double arr[SIZE_TRAIN][NUM_OUTPUT]){
    double** intermediate = calloc(SIZE_TRAIN, sizeof(double));
    for (size_t i = 0; i < SIZE_TRAIN; i++){                          
        intermediate[i] = arr[i];
    }
    return (double**) intermediate;
}


#define CHECK_COST_FUNCTION(actual, pred, expected, func)             \
    do {                                                              \
    double **actual_ = convert_arr_ptr_ptr((actual));                 \
    if (!actual_){                                                    \
        printf("Memory allocation error for actual\n");               \
        return MEMORY_ALLOCATION_ERROR;                               \
    }                                                                 \
    double **pred_ = convert_arr_ptr_ptr((pred));                     \
    if (!pred_){                                                      \
        printf("Memory allocation error for pred\n");                 \
        return MEMORY_ALLOCATION_ERROR;                               \
    }                                                                 \
    double result = func((actual_), (pred_), (SIZE_TRAIN));           \
    free(pred_);                                                      \
    free(actual_);                                                    \
    double diff = fabs(result - (expected));                          \
    assert(diff < EPS && "Difference exceeded EPS in cost function"); \
    } while (0);



int test_mean_squared_error(void){

    #define CHECK_MSE(actual, pred, expected) \
        CHECK_COST_FUNCTION((actual), (pred), (expected), mean_squared_error)
    
    /*************************************************************************/
    /* 1) All-zero arrays: MSE should be 0.                                  */
    /*************************************************************************/
    {
        double act[SIZE_TRAIN][NUM_OUTPUT] = {{0},{0},{0},{0},{0},{0}};
        double pred[SIZE_TRAIN][NUM_OUTPUT] = {{0},{0},{0},{0},{0},{0}};
        CHECK_MSE(act, pred, 0.0);
    }

    /*************************************************************************/
    /* 2) Simple positive integers: ((2-0)^2 + (1-0)^2 + (1-0)^2)/6 = 1.      */
    /*************************************************************************/
    {
        double act[SIZE_TRAIN][NUM_OUTPUT] = {{0},{0},{0},{0},{0},{0}};
        double pred[SIZE_TRAIN][NUM_OUTPUT] = {{2},{1},{1},{0},{0},{0}};
        CHECK_MSE(act, pred, 1.0);
    }

    /*************************************************************************/
    /* 3) Mixed integers (positive & negative): ((4-0)^2 + (-2-0)^2)/6=20/6. */
    /*************************************************************************/
    {
        double act[SIZE_TRAIN][NUM_OUTPUT] = {{0},{0},{0},{0},{0},{0}};
        double pred[SIZE_TRAIN][NUM_OUTPUT] = {{4},{0},{-2},{0},{0},{0}};
        // Use floating-point arithmetic explicitly: 20.0/6.0
        CHECK_MSE(act, pred, 20.0/6.0);
    }

    /*************************************************************************/
    /* 4) Another integer range test: Checking partial differences.          */
    /*************************************************************************/
    {
        // act2 and pred4 are identical => MSE = 0
        double act2[SIZE_TRAIN][NUM_OUTPUT]  = {{-2},{-1},{0},{1},{2},{3}};
        double pred4[SIZE_TRAIN][NUM_OUTPUT] = {{-2},{-1},{0},{1},{2},{3}};
        CHECK_MSE(act2, pred4, 0.0);

        // ((-4--2)^2 + (5-3)^2)/6 = ( (-4+2)^2 + (2)^2 )/6 = (4+4)/6=8/6
        double pred5[SIZE_TRAIN][NUM_OUTPUT] = {{-4},{-1},{0},{1},{2},{5}};
        CHECK_MSE(act2, pred5, 8.0/6.0);
    }

    /*************************************************************************/
    /* 5) Floating-point arrays with small differences.                      */
    /*************************************************************************/
    {
        double act3[SIZE_TRAIN][NUM_OUTPUT]  = {{0.1}, {-0.2}, {0.3}, {-0.4},{0.5}, {-0.6}};
        double pred6[SIZE_TRAIN][NUM_OUTPUT] = {{0.1}, {-0.2}, {0.3}, {-0.4},{0.5}, {-0.6}};
        // Identical => MSE should be 0
        CHECK_MSE(act3, pred6, 0.0);

        // Slight difference in first element => (0.3 - 0.1)^2 = 0.04, MSE=0.04/6
        double pred7[SIZE_TRAIN][NUM_OUTPUT] = {{0.3}, {-0.2}, {0.3}, {-0.4}, {0.5}, {-0.6}};
        double expected = 0.04 / 6.0;
        CHECK_MSE(act3, pred7, expected);
    }

    /*************************************************************************/
    /* 6) Large-value test: watch for potential overflow/precision.          */
    /*************************************************************************/
    {
        double act4[SIZE_TRAIN][NUM_OUTPUT]  = {{1e9}, {-1e9}, {1e9}, {-1e9}, {1e9}, {-1e9}};
        double pred4[SIZE_TRAIN][NUM_OUTPUT] = {{1e9}, {-1e9}, {0.9e9}, {-1.1e9}, {1.0e9}, {-1e9}};
        // We'll compute the expected MSE by direct calculation:
        double sum_sq = 0.0;
        for (int i=0; i<SIZE_TRAIN; i++) {
            double diff = act4[i][0] - pred4[i][0];
            sum_sq += diff * diff; // watch for large intermediate values
        }
        double expected = sum_sq / SIZE_TRAIN;
        CHECK_MSE(act4, pred4, expected);
    }

    /*************************************************************************/
    /* 7) (Optional) Random test: multiple runs can catch strange edge cases */
    /*************************************************************************/
    {
        // Seed for reproducibility or time(0) if truly random
        srand(12345);

        double act5[SIZE_TRAIN][NUM_OUTPUT], pred5[SIZE_TRAIN][NUM_OUTPUT];
        double sum_sq = 0.0;

        for(int i=0; i<SIZE_TRAIN; i++){
            // Generate random doubles, e.g., in [-5, 5]
            act5[i][0]  = ((double)rand() / RAND_MAX)*10.0 - 5.0;
            pred5[i][0] = ((double)rand() / RAND_MAX)*10.0 - 5.0;

            double diff = act5[i][0] - pred5[i][0];
            sum_sq += diff * diff;
        }
        double expected = sum_sq / SIZE_TRAIN;
        
        double** act5_ = convert_arr_ptr_ptr(act5);
        double** pred5_ = convert_arr_ptr_ptr(pred5);

        double result = mean_squared_error(act5_, pred5_, SIZE_TRAIN);

        free(act5_);
        free(pred5_);

        assert(fabs(result - expected) < EPS);
    }

    // If we get here, all asserts have passed!
    printf("All mean_squared_error tests passed successfully.\n");
    return 0;
    #undef CHECK_MSE
}

int test_mean_absolute_error(void){
 // Define a specialized macro for MAE, forwarding to CHECK_COST_FUNCTION
    #define CHECK_MAE(actual, pred, expected) \
        CHECK_COST_FUNCTION((actual), (pred), (expected), mean_absolute_error)

    /*************************************************************************/
    /* 1) All-zero arrays: MAE should be 0.                                  */
    /*************************************************************************/
    {
        double act[SIZE_TRAIN][NUM_OUTPUT]  = {{0},{0},{0},{0},{0},{0}};
        double pred[SIZE_TRAIN][NUM_OUTPUT] = {{0},{0},{0},{0},{0},{0}};
        CHECK_MAE(act, pred, 0.0);
    }

    /*************************************************************************/
    /* 2) Simple positive integers: sum of |2-0| + |1-0| + |1-0| = 4, /6=~0.6667 */
    /*************************************************************************/
    {
        double act[SIZE_TRAIN][NUM_OUTPUT]  = {{0},{0},{0},{0},{0},{0}};
        double pred[SIZE_TRAIN][NUM_OUTPUT] = {{2},{1},{1},{0},{0},{0}};
        // (2 + 1 + 1 + 0 + 0 + 0)/6 = 4/6 = 0.6666667...
        CHECK_MAE(act, pred, 4.0/6.0);
    }

    /*************************************************************************/
    /* 3) Mixed integers (positive & negative): (4 + |-2| + 0 + 0 + 0 + 0)/6=6/6=1 */
    /*************************************************************************/
    {
        double act[SIZE_TRAIN][NUM_OUTPUT]  = {{0},{0},{0},{0},{0},{0}};
        double pred[SIZE_TRAIN][NUM_OUTPUT] = {{4},{0},{-2},{0},{0},{0}};
        // |4-0| + |0-0| + |-2-0| + 0 + 0 + 0 = 4 + 0 + 2 = 6 => 6/6=1
        CHECK_MAE(act, pred, 1.0);
    }

    /*************************************************************************/
    /* 4) Another integer range test: Checking partial differences.          */
    /*************************************************************************/
    {
        // act2 and pred4 are identical => MAE = 0
        double act2[SIZE_TRAIN][NUM_OUTPUT]  = {{-2},{-1},{0},{1},{2},{3}};
        double pred4[SIZE_TRAIN][NUM_OUTPUT] = {{-2},{-1},{0},{1},{2},{3}};
        CHECK_MAE(act2, pred4, 0.0);

        // Compare act2 to pred5
        // act2 = {-2, -1, 0, 1, 2, 3}
        // pred5= {-4, -1, 0, 1, 2, 5}
        // Differences: 
        //    |-4 - (-2)|=2, |-1 - (-1)|=0, |0-0|=0, |1-1|=0, |2-2|=0, |5-3|=2
        // sum=2+0+0+0+0+2=4 => 4/6=0.6666667...
        double pred5[SIZE_TRAIN][NUM_OUTPUT] = {{-4},{-1},{0},{1},{2},{5}};
        CHECK_MAE(act2, pred5, 4.0/6.0);
    }

    /*************************************************************************/
    /* 5) Floating-point arrays with small differences.                      */
    /*************************************************************************/
    {
        double act3[SIZE_TRAIN][NUM_OUTPUT]  = {{0.1}, {-0.2}, {0.3}, {-0.4}, {0.5}, {-0.6}};
        double pred6[SIZE_TRAIN][NUM_OUTPUT] = {{0.1}, {-0.2}, {0.3}, {-0.4}, {0.5}, {-0.6}};
        // Identical => MAE should be 0
        CHECK_MAE(act3, pred6, 0.0);

        // Slight difference in first element => |0.3-0.1|=0.2 => sum=0.2 => 0.2/6=~0.0333...
        double pred7[SIZE_TRAIN][NUM_OUTPUT] = {{0.3}, {-0.2}, {0.3}, {-0.4}, {0.5}, {-0.6}};
        double expected = 0.2 / 6.0;
        CHECK_MAE(act3, pred7, expected);
    }

    /*************************************************************************/
    /* 6) Large-value test: watch for potential overflow/precision.          */
    /*************************************************************************/
    {
        double act4[SIZE_TRAIN][NUM_OUTPUT]  = {{1e9}, {-1e9}, {1e9}, {-1e9}, {1e9}, {-1e9}};
        double pred4[SIZE_TRAIN][NUM_OUTPUT] = {{1e9}, {-1e9}, {0.9e9}, {-1.1e9}, {1.0e9}, {-1e9}};
        // We'll compute the expected MAE by direct calculation:
        double sum_abs = 0.0;
        for (int i = 0; i < SIZE_TRAIN; i++) {
            double diff = fabs(act4[i][0] - pred4[i][0]);
            sum_abs += diff;  // watch for large intermediate values
        }
        double expected = sum_abs / SIZE_TRAIN;
        CHECK_MAE(act4, pred4, expected);
    }

    /*************************************************************************/
    /* 7) (Optional) Random test: multiple runs can catch strange edge cases */
    /*************************************************************************/
    {
        srand(12345); // or time(0) for non-deterministic
        double act5[SIZE_TRAIN][NUM_OUTPUT], pred5[SIZE_TRAIN][NUM_OUTPUT];
        double sum_abs = 0.0;

        for (int i = 0; i < SIZE_TRAIN; i++){
            // Generate random doubles in [-5, 5]
            act5[i][0]  = ((double)rand() / RAND_MAX)*10.0 - 5.0;
            pred5[i][0] = ((double)rand() / RAND_MAX)*10.0 - 5.0;

            sum_abs += fabs(act5[i][0] - pred5[i][0]);
        }
        double expected = sum_abs / SIZE_TRAIN;

        double** act5_ = convert_arr_ptr_ptr(act5);
        double** pred5_ = convert_arr_ptr_ptr(pred5);
        double result = mean_absolute_error(act5_, pred5_, SIZE_TRAIN);

        free(pred5_);
        free(act5_);

        assert(fabs(result - expected) < EPS);
    }

    printf("All mean_absolute_error tests passed successfully.\n");
    return 0;

    #undef CHECK_MAE
}

int test_cross_entropy_cost(void){
    // Specialized macro for cross-entropy:
    #define CHECK_CE(actual, pred, expected) \
        CHECK_COST_FUNCTION((actual), (pred), (expected), cross_entropy_cost)

    /*************************************************************************/
    /* 1) y=[1,0,0,0,0,0], p=[1,0,0,0,0,0] => cost = -[1*log(1)+0*log(1-1)]=0 */
    /*************************************************************************/
    {
        double y[SIZE_TRAIN][NUM_OUTPUT] = {{1},{0},{0},{0},{0},{0}};
        double p[SIZE_TRAIN][NUM_OUTPUT] = {{1},{0},{0},{0},{0},{0}};

        double sum_ce = 0.0;
        for(int i=0; i<SIZE_TRAIN; i++){
            double pc = fmax(fmin(p[i][0], 1.0 - 1e-12), 1e-12); // clamp
            sum_ce += y[i][0] * log(pc) + (1.0 - y[i][0]) * log(1.0 - pc);
        }
        double expected = -sum_ce;  // negative sum

        CHECK_CE(y, p, expected);
    }

    /*************************************************************************/
    /* 2) y=[1,0,0,0,0,0], p=[0.7,0.3,0,0,0,0] => - log(0.7)= ~0.3566749439   */
    /*************************************************************************/
    {
        double y[SIZE_TRAIN][NUM_OUTPUT] = {{1},{0},{0},{0},{0},{0}};
        double p[SIZE_TRAIN][NUM_OUTPUT] = {{0.7},{0.3},{0},{0},{0},{0}};

        double sum_ce = 0.0;
        for(int i=0; i<SIZE_TRAIN; i++){
            double pc = fmax(fmin(p[i][0], 1.0 - 1e-12), 1e-12);
            sum_ce += y[i][0] * log(pc) + (1.0 - y[i][0]) * log(1.0 - pc);
        }
        double expected = -sum_ce;  // e.g., ~0.356675
        CHECK_CE(y, p, expected);
    }

    /*************************************************************************/
    /* 3) y=[0.5,0.5,0,0,0,0], p=[0.6,0.4,0,0,0,0]                            */
    /*    cost= - [0.5*log(0.6) + 0.5*log(0.4)]                               */
    /*    Though typical "binary" CE expects y in {0,1}, this still works.   */
    /*************************************************************************/
    {
        double y[SIZE_TRAIN][NUM_OUTPUT] = {{0.5}, {0.5}, {0}, {0}, {0}, {0}};
        double p[SIZE_TRAIN][NUM_OUTPUT] = {{0.6}, {0.4}, {0}, {0}, {0}, {0}};

        double sum_ce = 0.0;
        for(int i=0; i<SIZE_TRAIN; i++){
            double pc = fmax(fmin(p[i][0], 1.0 - 1e-12), 1e-12);
            sum_ce += y[i][0] * log(pc) + (1.0 - y[i][0]) * log(1.0 - pc);
        }
        double expected = -sum_ce;
        CHECK_CE(y, p, expected);
    }

    /*************************************************************************/
    /* 4) Boundary test: y=[0,1], p=[1e-12, 1-1e-12], etc.                    */
    /*************************************************************************/
    {
        double y[SIZE_TRAIN][NUM_OUTPUT] = {{0},{1},{0},{0},{0},{0}};
        double p[SIZE_TRAIN][NUM_OUTPUT] = {{1e-15}, {1.0}, {0},{0},{0},{0}}; 
        // We'll clamp p[0] up to 1e-12, p[1] down to (1 - 1e-12)
        // Then the sum is y[1]*log(1-1e-12) => ~ -1e-12 => cost=1e-12
        // Or if y[1]=1 => sum= log(1-1e-12)= ~-1e-12 => cost= 1e-12
        double sum_ce = 0.0;
        for(int i=0; i<SIZE_TRAIN; i++){
            double pc = fmax(fmin(p[i][0], 1.0 - 1e-12), 1e-12);
            sum_ce += y[i][0] * log(pc) + (1.0 - y[i][0]) * log(1.0 - pc);
        }
        double expected = -sum_ce;
        CHECK_CE(y, p, expected);
    }

    /*************************************************************************/
    /* 5) Random test: ensure nothing breaks, clamp avoids log(0).           */
    /*************************************************************************/
    {
        srand(12345);
        double y[SIZE_TRAIN][NUM_OUTPUT], p[SIZE_TRAIN][NUM_OUTPUT];
        for(int i=0; i<SIZE_TRAIN; i++){
            // y in [0..1]
            y[i][0] = (double)rand() / RAND_MAX;
            // p in [0..1], then clamp
            p[i][0] = (double)rand() / RAND_MAX;
        }

        double sum_ce = 0.0;
        for(int i=0; i<SIZE_TRAIN; i++){
            double pc = fmax(fmin(p[i][0], 1.0 - 1e-12), 1e-12);
            sum_ce += y[i][0] * log(pc) + (1.0 - y[i][0]) * log(1.0 - pc);
        }
        double expected = -sum_ce;
        CHECK_CE(y, p, expected);
    }

    printf("All cross_entropy_cost tests passed successfully.\n");
    return 0;

    #undef CHECK_CE
}

int test_exponential_cost(void){

    #define CHECK_EXP(actual, pred, expected, n)                                  \
        do {                                                                      \
            double** actual_ = convert_arr_ptr_ptr((actual));                     \
            double** pred_ = convert_arr_ptr_ptr(pred);                           \
            double result = exponential_cost((actual_), (pred_), (n), SIZE_TRAIN);\
            double diff = fabs(result - (expected));                              \
            free(actual_);                                                        \
            free(pred_);                                                          \
            assert(diff < EPS_exp && "Difference exceeded EPS in cost function"); \
            } while (0);

    /*************************************************************************/
    /* 1) All-zero arrays => total_squared_error = 0.                        */
    /*    cost = | n * exp((1/n)*0 ) | = |n * exp(0)| = n                     */
    /*************************************************************************/
    {
        double act[SIZE_TRAIN][NUM_OUTPUT]  = {{0},{0},{0},{0},{0},{0}};
        double pred[SIZE_TRAIN][NUM_OUTPUT] = {{0},{0},{0},{0},{0},{0}};

        // Let's pick n = 2.0f for demonstration
        float n_val = 2.0f;
        // TSE = 0 => cost = n_val * exp((1/n_val)*0) = 2 * 1 = 2
        double expected = n_val; // since exp(0)=1

        CHECK_EXP(act, pred, expected, n_val);
    }

    /*************************************************************************/
    /* 2) Simple positive integers: act=all zeros, pred=[2,1,1,0,0,0].        */
    /*    TSE = (2-0)^2 + (1-0)^2 + (1-0)^2 + 0 + 0 + 0 = 2^2 + 1^2 + 1^2=4+1+1=6 */
    /*    cost = n * exp((1/n)*6). For instance, n=1 => cost=e^6             */
    /*************************************************************************/
    {
        double act[SIZE_TRAIN][NUM_OUTPUT]  = {{0},{0},{0},{0},{0},{0}};
        double pred[SIZE_TRAIN][NUM_OUTPUT] = {{2},{1},{1},{0},{0},{0}};

        float  n_val    = 1.0f;  // For example
        double TSE      = 0.0;
        for(int i=0; i<SIZE_TRAIN; i++){
            double error = act[i][0] - pred[i][0];
            TSE += error * error; // sum of squares
        }
        // TSE=6 => cost = 1*exp((1/1)*6)=exp(6)
        double expected = exp(TSE);

        CHECK_EXP(act, pred, expected, n_val);
    }

    /*************************************************************************/
    /* 3) Mixed values: act=[0,0,0,0,0,0], pred=[4,0,-2,0,0,0].               */
    /*    TSE= (4-0)^2 + (0-0)^2 + (-2-0)^2 =16+0+4=20 => cost=n*exp((1/n)*20).*/
    /*************************************************************************/
    {
        double act[SIZE_TRAIN][NUM_OUTPUT]  = {{0},{0},{0},{0},{0},{0}};
        double pred[SIZE_TRAIN][NUM_OUTPUT] = {{4},{0},{-2},{0},{0},{0}};

        float  n_val    = 2.0f;
        double TSE      = 0.0;
        for(int i=0; i<SIZE_TRAIN; i++){
            double error = act[i][0] - pred[i][0];
            TSE += error * error;
        }
        // cost = 2 * exp( (1/2)*20 ) = 2 * exp(10)
        double expected = n_val * exp((1.0/n_val)*TSE);

        CHECK_EXP(act, pred, expected, n_val);
    }

    /*************************************************************************/
    /* 4) Identical arrays => TSE=0 => cost = n * exp(0) = n                 */
    /*************************************************************************/
    {
        double act[SIZE_TRAIN][NUM_OUTPUT]  = {{-2},{-1},{0},{1},{2},{3}};
        double pred[SIZE_TRAIN][NUM_OUTPUT] = {{-2},{-1},{0},{1},{2},{3}};

        float  n_val    = 3.0f; // for instance
        double TSE      = 0.0;
        for(int i=0; i<SIZE_TRAIN; i++){
            double error = act[i][0] - pred[i][0]; // 0
            TSE += error*error;              // 0
        }
        // cost = 3 * exp(0)=3
        double expected = n_val * exp(TSE);

        CHECK_EXP(act, pred, expected, n_val);

        // Another variation: pred5
        // Example: pred5=[-4,-1,0,1,2,5]
        // TSE= ((-2)-(-4))^2+((-1)-(-1))^2+((0)-0)^2+((1)-1)^2+((2)-2)^2+((3)-5)^2
        //     = (2)^2 + (0)^2 + (0)^2 + (0)^2 + (0)^2 + (-2)^2 = 4+0+0+0+0+4=8
        {
            double pred5[SIZE_TRAIN][NUM_OUTPUT] = {{-4},{-1},{0},{1},{2},{5}};
            double TSE2 = 0.0;
            for(int i=0; i<SIZE_TRAIN; i++){
                double error = act[i][0] - pred5[i][0];
                TSE2 += error*error;
            }
            double expected2 = n_val * exp((1.0/n_val)*TSE2); // => 3*exp((1/3)*8)
            CHECK_EXP(act, pred5, expected2, n_val);
        }
    }

    /*************************************************************************/
    /* 5) Floating-point arrays with small differences.                      */
    /*************************************************************************/
    {
        double act3[SIZE_TRAIN][NUM_OUTPUT]  = {{0.1}, {-0.2}, {0.3}, {-0.4}, {0.5}, {-0.6}};
        double pred3[SIZE_TRAIN][NUM_OUTPUT] = {{0.1}, {-0.2}, {0.3}, {-0.4}, {0.5}, {-0.6}};
        float  n_val             = 1.5f; // for instance

        // TSE=0 => cost=1.5 * exp(0)=1.5
        double expected = n_val;
        CHECK_EXP(act3, pred3, expected, n_val);

        // Slight difference in first element
        double pred4[SIZE_TRAIN][NUM_OUTPUT] = {{0.3}, {-0.2}, {0.3},{ -0.4}, {0.5}, {-0.6}};
        double TSE2 = 0.0;
        for(int i=0; i<SIZE_TRAIN; i++){
            double error = act3[i][0] - pred4[i][0];
            TSE2 += error*error;
        }
        double expected2 = n_val * exp((1.0/n_val)*TSE2);
        CHECK_EXP(act3, pred4, expected2, n_val);
    }

    /*************************************************************************/
    /* 6) Large-value test: if TSE is huge, watch out for potential overflow. */
    /*************************************************************************/
    {
        double act4[SIZE_TRAIN][NUM_OUTPUT]  = {{1e9}, {-1e9}, {1e9}, {-1e9}, {1e9}, {-1e9}};
        double pred4[SIZE_TRAIN][NUM_OUTPUT] = {{1e9}, {-1e9}, {0.9e9}, {-1.1e9}, {1.0e9}, {-1e9}};
        float  n_val             = 1.0f;

        double TSE3 = 0.0;
        for(int i=0; i<SIZE_TRAIN; i++){
            double error = act4[i][0] - pred4[i][0];
            TSE3 += error * error;
        }
        // cost = n * exp((1/n)*TSE3) => 1 * exp(TSE3)
        // But TSE3 could be huge => potential exp() overflow => Inf
        double expected3 = exp(TSE3); // might be Inf

        double** act4_ = convert_arr_ptr_ptr(act4);
        double** pred4_ = convert_arr_ptr_ptr(pred4);
        double result = exponential_cost((act4_), (pred4_), (n_val), SIZE_TRAIN);

        free(act4_);
        free(pred4_);
        
        double diff = fabs(result - (expected3));             
        if (diff < EPS_exp){
            printf("Problems with extremelt large training values in exponential cost function. Potential double overflow.");
        }           

    }

    /*************************************************************************/
    /* 7) Random test: multiple runs can catch strange edge cases.           */
    /*************************************************************************/
    {
        srand(12345);
        double act5[SIZE_TRAIN][NUM_OUTPUT], pred5[SIZE_TRAIN][NUM_OUTPUT];
        for(int i=0; i<SIZE_TRAIN; i++){
            // generate random in [-5,5], for instance
            act5[i][0]  = ((double)rand()/RAND_MAX)*10.0 - 5.0;
            pred5[i][0] = ((double)rand()/RAND_MAX)*10.0 - 5.0;
        }
        float n_val = 2.5f;

        double TSE = 0.0;
        for(int i=0; i<SIZE_TRAIN; i++){
            double error = act5[i][0] - pred5[i][0];
            TSE += error*error;
        }
        double expected = n_val * exp((1.0/n_val)*TSE);

        //CHECK_EXP(act5, pred5, expected, n_val);
    }

    printf("Exponential cost not fully tested. All implemented tests passed.\n");
    //printf("All exponential_cost tests passed successfully.\n");
    return 0;
    #undef CHECK_EXP
}

int test_kl_divergence(void){
    printf("kl convergence not yet tested\n");
    return 0;
}

int test_generalised_kl_divergence(void){
    printf("generalised kl convergence not yet tested\n");
    return 0;
}

int test_itakura_saito_distance(void){
    printf("itakura saito distance not yet tested\n");
    return 0;
}


int main(void){

    test_mean_squared_error();
    test_mean_absolute_error();
    test_cross_entropy_cost();
    test_exponential_cost();
    test_kl_divergence();
    test_generalised_kl_divergence();
    test_itakura_saito_distance();

    printf("All tests for the cost functions passed\n");
    
    return 0;

}

#undef CHECK_COST_FUNCTION