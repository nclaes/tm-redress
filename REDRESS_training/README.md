# REDRESS - a novel leaRning EDge infeRence methodology for Embedded tSetlin machineS

## About
Paper title - "REDRESS: An Embedded Machine Learning Methodology Using Tsetlin Machines"
Status - Under review in IEEE Transactions on Pattern Analysis and Machine Intelligence

This code allows training Multiclass Standard Tsetlin Machines (Machine Learning) for classification problems. It allows Tsetlin Automata (TA) Re-profiling (TAR) with changes in TM achitectural parameters after every training cycles. It saves the raw TA states in a file if better training accuracy is achieved. REDRESS using TAR aims to reduce the number of includes and improve classification accuracy. 

REDRESS, also, proposes a compression methodology to store the trained TA model and an inference algorithm that performs classification using the compressed data structure. 

The tuned parameters are: Number clauses per class, s-hyperparamter, T-hyperparamter 
Booleanized data is provided in the /Boolean_datasets folder
Trained RAW TA files for MNIST, Fashion MNIST, Kuzushiji-MNIST, CIFAR2 and Six Keywords Spotting (KWS6) can be found in TA/ folder.

## Setup

The TsetlinMachine.h needs to be configured depending on the dataset. For example, below the configuration represents setting for MNIST, Fashion MNIST and Kuzushiji-MNIST CIFAR2 and KWS6.

CIFAR2:

		#define CLASSES 2
		#define FEATURES 1024
		#define NUMBER_OF_EXAMPLES 50000
		#define NUMBER_OF_VALI_EXAMPLES 10000
		#define THRESHOLD 30

KWS6:

		#define CLASSES 6
		#define FEATURES 377
		#define NUMBER_OF_EXAMPLES 15000
		#define NUMBER_OF_VALI_EXAMPLES 3120
		#define THRESHOLD 25

MNIST, FMNIST, KMNIST:

		#define CLASSES 10
		#define FEATURES 784
		#define NUMBER_OF_EXAMPLES 60000
		#define NUMBER_OF_VALI_EXAMPLES 10000
		#define THRESHOLD 20

## Run
```
python3 w_b.py --C 50,200,50 --s 5,10,0.50 --T 10,15,0.50 --e 50,100,50 --run MNIST
```

    What the above means: 

        --C 50 200 50   : go from 50 to 200 clauses in increments of 50
        --s 5 10 0.50   : go from 5 to 10 in s value in increments of 0.50
        --T 10 15 0.50  : go from 10 to 15 in T value in increments of 0.50
        --e 50 100 50   : go from 50 to 100 in epochs in increments of 50
        --run MNIST     : The name of the W&B run is called MNIST

In this example the resulting TA files will be stored in 
An example terminal output is given below: MNIST_TMASP_TA_files

The source code of C implementation of the original standard Tsetlin Machine (https://arxiv.org/abs/1804.01508) can be found at https://github.com/cair/TsetlinMachine.

