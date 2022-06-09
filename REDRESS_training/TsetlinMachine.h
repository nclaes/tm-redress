
// MODIFY MACOS AS PER YOUR DATASET
#define CLAUSES 200
#define CLASSES 10
#define NUMBER_OF_EXAMPLES_TRAIN 60000
#define NUMBER_OF_EXAMPLES_TEST 10000
#define THRESHOLD 10
#define FEATURES 784

#define NUMBER_OF_STATES 200
#define BOOST_TRUE_POSITIVE_FEEDBACK 0

#define PREDICT 1
#define UPDATE 0

struct TsetlinMachine { 
	short ta_state[CLAUSES][FEATURES][2];

	int clause_output[CLAUSES];

	int hit_count;

	int num_samples;

	int feedback_to_clauses[CLAUSES];
};

struct TsetlinMachine *CreateTsetlinMachine(int curr_class, int prev_iter_clauses, unsigned short* prev_TAs );

void tm_initialize(struct TsetlinMachine *tm, int curr_class, unsigned short* prev_TAs, int prev_iter_clauses);

unsigned int tm_show_value(struct TsetlinMachine *tm, int class_number);

void tm_update(struct TsetlinMachine *tm, int Xi[], int target, float s);

int tm_score(struct TsetlinMachine *tm, int Xi[]);

int tm_get_state(struct TsetlinMachine *tm, int clause, int feature, int automaton_type);

