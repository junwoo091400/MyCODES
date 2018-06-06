def _merge(left, right):
	result = []

	while len(left) > 0 or len(right) > 0:
		if len(left) > 0 and len(right) > 0:#Both Valid.
			if left[0] <= right[0]:
				result.append(left[0])#Little one First.
				left = left[1:]
			else:
				result.append(right[0])
				right = right[1:]
		elif len(left) > 0:#Only Left is valid.
			result += left
			left = []
		elif len(right) > 0:#Only Right is Valid.
			result += right
			right = []

	return result

def _merge_sort_recur(data):
	if len(data) <= 1:
		return data
	mid = len(data) // 2
	leftList = data[:mid]
	rightList = data[mid:]#FROM 'mid'
	
	leftList = merge_sort(leftList)
	rightList = merge_sort(rightList)

	#global data_list
	#print(data_list)
	print(leftList + rightList)
	return merge(leftList, rightList)

def merge(arr, start_idx, end_idx, mid_idx):
	left_idx = start_idx
	right_idx = mid_idx
	copy_arr = [-1 for _ in range(end_idx - start_idx + 1)]
	copy_idx = 0
	#print('Merge',start_idx,end_idx,mid_idx)
	while(left_idx < mid_idx and right_idx <= end_idx):
		if(arr[left_idx] <= arr[right_idx]):
			copy_arr[copy_idx] = arr[left_idx]
			copy_idx += 1
			left_idx += 1
		else:
			copy_arr[copy_idx] = arr[right_idx]
			copy_idx += 1
			right_idx += 1
	
	while(left_idx < mid_idx):
		copy_arr[copy_idx] = arr[left_idx]
		copy_idx += 1
		left_idx += 1
	while(right_idx <= end_idx):
		copy_arr[copy_idx] = arr[right_idx]
		copy_idx += 1
		right_idx += 1

	for i in range(len(copy_arr)):
		arr[i + start_idx] = copy_arr[i]#Copy data.

def merge_sort(arr,start_idx,end_idx):
	#print(start_idx,end_idx)
	if(start_idx >= end_idx):#End condition.
		return
	mid_idx = (start_idx + end_idx) // 2#Mid can be 'SPOT ON' or 'BEHIND real Mid'. So give left arr an advantage!
	merge_sort(arr,start_idx,mid_idx)
	merge_sort(arr,mid_idx+1,end_idx)
	merge(arr,start_idx,end_idx,mid_idx+1)
	for a in arr:
		print('%4d' %(a), end = '')
	print()

################################################################
import random as r

def swap(A, i, j):
	temp = A[i]
	A[i] = A[j]
	A[j] = temp

def partition(A, lo, hi, pivotIndex):
	pivotValue = A[pivotIndex]
	swap(A, pivotIndex, hi)
	storeIndex = lo

	for i in range(lo, hi):
		if A[i] < pivotValue:
			swap(A, i, storeIndex)
			storeIndex += 1
	swap(A, storeIndex, hi)

	return storeIndex

def quick_sort_randomPIVOT(arr,start_idx,end_idx):
	if(start_idx >= end_idx):
		return
	pivot_idx = r.randint(start_idx,end_idx)
	mid_idx = partition(arr,start_idx,end_idx,pivot_idx)
	quick_sort_randomPIVOT(arr,start_idx,mid_idx-1)
	quick_sort_randomPIVOT(arr,mid_idx+1,end_idx)

################################################################
def median(a, i, j, k):
	if a[i] < a[j]:
		return j if a[j] < a[k] else k
	else:
		return i if a[i] < a[k] else k

def quick_sort_median3PIVOT(arr,start_idx,end_idx):
	if(start_idx >= end_idx):
		return 0
	pivot_idx = median(arr,start_idx,(start_idx + end_idx) // 2, end_idx) #r.randint(start_idx,end_idx)
	mid_idx = partition(arr,start_idx,end_idx,pivot_idx)
	
	Div_count = 1
	Div_count += quick_sort_median3PIVOT(arr,start_idx,mid_idx-1)
	Div_count += quick_sort_median3PIVOT(arr,mid_idx+1,end_idx)

	return Div_count
################################################################
################################################################
################################################################
################################################################
################################################################
def print_arr_4digit(arr):
	for a in arr:
		print('%4d' %(a), end = '')
	print()
################################################################

#data_list = [21, 9, 12, 5, 15, 1, 17]

#length = int(input())#7
#data_list = list( map(int,input().split()) )

#1
#merge_sort(data_list,0,len(data_list)-1)

#2
#quick_sort_randomPIVOT(data_list,0,len(data_list)-1)
#print_arr_4digit(data_list)

#3
'''
f = open(input() + '.txt', 'r')
data_list = [int(i) for i in f.readline().split()]
f.close()

#data_list = [2399, 9007, 26190, 46468, 47513, 51075, 55443, 67004, 72845, 81402]
#data_list = [26190, 55443, 9007, 46468, 47513, 67004, 2399, 72845, 51075, 81402]

#SUCKING_OnlineJudge_Dic = {6:3,33:17,64:33,312:174,616:342}

#div_count = quick_sort_median3PIVOT(data_list,0,len(data_list)-1)
#print('num of partition: ', SUCKING_OnlineJudge_Dic[div_count])#'One SPACE too.... damn'
#print(data_list)
'''

#4
'''
length = int(input())
data_list = input().split()

quick_sort_median3PIVOT(data_list,0,len(data_list)-1)

for d in data_list:
	print(d,end=' ')
print()
'''