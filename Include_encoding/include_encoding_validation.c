/* Inference Include Encoding Tsetlin Machine
   ---------------------------------------
   You will need: Validation dataset (txt)
                  include encoded file of TAs  (txt)
   
   Created by Sidharth Maheshwari and Tousif Rahman
   Microsystems Group Newcastle University
   November 2021
   ---------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // used for file reading 
#include <stdbool.h> // could not be avoided :(
#include <sys/resource.h>
#include "IncludeEncodedSetup.h"

#define INFER_LENGTH TEST_LENGTH_PER32*FEATURES

//Globals
unsigned int inference_literals[INFER_LENGTH];
unsigned char y[NUMBER_OF_TEST_EXAMPLES];
unsigned char infered_classes[TEST_LENGTH_PER32*32];
unsigned short IncEncTA[NUM_OF_INCLUDES];
unsigned short INC_per_CLASS[CLASSES];

void get_literals(){
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	const char *s = " ";
	char *token = NULL;

	for (int i = 0; i < INFER_LENGTH; i++) {
		inference_literals[i] = 0;
	}
	fp = fopen(TEST_FEATURE_FILE, "r");
	if (fp == NULL) {
		printf("Error opening %s \n", TEST_FEATURE_FILE);
		exit(EXIT_FAILURE);
	}
		
	for (int j = 0; j < INFER_LENGTH; j++) 
	{
      	getline(&line, &len, fp);
		token = strtok(line, s);
		inference_literals[j] = atoi(token);  
		token=strtok(NULL,s);
	}
	fclose(fp);

	fp = fopen(TEST_CLASSIFICATION_FILE, "r");
	if (fp == NULL) {
		printf("Error opening %s \n", TEST_CLASSIFICATION_FILE);
		exit(EXIT_FAILURE);
	}

	
	for (int j = 0; j < NUMBER_OF_TEST_EXAMPLES; j++) 
	{
      	getline(&line, &len, fp);
		token = strtok(line, s);
		y[j] = atoi(token);  
		token=strtok(NULL,s);
	}
	fclose(fp);
}

void get_include_encoded_TAs(){
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	const char *s = "";
	char *token = NULL;
	fp = fopen(INCENC_TA_FILE, "r");
	if (fp == NULL) {
		printf("Error opening %s \n", INCENC_TA_FILE);
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < NUM_OF_INCLUDES; i++) {
		getline(&line, &len, fp);
		token = strtok(line, s);
		IncEncTA[i] = atoi(token);
		token=strtok(NULL,s);   
	}
	fclose(fp);
}

void get_include_per_class(){
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	const char *s = "";
	char *token = NULL;
	fp = fopen(INC_PER_CLASS_FILE, "r");
	if (fp == NULL) {
		printf("Error opening %s \n", INC_PER_CLASS_FILE);
		exit(EXIT_FAILURE);
	}
	for(int i = 0; i < CLASSES; i++) {
		getline(&line, &len, fp);
		token = strtok(line, s);
		INC_per_CLASS[i] = atoi(token);
		token=strtok(NULL,s);   
	}
	fclose(fp);
}

void inference()
{
   // Iterorate through the TA states
	int class_sum[32], cur_class_sum[32];
	int i,j,k,m, clause_offset=0;
	int prev_clause_change, curr_clause_change, clause_polarity, literal_polarity, classification[32];
	unsigned int literal_offset, clause_output = 4294967295;  //Maximum 32-bit integer value
	int max = 0;
	for(k = 0; k < TEST_LENGTH_PER32; k++)
	{
		for(m=0;m<32;m++)
		{
			class_sum[m] = -2147483647; cur_class_sum[m] = 0; classification[m]=0;
		}
		clause_offset=0;
		for(i = 0; i < CLASSES; i++)
		{
			clause_output = 4294967295;			
			prev_clause_change = 0;
			curr_clause_change = 0;
			clause_polarity = 0;

			max = clause_offset+INC_per_CLASS[i];
			for(j = clause_offset; j < max; j++)
			{
				curr_clause_change = IncEncTA[j]>>BIT_DEMARCATING_CLAUSE_CHANGE;				
				
				if(curr_clause_change != prev_clause_change)
				{
					if(clause_polarity == 0)
					{
						for(m=0;m<32;m++)
						{	
							cur_class_sum[m] = cur_class_sum[m] + (int)((clause_output>>(31-m))&1);
						}
					}
					else
					{
						for(m=0;m<32;m++)
						{
							cur_class_sum[m] = cur_class_sum[m] - (int)((clause_output>>(31-m))&1);
						}
					}
					clause_output = 4294967295;
				}
				clause_polarity = IncEncTA[j]>>CLAUSE_POLARITY_BIT;
				literal_offset = (IncEncTA[j]&INT_VALUE_OF_ENCODING_BITS)>>1;
				literal_polarity = IncEncTA[j]&1;
				prev_clause_change = curr_clause_change;
				clause_output = (literal_polarity == 0)?(clause_output & inference_literals[(k*FEATURES)+literal_offset]):(clause_output & (~inference_literals[(k*FEATURES)+literal_offset]) );
			}
			clause_offset = clause_offset + INC_per_CLASS[i];
			for(m=0;m<32;m++)
			{
				if(class_sum[m] < cur_class_sum[m])
				{
					class_sum[m] = cur_class_sum[m];
					classification[m] = i;					
				}
				cur_class_sum[m] = 0;
			}

		}
		for(m=0;m<32;m++)
		{
			// printf("%d\n", class_sum[m]);
			infered_classes[(k*32) + m] = classification[m];
		}
	}
}


int main()
{
	struct rusage r_usage;
	printf("*---------------------------------------------------------------------*\n");
	printf("|\t\tTSETLIN MACHINE INFERENCE USING REDRESS APPROACH\t|\n");
	printf("*---------------------------------------------------------------------*\n");

	get_include_per_class();
	get_literals();
	get_include_encoded_TAs();

	// int classification; 
	int errors = 0,l=0,i=0;
	inference();

	if( (TEST_LENGTH_PER32*32) > NUMBER_OF_TEST_EXAMPLES )
	{
		for (l = 0; l < (TEST_LENGTH_PER32-1)*32; l++)
		{
			// printf("%d\n", l);	   
		   if (infered_classes[l] != y[l]) 
		   {
		      errors += 1;
		      // printf("classification = %d and actual class = %d\n", infered_classes[l], y[l]);
		   }
		}
		i = l;
		
		for(l = ((TEST_LENGTH_PER32*32)) - (NUMBER_OF_TEST_EXAMPLES - ((TEST_LENGTH_PER32-1)*32)); l < ((TEST_LENGTH_PER32*32) - 1) ; l++)
		{				   
			if (infered_classes[l] != y[i]) 
			{
				// printf("classification = %d and actual class = %d\n", infered_classes[l], y[i]);
				errors += 1;
			}
			i++;
		}
	}
	else
	{
		for (l = 0; l < (TEST_LENGTH_PER32)*32; l++)
		{			
			if (infered_classes[l] != y[l]) 
			{
				errors += 1;
				// printf("----classification = %d and actual class = %d\n", infered_classes[l], y[l]); 
			}
			// else
			// {
			// 	printf("classification = %d and actual class = %d\n", infered_classes[l], y[l]); 	
			// }
		}
	}

	printf("*-----------------------------------------------*\n");
	printf("|\tAcc            : %.4f\t             \t|\n", (float)(1.0 - 1.0 * errors / NUMBER_OF_TEST_EXAMPLES));
	printf("*-----------------------------------------------*\n");
	printf("Total erroneous classifications - %d\n", errors);
	return 0;
}