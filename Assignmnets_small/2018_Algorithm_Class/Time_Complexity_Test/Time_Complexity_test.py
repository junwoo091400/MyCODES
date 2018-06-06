class Queue:
	#Constructor
	def __init__(self, size = 8):
		self.queue = [0 for _ in range(size)]
		self.maxSize = size
		self.head = 0
		self.tail = 0

	#Adding elements
	def upgrade_queue(self):
		Bigger_queue = [0 for _ in range(self.maxSize*2)]#NEW!
			
		copy_idx = self.head
		new_idx = 0

		while(copy_idx != self.tail):#If we reach 'tail', the queue cycle is complete.
			Bigger_queue[new_idx] = self.queue[copy_idx]
			new_idx += 1
			copy_idx = (copy_idx + 1)%self.maxSize#Update queue idx.

		self.queue = Bigger_queue
		self.maxSize = self.maxSize * 2
		self.head = 0
		self.tail = new_idx

	def enqueue(self, data):
		#Checking if the queue is full
		if self.size() >= (self.maxSize - 1):#Minus 1 == FULL!! // Circular Queue.
			self.upgrade_queue()#Increase SIZE!
			#print('Upgraded size to', self.maxSize)

		self.queue[self.tail] = data
		self.tail = (self.tail + 1)%self.maxSize
		return True 

	#Deleting elements 
	def dequeue(self):
		#Checking if the queue is empty
		if self.size() <= 0:
			return ("Queue Empty") 
		data = self.queue[self.head]
		self.head = (self.head + 1)%self.maxSize
		return data
		
	#Calculate size
	def size(self):
		return (self.tail - self.head + self.maxSize) % self.maxSize # Make sure it returns 'Positive'...

#############################################################
#############################################################
#############################################################
#############################################################
#############################################################

def Swap(arr,i,f):
	arr[i], arr[f] = arr[f], arr[i]

def Bubble_Sort(arr):
	for start in range(len(arr)-1):
		Changed = False
		for swap_idx in range(start,len(arr)-1):
			if(arr[swap_idx] > arr[swap_idx+1]):
				Swap(arr,swap_idx,swap_idx+1)
				Changed = True
		if(not Changed):#Didn't change. Perfectly in order!
			return
###################################################################3
def Insertion_Sort(A,start_idx,end_idx):
	for j in range(start_idx+1,end_idx+1):
		key = A[j]
		i = j-1
		while (i >= start_idx and key < A[i]):
			A[i+1] = A[i]#Move Back.
			i -= 1
		A[i+1] = key
###################################################################3
def Linear_Search(arr,start_idx,end_idx,target):
	for i in range(start_idx,end_idx+1):
		if(arr[i] == target):
			return i
	return -1 #Not found.
###################################################################3
def Binary_Search(arr,start_idx,end_idx,target):#MUST be sorted...
	#Insertion_Sort(arr,start_idx,end_idx)#First, sort.
	if(start_idx > end_idx):
		return -1
	candidate_idx = (start_idx + end_idx) // 2
	if(arr[candidate_idx] == target):
		return candidate_idx
	elif(arr[candidate_idx] > target):
		return Binary_Search(arr,start_idx,candidate_idx-1,target)
	else:
		return Binary_Search(arr,candidate_idx+1,end_idx,target)
###################################################################3
PATH = 0#Can walk through.
BLOCK = 1
VISITED = 2

def getPathKind(arr,pos):
	return arr[pos[0]][pos[1]]

def addDirection(pos,direction):
	return(pos[0]+direction[0],pos[1]+direction[1])

NEWS_list = [(1,0),(0,1),(0,-1),(-1,0)]

def posIsInsideMaze(arr,pos):
	if(pos[0] < 0 or pos[1] < 0):
		return False
	Len = len(arr)
	Width = len(arr[0])
	if(pos[0] >= Len or pos[1] >= Width):
		return False
	return True

def findMazePath_Stack(arr,start_pos,end_pos):
	#print('STACKING')
	#print_Maze(arr)
	Stack = []
	Stack.append(start_pos)
	while(len(Stack) > 0):
		pos = Stack.pop()
		#print(pos,Stack)
		if(pos == end_pos):
			return True
		if(getPathKind(arr,pos) == BLOCK):
			continue#Skip this one.
		elif(getPathKind(arr,pos) == VISITED):
			continue#Skip also.
		elif(getPathKind(arr,pos) == PATH):#THIS, is valid.
			for direction in NEWS_list:
				newPos = addDirection(pos,direction)
				if(posIsInsideMaze(arr,newPos)):
					Stack.append(newPos)#Just append!
		else:
			pass#INVALID Something Detected...
		arr[pos[0]][pos[1]] = VISITED
	return False#No path found.

def findMazePath_Stack_safe(arr,start_pos,end_pos):
	copyArr = copyMaze(arr)
	#print(copyArr[0] is arr[0],'check')
	return findMazePath_Stack(copyArr,start_pos,end_pos)

####################################################################

def findMazePath_Queue(arr,start_pos,end_pos):
	#print('STACKING')
	#print_Maze(arr)
	mazeQueue = Queue()
	mazeQueue.enqueue(start_pos)
	while(mazeQueue.size() > 0):
		pos = mazeQueue.dequeue()
		#print(pos,Stack)
		if(pos == end_pos):
			return True
		if(getPathKind(arr,pos) == BLOCK):
			continue#Skip this one.
		elif(getPathKind(arr,pos) == VISITED):
			continue#Skip also.
		elif(getPathKind(arr,pos) == PATH):#THIS, is valid.
			for direction in NEWS_list:
				newPos = addDirection(pos,direction)
				if(posIsInsideMaze(arr,newPos)):
					mazeQueue.enqueue(newPos)#ENQUEUE.
		else:
			pass#INVALID Something Detected...
		arr[pos[0]][pos[1]] = VISITED
	return False#No path found.

def findMazePath_Queue_safe(arr,start_pos,end_pos):
	copyArr = copyMaze(arr)
	#print(copyArr[0] is arr[0],'check')
	return findMazePath_Queue(copyArr,start_pos,end_pos)

###################################################################3
import random as r

def getRandomPos(maxLen,maxWidth):
	return(r.randint(0,maxLen-1),r.randint(0,maxWidth-1))

def copyMaze(maze):
	return [maze[i][:] for i in range(len(maze))]
	#maze[:] doesn't work. Each row becomes SAME!!!!(same object.)
	#FYI) test CODE for above situation:
	#a = [[1,2],[5,3]], b = a[:]
	#a[0][0] = 99
	#print(b)

def genMaze(Length,Width,start_pos,end_pos):
	maze = [[BLOCK for _ in range(Width)] for _ in range(Length)]
	BLOCKS = [(i,j) for i in range(Length) for j in range(Width)]
	maze[start_pos[0]][start_pos[1]] = PATH
	maze[end_pos[0]][end_pos[1]] = PATH
	BLOCKS.remove(start_pos)
	BLOCKS.remove(end_pos)
	#print('FRESH!')
	#print_Maze(maze)
	while(not findMazePath_Stack_safe(maze,start_pos,end_pos)):#Only play with 'copied' version!!!
		#print_Maze(maze)
		pos = BLOCKS.pop(r.randint(0,len(BLOCKS)-1))#Random 'block' select.
		#pos = getRandomPos(Length,Width)
		#while(getPathKind(maze,pos) != BLOCK):#Find the Block!!
		#	pos = getRandomPos(Length,Width)
		maze[pos[0]][pos[1]] = PATH#Change to PATH!
	return maze

###################################################################3
def print_Maze(maze):
	print('#'*10)
	for i in range(len(maze)):
		for j in range(len(maze[i])):
			print(maze[i][j],end = "")
		print('') # '\n'
	print('#'*10)
###################################################################3

###################################################################3

###################################################################3

###################################################################3
import time

def testBubble(datas,repeat):
	Result = [-1 for _ in range(len(datas))]
	for i in range(len(datas)):
		t1 = time.time()
		for _ in range(repeat):
			Bubble_Sort(datas[i][:])
		t2 = time.time()
		Result[i] = t2 - t1
	return Result

def testInsertion(datas,repeat):
	Result = [-1 for _ in range(len(datas))]
	for i in range(len(datas)):
		t1 = time.time()
		for _ in range(repeat):
			Insertion_Sort(datas[i][:],0,len(datas[i])-1)
		t2 = time.time()
		Result[i] = t2 - t1
	return Result
###################################################################3
def testLinearSearch(datas,targets,repeat):
	Result = [-1 for _ in range(len(datas))]
	for i in range(len(datas)):
		t1 = time.time()
		for _ in range(repeat):
			Linear_Search(datas[i],0,len(datas[i])-1,targets[i])
		t2 = time.time()
		Result[i] = t2 - t1
	return Result

def testBinarySearch(datas,targets,repeat):
	Result = [-1 for _ in range(len(datas))]
	for i in range(len(datas)):
		t1 = time.time()
		for _ in range(repeat):
			Binary_Search(datas[i],0,len(datas[i])-1,targets[i])
		t2 = time.time()
		Result[i] = t2 - t1
	return Result
###################################################################3
def testMazeQueue(mazes,start_pos_s,end_pos_s,repeat):
	Result = [-1 for _ in range(len(mazes))]
	for i in range(len(mazes)):
		t1 = time.time()
		for _ in range(repeat):
			findMazePath_Queue_safe(mazes[i],start_pos_s[i],end_pos_s[i])
		t2 = time.time()
		Result[i] = t2 - t1
	return Result

def testMazeStack(mazes,start_pos_s,end_pos_s,repeat):
	Result = [-1 for _ in range(len(mazes))]
	for i in range(len(mazes)):
		t1 = time.time()
		for _ in range(repeat):
			findMazePath_Stack_safe(mazes[i],start_pos_s[i],end_pos_s[i])
		t2 = time.time()
		Result[i] = t2 - t1
	return Result
###################################################################3

import matplotlib.pyplot as plt
###################################################################3
#data_list = [26190, 55443, 9007, 46468, 47513, 67004, 2399, 72845, 51075, 81402]

print('Time Complexity TEST!')
print('Enter N(1 ~ 10^3)')
N = int(input())
if(N > 1000):
	N = 1000
print('N:',N)

print('TEST 1 : Sorting(Bubble / Insertion) : REPETITION 10 times.')

TEST_N_list = [N//4, (N//4)*2, (N//4)*3, N]

data_list_s = [[r.randint(1,1000) for _ in range(k)] for k in TEST_N_list]#Create 'data' list.

#1
Bubble_result = testBubble(data_list_s, 10)

#2
Insertion_result = testInsertion(data_list_s, 10)

plt.figure(1)
plt.plot(TEST_N_list,Bubble_result,label='Bubble_10x')
plt.plot(TEST_N_list,Insertion_result,label='Insertion_10x')
plt.legend()
#################################################################
#3
TARGET_s = [r.choice(data_list) for data_list in data_list_s]

print('TEST 2 : Searching(Linear / Binary) : REPETITION 100 times.')

Linear_Search_result = testLinearSearch(data_list_s, TARGET_s, 100)

#4
for d in data_list_s:
	d.sort()#SORT'em!
Binary_Search_result = testBinarySearch(data_list_s, TARGET_s, 100)

plt.figure(2)
plt.plot(TEST_N_list,Linear_Search_result,label='Linear_Search_100x')
plt.plot(TEST_N_list,Binary_Search_result,label='Binary_Search_100x')
plt.legend()
###################################################################3

#Maze = [[0,0,0,0],[1,1,0,0],[1,1,1,0],[1,1,0,0]]

print('TEST 3 : Maze Path Finding(Stack / Queue) : REPETITION 10 times.')
print('Because of the Shear time it takes,,, Test Data are FIXED!')
lengths = [10,20,30,40]
print(lengths)

print('Generating Mazes..')
Mazes = [genMaze(l,l,(0,0),(l-1,l-1)) for l in lengths]#EPIC!!!
print('Done.')
Start_pos_s = [(0,0) for _ in lengths]
End_pos_s = [(l-1,l-1) for l in lengths]
#print_Maze(Maze)

#5
MazeStack_result = testMazeStack(Mazes,Start_pos_s,End_pos_s,10)

#6
MazeQueue_result = testMazeQueue(Mazes,Start_pos_s,End_pos_s,10)

plt.figure(3)
plt.plot(lengths,MazeStack_result,label='Maze_Stack_10x')
plt.plot(lengths,MazeQueue_result,label='Maze_Queue_10x')
plt.legend()

plt.show()

#print(MazeStack_result)
#print(MazeQueue_result)

input('Waiting..')