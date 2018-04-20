N = int(input())

Blob_map = [[0 for _ in range(N)] for _ in range(N)]

for i in range(N):
	#input() # Work-around '\n'
	Binary = input()
	#print('Got',Binary)
	#print(Binary)
	if(len(Binary) != N):
		print('Wrong ROW input! (Length not {})'.format(N))
	else:
		for j in range(N):
			Blob_map[i][j] = int(Binary[j])

#print(Blob_map)

#Image = '1', Background = '0'

Eight_directions = [[1,0],[1,1],[0,1],[-1,1],[-1,0],[-1,-1],[0,-1],[1,-1]]

def find_blob_size(arr,i,j):
	if(i<0 or j<0 or i>= len(arr) or j>=len(arr[i]) or arr[i][j] != 1):#We only like blob!
		return 0
	#print('pass through',i,j)
	global Eight_directions

	arr[i][j] = 2#Paint Itself
	sum_count = 1

	for ddir in range(8):
		sum_count += find_blob_size(arr,i+Eight_directions[ddir][0], j+Eight_directions[ddir][1])
		#print(i,j,sum_count)

	return sum_count

def find_blob_count(arr):
	total_blob_count = 0
	blob_size_list = []

	for i in range(len(arr)):
		for j in range(len(arr[i])):
			blob_cell_count = find_blob_size(arr,i,j)
			if(blob_cell_count > 0):
				total_blob_count += 1
				blob_size_list.append(blob_cell_count)

	return (total_blob_count, blob_size_list)

Total_blob_count, Blob_size_list = find_blob_count(Blob_map)

print(Total_blob_count)

for size in Blob_size_list:
	print(size,end=' ')

print()

#input('WAITING...')