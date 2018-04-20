Maze_map = [[0 for j in range(25)] for i in range(25)]

M, N = 0, 0

#############################################

Maze_count = [[0 for j in range(25)] for i in range(25)]

Maze_check = [[0 for j in range(25)] for i in range(25)]

#############################################

def findMazePath(x, y):
	if(x > M-1 or x<0 or y>N-1 or y<0):# Out of Map
		return -1
	if(Maze_map[x][y] == 1 or Maze_check[x][y] == 1):# Visited or Wall.
		return -1
	
	if(x == 0 and y == N-1):# TARGET!
		return 0# Found it!

	Maze_check[x][y] = 1

	# do something.
	result = [findMazePath(x+1,y),findMazePath(x,y+1),findMazePath(x-1,y),findMazePath(x,y-1)]

	Maze_check[x][y] = 0 # Uncheck.

	minimum = 9999
	found_min = False

	for res in result:
		if(res != -1):#Valid length
			if(minimum > res):
				minimum = res
				found_min = True

	if(found_min):
		return minimum + 1#Including one Move!
	else:
		return -1# No Path.

#############################################

def print_Map():
	print('#'*10)
	for i in range(M):
		for j in range(N):
			print(Maze_map[i][j],end = "")
		print('') # '\n'
	print('#'*10)

#############################################

## INPUT ##

M, N = map(int,input().split(' ')) # integers from 'String'

#print('Got',M,N)

for i in range(M):
	#input() # Work-around '\n'
	Binary = input()
	#print('Got',Binary)
	#print(Binary)
	if(len(Binary) != N):
		print('Wrong ROW input! (Length not {})'.format(N))
	else:
		for j in range(N):
			Maze_map[i][j] = int(Binary[j])

#############################################

#(N-1, 0)에서 출발하여 (0, M-1)의 위치로 이동#

#print_Map()

print(findMazePath(M-1,0) + 1)#Why +1 needed, you online judge...

#input('Waiting...')

#############################################