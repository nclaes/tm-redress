# REDRESS - a novel leaRning EDge infeRence methodology for Embedded tSetlin machineS

## About
Paper title - "REDRESS: An Embedded Machine Learning Methodology Using Tsetlin Machines"
Status - Under review in IEEE Transactions on Pattern Analysis and Machine Intelligence

This code allows training Multiclass Standard Tsetlin Machines (Machine Learning) for classification problems. It allows Tsetlin Automata (TA) Re-profiling (TAR) with changes in TM achitectural parameters after every training cycles. It saves the raw TA states in a file if better training accuracy is achieved. REDRESS using TAR aims to reduce the number of includes and improve classification accuracy. 

REDRESS, also, proposes a compression methodology to store the trained TA model and an inference algorithm that performs classification using the compressed data structure. 

The tuned parameters are: Number clauses per class, s-hyperparamter, T-hyperparamter 
Booleanized data is provided in the /Boolean_datasets folder
Trained RAW TA files for MNIST, Fashion MNIST, Kuzushiji-MNIST, CIFAR2 and Six Keywords Spotting (KWS6) can be found in `include_encoding/TA_files/` folder.

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
--------------------------------------------------------------------------
Training--
Usage information:
```
cd REDRESS_training/
make
./redress
```
Example:
```
./redress ../Boolean_datasets/mnist_train.txt ../Boolean_datasets/mnist_test.txt mnist_ver1.txt 7.5 2 1 mnist_ver1.txt 200
```

--------------------------------------------------------------------------

Compression--
Usage information:
```
cd Include_encoding/
python3 inc_enc_and_inp_preprocess.py -h
```

    Output: 

        -----------------------------------------------------------------------
		usage: inc_enc_and_inp_preprocess.py [-h] [-states STATES] [-te TE]
		                                     dataset_name threshold clauses classes
		                                     features test_filename ta

		REDRESS: a novel leaRning EDge infeRence methodology for Embedded tSetlin
		machineS. This script can perform three tasks: (1) It generates an include
		encoded automata file from the Tsetlin Automata (TA) file, obtained after
		training standard Tsetlin Machine (TM), (2) It will transform the input test
		file to REDRESS input file format and

		positional arguments:
		  dataset_name    Provide the name of the dataset or any name to the run
		  threshold       Provide the value of "Threshold" used while training the TM
		                  to obtain the TA file
		  clauses         Provide the number of clauses used while training the TM
		  classes         Provide the number of classes in the undertaken
		                  classification problem
		  features        Provide the number of features in the undertaken
		                  classification problem
		  test_filename   Provide the name of the test file
		  ta              Provide the Tsetlin Automata filename. This is needed when
		                  "op" value is 1 implying both encoding and input test file
		                  preprocessing will be performed

		optional arguments:
		  -h, --help      show this help message and exit
		  -states STATES  Provide the number of states in the TA. Default: 200
		  -te TE          Provide the number of test examples (te) you want to infer.
		                  If no value provided all examples in test file will be
		                  inferred.

Example: 10000 test datapoints
```
python3 inc_enc_and_inp_preprocess.py MNIST 10 200 10 784 ../Boolean_datasets/mnist_test.txt ./TA_files/MNIST_High_Acc_TAs_TL_C200_T10.txt -te 10000
```
It generates six files:

			MNIST_REDRESS_Testfile10000.txt - Bit compressed test data for 10000 datapoints.
			MNIST_REDRESS_classification_file10000.txt - Expected classification output. This will used to test the inference accuracy.
			MNIST784_REDRESS_IncEncTA_C10_CL200_T10.txt - Compressed TA file.
			MNIST784_REDRESS_Inc_per_class.txt - Number of includes per code.
			IncludeEncodedSetup.h - Setup file required by the inference code include_encoding_validation.c
			MNIST_data_store.h - file required in the STM32F746G-DISCO microcontroller. It includes input data, compressed TA file and other parameters.

--------------------------------------------------------------------------

Inference: The code `include_encoding_validation.c` checks the accuracy of the model using compressed data structure. This uses first five aforementioned files. The `inference()` function in the file can be used to infer in the microcontroller.

Usage: 
```
gcc include_encoding_validation.c
./a.out
```

--------------------------------------------------------------------------
This code is restructured, modified, and/or created by Sidharth Maheshwari and Tousif Rahman, Newcastle University, and, is originally written by Prof. Ole--Christoffer Granmo, University of Agder.
The source code of C implementation of the original standard Tsetlin Machine (https://arxiv.org/abs/1804.01508) can be found at https://github.com/cair/TsetlinMachine.

For any queries please contact Sidharth Maheshwari: sidharth.maheshwari@newcastle.ac.uk
Other contacts: S.Rahman@newcastle.ac.uk, rishad.shafik@newcastle.ac.uk

