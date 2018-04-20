##############################################################

def get_wordSet_with_Count(strringy):
	#BlackList = [',',"\'","\"",":"]
	
	refined = strringy.split()#Since Most words are seperated with ' '

	Dic = {}#Empty dic.

	for word in refined:
		if(word[-1] == '.' or word[-1] == ','):# Words can have comma or dot at the End.
			word = word[:-1]#Cut the dot
		word = word.lower()# Lower the case. So 'The' and 'the' are counted equal.
		if(word in Dic):
			Dic[word] = Dic[word] + 1#Increment count.
		else:
			Dic[word] = 1#New Word.

	return Dic

def get_ordered_Key_by_Value(dict):
	return sorted(dict, key = dict.get, reverse = True)# Will be ordered by Value, returned by 'get' function.

##############################################################

def getMaxASCII_alphabet_value(word):
	max_ascii = ord('A')#Minimum, 65.
	for alpha in word:
		if(alpha.isalpha()):#Is Alphabet?
			if(max_ascii < ord(alpha)):
				max_ascii = ord(alpha)#Update.
	return max_ascii#BELIEVE that at least one alphabet was included as input, otherwise, Screw you.

def getMinASCII_alphabet_value(word):
	min_ascii = ord('z')#Maximum, 122.
	for alpha in word:
		if(alpha.isalpha()):#Is Alphabet?
			if(min_ascii > ord(alpha)):
				min_ascii = ord(alpha)#Update.
	return min_ascii

##############################################################

def get_Factorial_recur(n):
	if(n == 0):
		return 1
	return n * get_Factorial_recur(n-1)

##############################################################

get_Fibonacci_used_LIST = [1,1]#0th, 1th.

def get_Fibonacci(n):#Uses memory(list) to compromise computation time.
	
	if(len(get_Fibonacci_used_LIST) < n):#Require 'Update'
		get_Fibonacci_used_LIST.append(get_Fibonacci(n-1) + get_Fibonacci(n-2))#Call 'n-1' first.
	
	return get_Fibonacci_used_LIST[n-1]

##############################################################

def get_Fibonacci_DIRTY_Recur(n):
	if(n == 1 or n == 2):
		return 1
	return get_Fibonacci_DIRTY_Recur(n-1) + get_Fibonacci_DIRTY_Recur(n-2)# SOOOOO DIRTY!!!!

##############################################################

def get_GCD_recur(a,b):
	if(a == 0):#Calc done.
		return b
	return get_GCD_recur(b % a, a)# Remain, (<) Divider

##############################################################

def LINEAR_SEARCH_find_alphabet_index(target,stringy):
	if( not target.isalpha()):#NOT valid target.
		return -1
	target = target.lower()
	for idx in range(len(stringy)):
		if stringy[idx].isalpha():#verify
			if(stringy[idx].lower() == target):#Target is already lowered.
				return idx
	return -1

##############################################################

def BINARY_SEARCH_find_alphabet_index(target,stringy):
	low_case_str = ''
	for idx in range(len(stringy)):
		add_char = stringy[idx]
		if(add_char.isalpha()):
			add_char = add_char.lower()#Pre-process to MAKE-SURE all alphabets are in Lower case!!!!
		low_case_str += (add_char)

	target = target.lower()#Make target 'lower case' too...

	sorted_string = ''.join(sorted(low_case_str))#Returns 'sorted string' CLIMBING order. --- ++++
	
	return BINARY_SEARCH_low_level(target,sorted_string,0,len(sorted_string)-1,ord)

def BINARY_SEARCH_low_level(target,sorted_string,start_idx,end_idx,order_func):
	if(start_idx > end_idx):
		return -1
	mid_idx = (start_idx + end_idx) // 2
	if(target == sorted_string[mid_idx]):
		return mid_idx
	if(order_func(sorted_string[mid_idx]) < order_func(target)):#'Increasing Order' is default.
		return BINARY_SEARCH_low_level(target,sorted_string,mid_idx+1,end_idx,order_func)
	else:
		return BINARY_SEARCH_low_level(target,sorted_string,start_idx,mid_idx-1,order_func)

##############################################################

def find_ASCII_minMax_from_string(stringy):
	ASC_min = 255
	ASC_max = 0
	for segment in stringy:
		if(ord(segment) < ASC_min):
			ASC_min = ord(segment)
		if(ord(segment) > ASC_max):
			ASC_max = ord(segment)
	return (ASC_min, ASC_max)

##############################################################

def print_string_in_binary_8(stringy):
	for segment in stringy:
		print(bin(ord(segment))[2:].zfill(8),end=' ')#Fill left over part on left with 0.

##############################################################

input_string = input()

wordset_dict = get_wordSet_with_Count(input_string)

ordered_keys = get_ordered_Key_by_Value(wordset_dict)

Most_Frequent_Word = ordered_keys[0]#No.1 Used Word from 'list'
print('Most Frequent Word :',Most_Frequent_Word)

Max_ASCII = getMaxASCII_alphabet_value(Most_Frequent_Word)
Min_ASCII = getMinASCII_alphabet_value(Most_Frequent_Word)
print('Max = {}, Min = {}'.format(Max_ASCII,Min_ASCII))

print('Factorial Value')
Min_ASCII_factorial = get_Factorial_recur(Min_ASCII)
print(Min_ASCII_factorial)

fib_input = Max_ASCII - Min_ASCII
print('FAST! Fibonacci. Recursive, but uses memory(list) so MUCH better!!!!!')

for i in range(1, fib_input+1):
	print(get_Fibonacci(i),end=' ')

Fast_Fibonacci = get_Fibonacci(fib_input)
print('\n',Fast_Fibonacci)

print('SLOW~ Fibonacci. Recursive, very very dumb. 2^x calculation, so waste of time, basically.')

for i in range(1, fib_input+1):
	print(get_Fibonacci_DIRTY_Recur(i),end=' ')

Slow_Fibonacci = get_Fibonacci_DIRTY_Recur(fib_input)
print('\n',Slow_Fibonacci)

##############################################################
##############################################################

print('Getting GCD...')
GCD_val = get_GCD_recur(Min_ASCII, Max_ASCII)
print(GCD_val)

##############################################################
##############################################################

target_list = ['E', 'T', 'O', 'A', 'N']

print('Linear Search starting...')
for tgt in target_list:
	res = LINEAR_SEARCH_find_alphabet_index(tgt,input_string)
	if(res == -1):
		print(tgt,'Not Found.')
	else:
		print(tgt,'Found on index',res)

print('Binary Search Starting...')
for tgt in target_list:
	res = BINARY_SEARCH_find_alphabet_index(tgt,input_string)
	if(res == -1):
		print(tgt,'Not Found.')
	else:
		print(tgt,'Found on index',res)

##############################################################
##############################################################

print('Min/Max ASCII value from Input String find...')

asc_min,asc_max = find_ASCII_minMax_from_string(input_string)

print('Min',chr(asc_min),asc_min,'Max',chr(asc_max),asc_max)

print_string_in_binary_8(input_string)

input('Waiting....')

##############################################################