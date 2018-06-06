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
			print('Upgraded size to', self.maxSize)

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

class BST:
	def __init__(self):
		self.root = None
	def insert(self, node):
		x = self.root
		y = None
		while(x != None):
			y = x
			if(x.key > node.key):
				x = x.left
			else:
				x = x.right

		if(y == None):
			self.root = node#First node ever, congrats.
		elif(y.key > node.key):
			y.left = node
		else:
			y.right = node#IF, same, just let it flow into RIGHT.... lol.

		node.parent = y # Make sure to set Parent!!!!

	def inOrder_print(self):
		if(self.root != None):
			self.root.middlePrint()
	def preOrder_print(self):
		if(self.root != None):
			self.root.prePrint()
	def postOrder_print(self):
		if(self.root != None):
			self.root.postPrint()
	def levelOrder_print(self):
		queue = Queue()
		queue.enqueue(self.root)
		while(queue.size() > 0):
			node = queue.dequeue()
			if(node != None):
				print(node.key)
				queue.enqueue(node.left)
				queue.enqueue(node.right)

	def getTotalNodeCount(self):
		if(self.root != None):
			return self.root.getTotalNodeCount()
		else:
			return 0
	def getHeight(self):
		if(self.root != None):
			return self.root.getHeight()
		else:
			return 0
	def getTotalTerminalNodeCount(self):
		if(self.root != None):
			return self.root.getTotalTerminalNodeCount()
		else:
			return 0


class Node:
	def __init__(self,key = None):
		self.key = key
		self.parent = None
		self.left = None
		self.right = None
	def addLeftNode(self,node):
		self.left = node
		node.parent = self
	def addRightNode(self,node):
		self.right = node
		node.parent = self
	def getHeight(self):
		leftHeight, rightHeight = 0, 0
		if(self.left != None):
			leftHeight = self.left.getHeight()
		if(self.right != None):
			rightHeight = self.right.getHeight()
		return 1 + max(leftHeight, rightHeight)#Count this Node 'itself' as height 1.
	def getTotalNodeCount(self):
		Count = 1
		if(self.left != None):
			Count += self.left.getTotalNodeCount()
		if(self.right != None):
			Count += self.right.getTotalNodeCount()
		return Count
	def getTotalTerminalNodeCount(self):
		if(self.left == None and self.right == None):
			return 1
		Count = 0
		if(self.left != None):
			Count += self.left.getTotalTerminalNodeCount()
		if(self.right != None):
			Count += self.right.getTotalTerminalNodeCount()
		return Count

	def prePrint(self):
		print(self.key)
		if(self.left != None):
			self.left.prePrint()
		if(self.right != None):
			self.right.prePrint()
	def postPrint(self):
		if(self.left != None):
			self.left.postPrint()
		if(self.right != None):
			self.right.postPrint()
		print(self.key)
	def middlePrint(self):
		if(self.left != None):
			self.left.middlePrint()
		print(self.key)
		if(self.right != None):
			self.right.middlePrint()

	def calculator_prePrint(self):
		print(self.key,end='')
		if(self.left != None):
			self.left.calculator_prePrint()
		if(self.right != None):
			self.right.calculator_prePrint()
	def calculator_postPrint(self):
		if(self.left != None):
			self.left.calculator_postPrint()
		if(self.right != None):
			self.right.calculator_postPrint()
		print(self.key,end='')
	def calculator_middlePrint(self):
		print('(',end='')
		if(self.left != None):
			self.left.calculator_middlePrint()
		print(self.key,end='')
		if(self.right != None):
			self.right.calculator_middlePrint()
		print(')',end='')

def oneDigitOnly_postfix_to_expression_tree(postfix):
	stack = []
	for i in postfix:
		node = Node(i)
		ch_ord = ord(i)
		if(ch_ord >= ord('0') and ch_ord <= ord('9')):#Number.
			stack.append(node)
		else:#Operator....
			num1 = stack.pop()
			num2 = stack.pop()
			node.left = num2
			node.right = num1
			stack.append(node)
	#print('Should be 1,',len(stack))
	if(len(stack) != 1):
		print('Stack len at the end of ET() is',len(stack),'Something is Wrong....')
		return None
	else:
		return stack.pop()

#1
Animal_list = ['fox','bear','goose','ant','dog','hippo','cat','eagle','iguana']

animal_bst = BST()

for a in Animal_list:
	animal_bst.insert(Node(a))

print('inOrder***')
animal_bst.inOrder_print()
print('\npreOrder***')
animal_bst.preOrder_print()
print('\npostOrder***')
animal_bst.postOrder_print()
print('\nlevelOrder***')
animal_bst.levelOrder_print()
print('\n')

#2
pf = input('Enter POSTFIX equation(ex. 235*+) : ')#235*+'
pf_node = oneDigitOnly_postfix_to_expression_tree(pf)
if(pf_node != None):
	print('inOrder***')
	pf_node.calculator_middlePrint()
	print('\npreOrder***')
	pf_node.calculator_prePrint()
	print('\npostOrder***')
	pf_node.calculator_postPrint()
else:
	print('WRONG postfix input:',pf)

#3
#mf = input('Enter MiddleFIX equation(ex. ')

#4
print('\n\n\nFolder Time...')
import os
def get_size(start_path = '.'):
	total_size = 0
	for dirpath, dirnames, filenames in os.walk(start_path):
		for f in filenames:
			fp = os.path.join(dirpath, f)
			total_size += os.path.getsize(fp)
	return total_size
Dir_parent = '/Users/기본'
Dir_list = ['Music', 'My Documents','Downloads','Videos']

for d in Dir_list:
	target_dir = os.path.join(Dir_parent, d)
	print('SIZE of',target_dir,':',end='')
	size = get_size(target_dir)
	if(size < 2**10):
		print(size,'Bytes')
	elif(size < 2**20):
		print(size//(2**10),'KBytes')
	elif(size < 2**30):
		print(size//(2**20),'MBytes')
	elif(size < 2**40):
		print(size//(2**30),'GBytes')
	else:
		print(size,'Bytes')

#5
print('\n\nFor Example... lets calculate some characteristics of the BST created with Animal Names...')

print('Total Node Count :', animal_bst.getTotalNodeCount())
print('Total Terminal Node Count :', animal_bst.getTotalTerminalNodeCount())
print('Height of Tree :', animal_bst.getHeight())