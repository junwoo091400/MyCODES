from Base import FootLog

def Datestr_to_Int(datestr):
	bigStr = ''.join(datestr.split('-'))
	return int(bigStr[2:])#'20' is eliminated![We live in 20th century.]

'''
def Time2Float(timestr,state):
	if(state == DateHandler.BREAKFAST):
	elif(state == DateHandler.LUNCH):
	elif(state == DateHandler.DINNER):
		return TimeDiff('')
	else:
		return -1
'''

def find_1_maxIdx(arr):
	for i in range(len(arr)-1,-1,-1):
		if(arr[i] == 1):
			return i
	return -1 # Not found.

def find_MaxValue_Idx(predict):
	max = 0
	idx = -1
	for i in range(len(predict)):
		if(predict[i]>max):
			max = predict[i]
			idx = i
	return idx

FLAG_1 = 1<<0
FLAG_2 = 1<<1
FLAG_3 = 1<<2
FLAG_4 = 1<<3
FLAG_5 = 1<<4

def Encrpted_to_List5(ts):
	retArr = [0,0,0,0,0]
	if(ts & FLAG_1):
		retArr[0] = 1
	if(ts & FLAG_2):
		retArr[1] = 1
	if(ts & FLAG_3):
		retArr[2] = 1
	if(ts & FLAG_4):
		retArr[3] = 1
	if(ts & FLAG_5):
		retArr[4] = 1
	return retArr

def Timestr_difference_Seconds(start,stop):
	stt = [int(x) for x in start.split(':')]
	stp = [int(x) for x in stop.split(':')]
	delta = (stp[0]-stt[0])*3600 + (stp[1]-stt[1])*60 + (stp[2]-stt[2])
	return delta