import csv

#July 20th, 2018.
#Looks through the 'csv' fil, and reject Retweets. And also removes Mentions(@xxx).

def remove_Mentions(text):
	#'@something' will always be seperated from words with SPACES.
	splited = text.split(' ')
	result = []
	for s in splited:
		#Note, 's' can be ''
		#EXAMPLE : "Model X severely  impacted".
		#Because of consecutive-SPACEs, the s becomes ''. So watch out for that case.
		if(len(s) != 0 and s[0] != '@'):
			result.append(s)#Valid ones.
	return ' '.join(result)#Rejoin with 'spaces'.

def is_Retweet(text):
	if(text[:2] == 'RT'):
		return True
	return False#Else.

#print(is_Retweet('RT df'))
#print(remove_Mentions('sdfs @sdfs etoi sjdfo sdjghlfkjsf @dfhg sd @dghoet \n kkk'))

def CSV_to_TrainData(screen_name):
	Train_data = []
	with open('%s_tweets.csv' % screen_name, 'r', encoding='UTF-8') as f:
		reader = csv.reader(f, delimiter='\t')
		Data = [row for row in reader]
		print(Data[0],"HEADER")
		for idx in range(len(Data)-1,1,-1):#IN 'REVERSE' order...
			full_text = Data[idx][2]
			if(is_Retweet(full_text)):#SKIP retweets.
				continue
			removed_mentions = remove_Mentions(full_text)
			removed_bugs = removed_mentions.replace('&amp;','&')#BUG! straight from Tweepy.
			Train_data.append(removed_bugs)#Append 'string'
	#EXPORT
	import pickle
	with open('%s_traindata' % screen_name,'wb') as f:
		pickle.dump(Train_data,f)

if __name__ == '__main__':
	CSV_to_TrainData('elonmusk')