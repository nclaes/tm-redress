import numpy as np
from numpy import genfromtxt
import operator
import collections
import math
import sys
import subprocess
import argparse
import os

def include_encoding(num_literals,num_classes,num_clauses,binary_states,output_filename,include_per_class_filename):

	state_type_count = collections.Counter(np.array(binary_states))
	clauses_with_all_zero_TAs = 0
	clause_num = 0
	ta_num = 0
	bit_1_counter1 = 0
	bit_1_counter2 = 0
	flag = 0
	addr = 0
	active_TAs_per_class = np.zeros(num_classes, dtype='H')
	# active_TAs_per_clause = np.zeros(num_classes*num_clauses, dtype='H')
	number_of_tas = len(binary_states)
	automata = list()  #np.zeros(state_type_count[1], dtype='H')

	for i in range(num_classes):  # num_classes
		bit_1_counter1 = 0
		clause_num = i*num_clauses
		polarity = 0        # This is to encode the polarity of clause using the MSB
		clause_change = 0   # This is to encode change in clause using the bit next to MSB
		for j in range(num_clauses):
			bit_1_counter2 = 0
			ta_sq = (clause_num + j)*num_literals
			flag = 0
			for k in range(num_literals):
				ta_num = ta_sq + k
				if(binary_states[ta_num] == 1):
					flag = 1
					addr = k|polarity; addr = addr|clause_change;
					automata.append(addr)
					bit_1_counter1 = bit_1_counter1 + 1
					bit_1_counter2 = bit_1_counter2 + 1
					
			# active_TAs_per_clause[clause_num + j] = bit_1_counter2
			if(bit_1_counter2 == 0):
				clauses_with_all_zero_TAs = clauses_with_all_zero_TAs + 1
			if(polarity == 0):
				polarity = 32768
			else:
				polarity = 0
			if(flag == 1):
				if(clause_change == 0):
					clause_change = 16384
				else:
					clause_change = 0


		active_TAs_per_class[i] = bit_1_counter1
	automata = np.array(automata, dtype='H')
	# print(automata, max(automata), min(automata), len(automata))
	# print('-----------------------------------------------------------------------')
	# print(active_TAs_per_clause, max(active_TAs_per_clause), min(active_TAs_per_clause), sum(active_TAs_per_clause), len(active_TAs_per_clause))
	# print('-----------------------------------------------------------------------')
	# print(active_TAs_per_class, max(active_TAs_per_class), min(active_TAs_per_class), len(active_TAs_per_class), sum(active_TAs_per_class))
	print('-----------------------------------------------------------------------')
	print('No of clauses with no include TAs = ',clauses_with_all_zero_TAs)
	print('No of include TAs = ', state_type_count[1])
	print('No of exclude TAs = ', state_type_count[0])
	print('Total no of TAs = ',len(binary_states))

	np.savetxt(include_per_class_filename, active_TAs_per_class, fmt='%u', delimiter=' ' )
	np.savetxt(output_filename, automata, fmt='%u', delimiter=' ' )

	print()
	return automata, active_TAs_per_class, state_type_count[1]

def parse_inc_exc_from_raw_states(raw_states, states):
	binary_states = np.zeros(len(raw_states), dtype='B')
	for i in range(len(raw_states)):
		if(raw_states[i] > states):
			binary_states[i] = 1
		else:
			binary_states[i] = 0
	# fp = open('output.txt', 'w')
	# fp.writelines(''.join(str((binary_states))))
	# fp.close()
	return binary_states

def REDRESS_input_format(test_filename, output_filename, features,te):	
	test_data = np.loadtxt(test_filename, delimiter='\n', dtype=str)
	if(not te):
		te = len(test_data)	
	print('-----------------------------------------------------------------------')
	print('Assuming 32-bit word size for an integer')
	print('Number of test examples considered =', te); print('No of 32-bit words required to save them =', math.ceil(te/32)*features)
	test_data_int = np.ndarray((te, features), dtype='B')
	classification_output = np.ndarray(te, dtype='B')
	REDRESS_test_data = np.zeros((math.ceil(te/32),features), dtype=np.uint32);#print(REDRESS_test_data.shape)
	
	for i in range(te):
		temp = test_data[i].split(" ")
		test_data_int[i] = temp[0:features]
		classification_output[i] = int(float(temp[-1]))
	del test_data	

	#------------------------------------------------------------------
	classification_filename = output_filename + 'classification_file' + str(te) + '.txt'
	np.savetxt(classification_filename ,classification_output, delimiter=' ',fmt='%u')
	print('True classification output filename -', classification_filename)
	#------------------------------------------------------------------

	#------------------------------------------------------------------
	for i in range(math.floor(te/32)):
		for j in range(features):
			gather_jth_bitsof_32inputs = []
			for k in range(32):
				gather_jth_bitsof_32inputs.append(test_data_int[(i*32)+k][j])
			REDRESS_test_data[i][j] = int(''.join(str(e) for e in gather_jth_bitsof_32inputs),2)			
	
	if((te % 32) > 0):
		for j in range(features):
			gather_jth_bitsof_32inputs = []
			for k in range(te % 32):
				gather_jth_bitsof_32inputs.append(test_data_int[(math.floor(te/32)*32)+k][j])
			REDRESS_test_data[math.floor(te/32)][j] = int(''.join(str(e) for e in gather_jth_bitsof_32inputs),2)
	
	REDRESS_test_data = np.reshape(REDRESS_test_data,(math.ceil(te/32)*features))
	#------------------------------------------------------------------
	# print(REDRESS_test_data)

	test_feature_filename = output_filename + 'Testfile' + str(te) + '.txt'
	np.savetxt(test_feature_filename ,REDRESS_test_data, delimiter=' ',fmt='%u')
	print('REDRESS input format filename -', test_feature_filename)
	return REDRESS_test_data, test_feature_filename, classification_filename, te, classification_output

def Generate_IncEncSetup_H(arr):

	setup_file = open('IncludeEncodedSetup.h', 'w')
	lines = ['#ifndef INCLUDEENCODEDSETUP_H','#define INCLUDEENCODEDSETUP_H', '#define THRESHOLD ' + arr[0], '#define CLAUSES ' + arr[1], '#define CLASSES ' + arr[2],'#define FEATURES ' + arr[3],'#define NUMBER_OF_STATES ' + arr[4], '#define NUMBER_OF_TEST_EXAMPLES ' + arr[5], '#define TEST_LENGTH_PER32 ' + arr[11],'const char* TEST_FEATURE_FILE = "' + arr[6] + '";', 'const char* TEST_CLASSIFICATION_FILE = "' + arr[7] + '";', '#define NUM_OF_INCLUDES ' + arr[8], 'const char* INCENC_TA_FILE = "' + arr[9] + '";', 'const char* INC_PER_CLASS_FILE = "' + arr[10] + '";', '#define CLAUSE_POLARITY_BIT 15		// The bit holding information on clause polarity. Right shifts required to get the polarity of the current clause', '#define BIT_DEMARCATING_CLAUSE_CHANGE 14		// The bit that demarcates beginning of next clause. Right shifts required to get the bit which denotes change of clause','#define INT_VALUE_OF_ENCODING_BITS 16383  // Integer value of the remaining bits used to encode the offset of includes which correspond to the literal offset address','#endif']
	setup_file.writelines('\n'.join(lines))
	setup_file.close()


def Generate_data_store_h(automata, active_TAs_per_class, REDRESS_test_data, classification_output, arr, dataset_name):
	ucontroller_file = open(dataset_name+'_data_store.h', 'w')
	ucontroller_file.writelines(['#ifndef RLE_DATA_STORE_H_\n', '#define RLE_DATA_STORE_H_\n', '#include "stdio.h"\n', '#define CLAUSES ' + arr[1] + '\n', '#define CLASSES ' + arr[2] + '\n', '#define FEATURES ' + arr[3] + '\n', '#define NUMBER_OF_TEST_EXAMPLES ' + arr[5] + '\n', '#define TEST_LENGTH_PER32 ' + arr[11] + '\n', '#define NUM_OF_INCLUDES ' + arr[8] + '\n', '#define CLAUSE_POLARITY_BIT 15		// The bit holding information on clause polarity. Right shifts required to get the polarity of the current clause' + '\n', '#define BIT_DEMARCATING_CLAUSE_CHANGE 14		// The bit that demarcates beginning of next clause. Right shifts required to get the bit which denotes change of clause' + '\n','#define INT_VALUE_OF_ENCODING_BITS 16383  // Integer value of the remaining bits used to encode the offset of includes which correspond to the literal offset address' + '\n', 'uint16_t IncEncTA[] = {'])
	automata = str(list(automata))
	automata = automata.replace('[','')
	automata = automata.replace(']','')
	REDRESS_test_data = str(list(REDRESS_test_data))
	REDRESS_test_data = REDRESS_test_data.replace('[','')
	REDRESS_test_data = REDRESS_test_data.replace(']','')
	ucontroller_file.writelines(automata + '};\n' + 'const uint32_t inference_literals[] = {'+ REDRESS_test_data + '};\n')
	classification_output = str(list(classification_output))
	classification_output = classification_output.replace('[','')
	classification_output = classification_output.replace(']','')
	active_TAs_per_class = str(list(active_TAs_per_class))
	active_TAs_per_class = active_TAs_per_class.replace('[','')
	active_TAs_per_class = active_TAs_per_class.replace(']','')
	ucontroller_file.writelines('const int8_t y[] = {' + classification_output + '};\n' + 'uint16_t INC_per_CLASS[] = {' + active_TAs_per_class + '};\n' +'#endif')
	ucontroller_file.close()

# def verification(clauses, classes, features, te, num_includes, REDRESS_test_data, classification_output, active_TAs_per_class, automata):



def main():
	print('-----------------------------------------------------------------------')
	
	np.set_printoptions(threshold=sys.maxsize)
	p = subprocess.run(['pwd'], stdout=subprocess.PIPE).stdout.decode('utf-8').strip() + '/'
	parser = argparse.ArgumentParser(description="REDRESS: a novel leaRning EDge infeRence methodology for Embedded tSetlin machineS. This script can perform three tasks: (1) It generates an include encoded automata file from the Tsetlin Automata (TA) file, obtained after training standard Tsetlin Machine (TM), (2) It will transform the input test file to REDRESS input file format and ")
	parser.add_argument("dataset_name", help="Provide the name of the dataset or any name to the run", type=str)
	parser.add_argument("threshold", help="Provide the value of \"Threshold\" used while training the TM to obtain the TA file", type=int)
	parser.add_argument("clauses", help="Provide the number of clauses used while training the TM", type=int)
	parser.add_argument("classes", help="Provide the number of classes in the undertaken classification problem", type=int)
	parser.add_argument("features", help="Provide the number of features in the undertaken classification problem", type=int)		
	parser.add_argument("test_filename", help="Provide the name of the test file", type=str)
	parser.add_argument("ta", help="Provide the Tsetlin Automata filename. This is needed when \"op\" value is 1 implying both encoding and input test file preprocessing will be performed", type=str)
	parser.add_argument("-states", help="Provide the number of states in the TA. Default: 200", type=int,default=200)
	# parser.add_argument("-op",help="Range:[0,1]. 0 indicates only input test file will be preprocessed. 1 indicates include file will be encoded as well. Default: '1'", type=int, default=1)
	parser.add_argument("-te", help="Provide the number of test examples (te) you want to infer. If no value provided all examples in test file will be inferred.", type=int)
	
	args = parser.parse_args()

	if(not os.path.isfile(args.test_filename)):
		print('Test file not present in the designated directory.')
		sys.exit()
		

	# if(args.op):
	# 	option = args.op
	if(args.ta):
		TA_filename = args.ta
		if(os.path.isfile(TA_filename)):
			print('Encoding TA file & Transforming input file to REDRESS format.')
		else:
			print('TA file not found in the designated directory.')
			sys.exit()
	else:
		print('ERROR: TA filename needed, please check help.')
		sys.exit()
		
	# else:
		# option = 0
		# print('Only input file will be transformed to REDRESS format.')

	if(args.states):
		states = args.states
	
	REDRESS_test_data, test_feature_filename, classification_filename, te, classification_output = REDRESS_input_format(args.test_filename, args.dataset_name + '_REDRESS_', args.features, args.te)

	if(te % 32 > 0):
		chunks_of_32 = math.ceil(te/32)
	else:
		chunks_of_32 = math.floor(te/32)

	# if(option):
	raw_states = genfromtxt(TA_filename, delimiter=' ', dtype='H')	
	binary_states = parse_inc_exc_from_raw_states(raw_states, states); del raw_states
	encoded_filename = args.dataset_name + str(args.features) + '_REDRESS_IncEncTA_C' + str(args.classes) + '_CL' + str(args.clauses) + '_T' + str(args.threshold) + '.txt'
	include_per_class_filename = args.dataset_name + str(args.features) + '_REDRESS_Inc_per_class' + '.txt'
	automata, active_TAs_per_class, num_includes = include_encoding(args.features*2, args.classes, args.clauses, binary_states, encoded_filename, include_per_class_filename)
	del binary_states
	IncEncSetup_array = [str(args.threshold), str(args.clauses), str(args.classes), str(args.features), str(states), str(te), ''+test_feature_filename, classification_filename, str(num_includes), encoded_filename, include_per_class_filename, str(chunks_of_32)]
	# else:
	# 	IncEncSetup_array = [str(args.threshold), str(args.clauses), str(args.classes), str(args.features), str(states), str(te), test_feature_filename, classification_filename]

	Generate_IncEncSetup_H(IncEncSetup_array)
	Generate_data_store_h(automata, active_TAs_per_class, REDRESS_test_data, classification_output, IncEncSetup_array, args.dataset_name)

	# verification(args.clauses,args.classes,args.features,te,num_includes,REDRESS_test_data,classification_output,active_TAs_per_class,automata)

	print('-----------------------------------------------------------------------')

if __name__ == '__main__':
	main()