import random as r
import time as t
import string

## WORD GENERATION.

vowels = list('aeiou')

def genWord(min, max):
	word = ''
	syllables = min + int(r.random() * (max - min + 1)) # RANDOM number of 'syllables(One Sound)'
	for i in range(0, syllables):
		word += genSyllable()
	return word#.capitalize()
	

def genSyllable():
	rand = r.random()
	if rand < 0.333:
			return wordPart('v') + wordPart('c')
	if rand < 0.666:
			return wordPart('c') + wordPart('v')
	return wordPart('c') + wordPart('v') + wordPart('c')

non_vowel_ascii_list = [ch for ch in list(string.ascii_lowercase) if ch not in vowels]

def wordPart(type):
	if type is 'c':
			return r.sample(non_vowel_ascii_list, 1)[0]
	if type is 'v':
			return r.sample(vowels, 1)[0]

#WORD GENERATION END.

##Optimized radixSort.(ASSUME Capital Letter doesn't exist.)

Alphabet_ordered_check_list = [False] * 26


def radixSort_optimized(A,d):
	for i in range(26):
		Alphabet_ordered_check_list[i] = False#Init.


#

#From Github

def MSD_radix_string_sort(L, i):

	# base case (list must already be sorted)
	if len(L) <= 1:
		return L

	# divide (first by length, then by lexicographical order of the first character)
	done_bucket = []
	buckets = [[] for _ in range(27)] # one for each letter in a-z

	for s in L:
		if i >= len(s):
			done_bucket.append(s)#Literally, finished.
		else:
			buckets[ ord(s[i]) - ord('a') ].append(s)

	# conquer (recursively sort buckets)
	buckets = [ MSD_radix_string_sort(b, i + 1) for b in buckets ]

	# marry (chain all buckets together)
	return done_bucket + [ b for blist in buckets for b in blist ]

#

## Standard radixSort

R=256

def countingSort(A, d):
	Optimal_str_len = d + 1#Length of optimal string.
	B = [-1] + [0]*len(A)
	C = [0]*(R)

	for word in A:
		if(len(word) < Optimal_str_len):
			C[0] += 1
		else:
			idx = ord(word[d])#That 'index' tho!
			C[idx] += 1
	#Finished counting each category(ord).

	for i in range(1,len(C)):
		C[i] = C[i-1] + C[i]
	#Finished stacking numbers from the beginning.

	for A_idx in range(len(A)-1,-1,-1):#From the Back.
		if(len(A[A_idx]) < Optimal_str_len):
			B[C[0]] = A[A_idx]#Special Case. String too short.
			C[0] -= 1
		else:
			idx = ord(A[A_idx][d])
			B[C[idx]] = A[A_idx]
			C[idx] -= 1
	#Finished filling up 'B' list from idx 1 ~ len(A).

	for i in range(1,len(A)+1):
		A[i-1] = B[i]
	#Save the result back into 'A' list.

def radixSort(A, d):#d is Maximum 'Number of Digits(theory)'
	for i in range(d-1, -1, -1):#until '0'
		countingSort(A, i)

#Standard radixSort.End.

PYTHON_SORT_ACTIVATE = True

GITHUB_BUCKET_SORT_ACTIVATE = True

n = 5 #음절의 수

Word_test_count = 1000000

t0 = t.time()

try:
	File = open(str(Word_test_count) + '.txt','r')#Test Data Available Already?
	File_content = File.readlines()
	x = [word.strip('\n') for word in File_content]
except:
	x = [genWord(1, n) for i in range(Word_test_count)]
	File = open(str(Word_test_count) + '.txt','w')
	for word in x:
		File.write('%s\n' % word)

File.close()

t1 = t.time()
print('Got Words :',t1 - t0,'Seconds')

if PYTHON_SORT_ACTIVATE:
	x_copy = x[:]#COPY!
	print('Python Sort Starting...')
	t1 = t.time()
	x_copy.sort() # SO, DAMN, FAST!
	t2 = t.time()
	print('Python Sorted :', t2-t1,'Seconds')

	print('Saving Result...')
	File = open(str(Word_test_count) + '_pythonSorted.txt','w')
	for word in x_copy:
		File.write('%s\n' % word)
	File.close()

if GITHUB_BUCKET_SORT_ACTIVATE:
	x_copy = x[:]#COPY!
	print('Github Bucket Sort Starting...')
	t1 = t.time()
	result = MSD_radix_string_sort(x_copy,0)
	t2 = t.time()
	print('Github Bucket Sorted :', t2-t1,'Seconds')

	print('Saving Result...')
	File = open(str(Word_test_count) + '_githubBucketSorted.txt','w')
	for word in result:
		File.write('%s\n' % word)
	File.close()

#
print('Radix Sort Starting...')
digit = n*3 # 영문자 개수의 최대치
#print(x)
t1 = t.time()
radixSort(x, digit)
t2 = t.time()

print('Radix Sorted :', t2-t1,'Seconds')
#print(x)

print('Saving Result...')
File = open(str(Word_test_count) + '_radixSorted.txt','w')
for word in x:
	File.write('%s\n' % word)
File.close()

input('Waiting...')