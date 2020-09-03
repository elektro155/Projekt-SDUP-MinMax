/*
 * main.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <stdlib.h> //atoi
#include <limits.h>
#include <stdbool.h>
#include "platform.h"
#include "xil_printf.h"


#define UNUSED(x) (void)(x)

#define CALCULATE_HORNER_IN_FPGA 	(1)

#define DEGREE_OF_A_POLYNOMINAL     5U
#define RANGE                       100U
#define FINE_RANGE                  1.0f

int32_t calculateHornerValInFPGA(s32 arg, s32 cn0, s32 cn1, s32 cn2, s32 cn3, s32 cn4, s32 cn5);

void showPolynominal(const int8_t * polynominal_constants)
{
	xil_printf("\r\n\r\nPolynominal:\r\n");
    for(uint8_t a = 0; a <= DEGREE_OF_A_POLYNOMINAL ; ++a)
    {
        if(polynominal_constants[a] >= 0)
        {
            if(a != 0) xil_printf("+");
            xil_printf("%dx^%d", polynominal_constants[a], DEGREE_OF_A_POLYNOMINAL - a);
        }
        else
        {
        	xil_printf("-%dx^%d", -polynominal_constants[a], DEGREE_OF_A_POLYNOMINAL - a);
        }
    }
    xil_printf("\r\n\r\n");
}

bool numberInRange(const int a)
{
    bool retVal = false;
    if(a < 11 && a > -11) retVal = true;
    return retVal;
}

int getnumber()
{
    char str[100];
    for(int a = 0; a < 100; a++)
    {
        str[a] = inbyte();
        if(str[a] == '\r')
        {
            str[a] = 0U; //NULL
            break;
        }
    }

    return atoi(str);
}

void enterParams(int8_t * polynominal_constants)
{
    for(int a = DEGREE_OF_A_POLYNOMINAL; a >= 0  ; a--)
    {
        bool done = false;
        while(!done)
        {
        	xil_printf("enter value for x^%d", a);
            int value = getnumber();

            if(numberInRange(value))
            {
                polynominal_constants[DEGREE_OF_A_POLYNOMINAL - a] = value;
                done = true;
            }
            else
            {
            	xil_printf("wrong value");
            }
        }
    }
}

int32_t calc_poly_rough_int(const int8_t * constants, const int32_t argument)
{
#if CALCULATE_HORNER_IN_FPGA
	return calculateHornerValInFPGA((s32)argument, (s32)constants[0], (s32)constants[1], (s32)constants[2], (s32)constants[3], (s32)constants[4], (s32)constants[5]);
#else
    //Horner's method
    int32_t retVal = (int32_t)constants[0];
    for(uint8_t a = 1; a < (DEGREE_OF_A_POLYNOMINAL + 1); a++)
    {
        retVal = retVal * argument + (int32_t)constants[a];
    }
    return retVal;
#endif
}

float calc_poly_fine_float(const int8_t * constants, const float argument)
{
    //Horner's method
    float retVal = (float)constants[0];
    for(uint8_t a = 1; a < (DEGREE_OF_A_POLYNOMINAL + 1); a++)
    {
        retVal = retVal * argument + (float)constants[a];
    }
    return retVal;
}

void calculate_min_max(const int8_t * polynominal_constants, float* min, float * max)
{
    const uint16_t values_array_size = 2*RANGE + 1;
    int32_t values[values_array_size];
    int32_t minVal = INT_MAX;
    int32_t maxVal = INT_MIN;
    int32_t arg_at_min = 0U;
    int32_t arg_at_max = 0U;

    //calculate rough integer values for polynominal arguments and find integer min and max
    for(int a = 0; a < values_array_size; a++)
    {
        int32_t arg = a - RANGE;
        values[a] = calc_poly_rough_int(polynominal_constants, arg);
        //xil_printf("retVal: %d at argument: %d\r\n", values[a], arg);

        //get min and max
        if(values[a] < minVal)
        {
            arg_at_min = arg; //new min argument
            minVal = values[a]; //new min value
        }
        if(values[a] > maxVal)
        {
            arg_at_max = arg; //new max argument
            maxVal = values[a]; //new max value
        }
    }

    //printf("min arg int: %d max arg int: %d\n", arg_at_min, arg_at_max);

    //calculate fine values for minimum and maximum, step 0.01, check values in +/- 1 scope for given minimum and maximum arguments
    const uint16_t values_fine_array_size = 201U; //+/- 100 steps and zero
    float values_fine[values_fine_array_size];
    float minVal_fine = (float)minVal;
    float maxVal_fine = (float)maxVal;
    float min_fine_arg = 0.0f;
    float max_fine_arg = 0.0f;
    float arg = 0.0f;

    //minimum
    arg = (float)arg_at_min - FINE_RANGE;
    for(int a = 0; a < values_fine_array_size; a++)
    {
        values_fine[a] = calc_poly_fine_float(polynominal_constants, arg);

        //get fine min
        if(values_fine[a] < minVal_fine)
        {
            minVal_fine = values_fine[a];
            min_fine_arg = arg;
        }

        arg += 0.01f;
    }

    //maximum
    arg = (float)arg_at_max - FINE_RANGE;
    for(int a = 0; a < values_fine_array_size; a++)
    {
        values_fine[a] = calc_poly_fine_float(polynominal_constants, arg);

        //get fine max
        if(values_fine[a] > maxVal_fine)
        {
            maxVal_fine = values_fine[a];
            max_fine_arg = arg;
        }

        arg += 0.01f;
    }

    UNUSED(min_fine_arg);
    UNUSED(max_fine_arg);
    //printf("min arg fine: %3.2f max arg fine: %3.2f\n", min_fine_arg, max_fine_arg);

    *min = minVal_fine;
    *max = maxVal_fine;
    //*min = (float) minVal;
    //*max = (float) maxVal;
}

int main()
{
	init_platform();

    int8_t polynominal_constants[DEGREE_OF_A_POLYNOMINAL + 1U] = {0, 1, -4, 2, 0, 1}; //example values of constants for polynominal f(x)= x^4 - 4*x^3 + 2*x^2 + 1
    float min = 0.0f; float max = 0.0f;

    while (1)
    {
        enterParams(polynominal_constants);

        showPolynominal(polynominal_constants);

        xil_printf("CALCULATING... \r\n");

        calculate_min_max(polynominal_constants, &min, &max);

        printf("DONE\n\nMIN:%4.2f\nMAX:%4.2f\n\n", min, max);

        xil_printf("\r\nEnter any value to continue\r\n");
        (void)getnumber();
    }
}
