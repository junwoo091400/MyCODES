import csv

import tweepy

from twitter_tool import * # Helper functions

from credentials import *

#July 20th, 2018.

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

def CSV_tweetOut(screen_name):
	total_tweet_count = 0
	error_tweet_count = 0
	auth = tweepy.OAuthHandler(consumer_key, consumer_secret)
	auth.set_access_token(access_key, access_secret)
	api = tweepy.API(auth)
	with open('%s_tweets.csv' % screen_name, 'r', encoding='UTF-8') as f:
		reader = csv.reader(f)
		Data = [row for row in reader]
		print(Data[0],"HEADER")
		for idx in range(len(Data)-1,1,-1):#IN 'REVERSE' order...
			full_text = Data[idx][2]
			if(is_Retweet(full_text)):#SKIP retweets.
				continue
			removed_mentions = remove_Mentions(full_text)
			#print(removed_mentions,'OK?')
			#input('pass?')
			try:
				api.update_status(removed_mentions)#Might raise error for 'duplicate' and stuff...
			except:
				error_tweet_count += 1
				pass
			total_tweet_count += 1
			if(total_tweet_count % 10 == 0):
				print('Error/Total',error_tweet_count,total_tweet_count)

if __name__ == '__main__':
	CSV_tweetOut('elonmusk')