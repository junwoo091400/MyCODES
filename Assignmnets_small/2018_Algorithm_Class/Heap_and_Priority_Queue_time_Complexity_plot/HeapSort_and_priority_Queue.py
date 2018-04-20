def Max_heapify(arr,i):
	if(i*2 > len(arr) - 1):#Child Idx out of range
		return
	k = 0
	if(i*2+1 > len(arr) - 1):
		k = i*2
	else:
		if(arr[i*2] >= arr[i*2+1]):
			k = i*2
		else:
			k = i*2 + 1
	if(arr[i] >= arr[k]):
		return
	arr[i], arr[k] = arr[k], arr[i]

	Max_heapify(arr,k)

def build_max_heap(arr):
	for i in range((len(arr)-1)//2,0,-1):
		Max_heapify(arr,i)
		#print(arr)

def heap_sort(arr):
	copy = arr[:]
	build_max_heap(copy)
	print('built max heap',copy)
	for i in range(len(copy)-1,0,-1):#Until '1'
		#print(i)
		copy[i], copy[1] = copy[1], copy[i]
		arr[i] = copy.pop(i)
		Max_heapify(copy,1)

def max_heap_insert(arr,key):
	arr.append(key)
	i = len(arr)-1
	while(i>1 and arr[i//2] < arr[i]):
		arr[i], arr[i//2] = arr[i//2], arr[i]
		i = i//2

def heap_is_empty(arr):
	return (len(arr) <= 1)

def heap_extract_max(arr):
	if(len(arr)<=1):
		return
	max = arr[1]
	arr[1] = arr[len(arr)-1]
	arr.pop(len(arr)-1)
	Max_heapify(arr,1)
	return max

def list_insert(arr,key):
	arr.append(key)

def list_extract_max(arr):
	if(len(arr)<=1):
		return
	max = 1
	for i in range(2,len(arr)-1):
		if(arr[max] < arr[i]):
			max = i
	Value = arr[max]
	arr[max] = arr[len(arr)-1]
	arr.pop(len(arr)-1)#Length -1
	return Value

def list_is_empty(arr):
	return (len(arr) <= 1)

#arr = [0] +[i**2 for i in range(50)]
#Max_heapify(arr,1)
#print(arr)
#heap_sort(arr)

#build_max_heap(arr)

#print(heap_extract_max(arr))

#for i in range(10):
#	max_heap_insert(arr,i**2)

#print(heap_extract_max(arr))# log(N)
#print(list_extract_max(arr))# N

#print(arr)

import matplotlib.pyplot as plt
import time

def timecheck(tfunc):
    def wrapper(*args, **kwargs):
        t1 = time.time()
        for i in range(10):# 10 times.
            result = tfunc(*args, **kwargs)
        t2 = time.time() - t1
        return result, t2 # ACTUAL output!
    return wrapper # 'Wrapping' func.



import random

@timecheck
def test_heap(N, M):
	arr = [-1]
	for i in range(N):
		max_heap_insert(arr,random.randint(0,N))

	insert_count = 0
	extract_count = 0
	
	for _ in range(M):
		max_heap_insert(arr,random.randint(0,N))
	for _ in range(M):
		max = heap_extract_max(arr)
	'''
	for i in range(M):
		if(random.randint(0,1) or heap_is_empty(arr)):
			max_heap_insert(arr,random.randint(0,N))
			insert_count += 1
		else:
			max = heap_extract_max(arr)
			extract_count += 1
			#print(max)
	'''
	return (insert_count, extract_count)

#test_heap(5,5)

@timecheck
def test_list(N, M):
	arr = [-1]
	for i in range(N):
		list_insert(arr,random.randint(0,N))
	
	insert_count = 0
	extract_count = 0

	for _ in range(M):
		list_insert(arr,random.randint(0,N))
	for _ in range(M):
		max = list_extract_max(arr)
	'''
	for i in range(M):
		if(random.randint(0,1) or list_is_empty(arr)):
			list_insert(arr,random.randint(0,N))
			insert_count += 1
		else:
			max = list_extract_max(arr)
			extract_count += 1
	'''
	return (insert_count, extract_count)

#print(test_list(100,5))

Max_num = 100
Input_nums = [1,5,10,30,50,100,200,400]

test_heap_result = [test_heap(Max_num, input_num)[1] for input_num in Input_nums] # Only 'time' part

test_list_result = [test_list(Max_num, input_num)[1] for input_num in Input_nums] # Only 'time' part

plt.plot(Input_nums,test_heap_result,label = 'test_heap')
plt.plot(Input_nums,test_list_result,label = 'test_list')

plt.legend()
plt.show()
