#include "MultiClassTsetlinMachine.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

int X_train[NUMBER_OF_EXAMPLES_TRAIN][FEATURES];
int y_train[NUMBER_OF_EXAMPLES_TRAIN];

int X_test[NUMBER_OF_EXAMPLES_TEST][FEATURES];
int y_test[NUMBER_OF_EXAMPLES_TEST];

void read_file(int prev_iter_clauses, unsigned short *prev_TAs, bool default_initilization, char *train_file, char *test_file, char *prev_TA_file)
{
	FILE * fp;
	char * line = NULL;
	size_t len = 0, temp=0;

	const char *s = " ";
	char *token = NULL;

	fp = fopen(train_file, "r");
	if (fp == NULL) {
		printf("Error opening\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < NUMBER_OF_EXAMPLES_TRAIN; i++) {
		temp=getline(&line, &len, fp);
		if(temp == -1)
		{
			printf("Could not read line in training dataset\n");
		}

		token = strtok(line, s);
		for (int j = 0; j < FEATURES; j++) {
			X_train[i][j] = atoi(token);
			token=strtok(NULL,s);
		}
		y_train[i] = atoi(token);
	}
	fclose(fp);

	fp = fopen(test_file, "r");
	if (fp == NULL) {
		printf("Error opening\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < NUMBER_OF_EXAMPLES_TEST; i++) {
		temp=getline(&line, &len, fp);
		if(temp == -1)
		{
			printf("Could not read line in testing dataset\n");
		}
		token = strtok(line, s);
		for (int j = 0; j < FEATURES; j++) {
			X_test[i][j] = atoi(token);
			token=strtok(NULL,s);
		}
		y_test[i] = atoi(token);
	}
	fclose(fp);

	if(default_initilization == 1)
	{
		fp = fopen(prev_TA_file, "r");
		if (fp == NULL) {
			printf(" TAs_standard.txt file not found in directory. Training will start with default initialization\n");
			exit(EXIT_FAILURE);
		}
		temp=getline(&line, &len, fp);
		if(temp == -1)
		{
			printf("Could not read line in input TA file for initialization\n");
		}
		token = strtok(line, s);
		int temp;
		for (int j = 0; j < prev_iter_clauses*CLASSES*FEATURES*2; j++) {
			temp = atoi(token);
			prev_TAs[j] = (temp > NUMBER_OF_STATES)?(NUMBER_OF_STATES+1):0;//prev_TAs[j] = (temp > 0)?(NUMBER_OF_STATES+1):0;  //prev_TAs[j] = (temp > 0)?(NUMBER_OF_STATES+1):0;  //
			token=strtok(NULL,s);
		}
		fclose(fp);

	}
}

void count_includes(struct MultiClassTsetlinMachine *mc_tm)
{
	int count = 0;
	for(int i = 0; i < CLASSES; i++)
	{
		for(int j = 0; j < CLAUSES; j++)
		{
			for (int k = 0; k < FEATURES; k++) 
			{
				
				if(((mc_tm->tsetlin_machines[i])->ta_state[j][k][0]) > NUMBER_OF_STATES)
					{
						count++;
					}
				// l++;
				
				if(((mc_tm->tsetlin_machines[i])->ta_state[j][k][1]) > NUMBER_OF_STATES)
					{
						count++;
					}
				// l++;
			}
		}
	}
	printf("Number of includes = %d\n", count);
}

void save_TAs(struct MultiClassTsetlinMachine *mc_tm, char *Output_TA_filename)
{
	FILE *TAs;
	TAs = fopen (Output_TA_filename,"w");
	// int updated_TAs[CLAUSES*CLASSES*FEATURES*2];
	// int l = 0;
	for(int i = 0; i < CLASSES; i++)
	{
		for(int j = 0; j < CLAUSES; j++)
		{
			for (int k = 0; k < FEATURES; k++) 
			{
				fprintf(TAs, "%d ", (mc_tm->tsetlin_machines[i])->ta_state[j][k][0]);
				// l++;
				// updated_TAs[l] = (mc_tm->tsetlin_machines[i])->ta_state[j][k][1];
				fprintf(TAs, "%d ", (mc_tm->tsetlin_machines[i])->ta_state[j][k][1]);				
				// l++;
			}
		}
	}
	printf("Stored TA in \"%s\"\n", Output_TA_filename);
	fclose(TAs);
}

int main( int argc, char *argv[])
{	
	printf("---------------------------------------------\n");
	srand(time(NULL));
	int epochs=2, prev_iter_clauses = 0;
	float s = 5, curr_accuracy = 0.0, max_accuracy = 0.0;
	bool default_initilization = 0;

	if (argc < 7) 
	{
		fprintf(stderr, "Usage: %s <train_file> <test_file> <Output_TA_filename> <hyperparameter_s> <epochs> <initialize> <TA_file_for_initialization> <clauses>\n", argv[0]);		
		printf("If <initialize> is '1' then provide TA filename for initialization along with the number of clauses used in previous training cycle otherwise default initialization will be used.\n");
		printf("<TA_file_for_initialization> <clauses> are not required if <initialize> is '0'\n");
		exit(EXIT_FAILURE);
	}
	if( atoi(argv[6]) == 1) 
	{
		if(argc < 9)
		{
			default_initilization = 0;
			printf("Initialization file or number of clauses in previous training cycle not provided. default initialization will be used\n");		
		}
		else
		{
			default_initilization = 1;
			prev_iter_clauses = atoi(argv[8]);
			printf("Clauses in previous iteration = %d\n", prev_iter_clauses);
		}      
   	}
   	else
   	{
   		printf("Default initialization will be used\n");
   	}
   	s = atof(argv[4]);
   	printf("Hyperparameter s = %.2f\n", s);
	epochs = atoi(argv[5]);
	unsigned short *prev_TAs = malloc(prev_iter_clauses*CLASSES*FEATURES*2*sizeof(unsigned short));	
	read_file(prev_iter_clauses, prev_TAs, default_initilization, argv[1], argv[2], argv[7]);

	struct MultiClassTsetlinMachine *mc_tsetlin_machine = CreateMultiClassTsetlinMachine(prev_iter_clauses, prev_TAs);
	
	max_accuracy = mc_tm_evaluate(mc_tsetlin_machine, X_train, y_train, NUMBER_OF_EXAMPLES_TRAIN);
	printf("Initial Accuracy: %f\n", max_accuracy);
	printf("Initial ");count_includes(mc_tsetlin_machine);
	printf("---------------------------------------------\n");
	for (int i = 0; i < epochs; i++) {
		
		clock_t start_total = clock();
		mc_tm_fit(mc_tsetlin_machine, X_train, y_train, NUMBER_OF_EXAMPLES_TRAIN, 1, s);
		clock_t end_total = clock();
		double time_used = ((double) (end_total - start_total)) / CLOCKS_PER_SEC;

		printf("EPOCH %d TIME: %f\n", i+1, time_used);
		curr_accuracy = mc_tm_evaluate(mc_tsetlin_machine, X_train, y_train, NUMBER_OF_EXAMPLES_TRAIN);
		printf("Training Accuracy: %f\n", curr_accuracy);
		count_includes(mc_tsetlin_machine);	
		if(curr_accuracy >= max_accuracy)
		{
			max_accuracy = curr_accuracy;
			save_TAs(mc_tsetlin_machine, argv[3]);
			// break;
		}
		// Uncomment the next line if you want to count the number of feedbacks
		// mc_tm_show_values(mc_tsetlin_machine);
		printf("+\n");
	}
	// save_TAs(mc_tsetlin_machine, argv[3]);
	curr_accuracy = mc_tm_evaluate(mc_tsetlin_machine, X_test, y_test, NUMBER_OF_EXAMPLES_TEST);	
	printf("---------------------------------------------\n");
	printf("Test Accuracy: %f\n", curr_accuracy);
	count_includes(mc_tsetlin_machine);
	printf("---------------------------------------------\n");
	return 0;
}
