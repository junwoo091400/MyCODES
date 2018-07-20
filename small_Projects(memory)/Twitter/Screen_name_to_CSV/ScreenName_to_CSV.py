#!/usr/bin/env python
# encoding: utf-8

#ORIGIN : https://gist.github.com/yanofsky/5436496#file-tweet_dumper-py
#July 20th, 2018<Friday>.
#Difference : 'twitter_tool' & 'credentials' module support. Also, use TABS in csv. There are a lot of ',' in tweets!!

import tweepy #https://github.com/tweepy/tweepy
import csv

from twitter_tool import * # Helper functions

from credentials import * # IDEA from https://www.digitalocean.com/community/tutorials/how-to-create-a-twitterbot-with-python-3-and-the-tweepy-library

#STRUCTURE of 'status(aka Tweet)' Object : https://gist.github.com/dev-techmoe/ef676cdd03ac47ac503e856282077bf2

'''
<NOTES to myself(Junwoo)>
1. in_reply_to_status_id_str = The 'parent tweet' of that tweet object.
2. in_reply_to_screen_name = Name of the person with 'parent tweet'
3. twt.in_reply_to_user_id = ID of the person with 'parent tweet'
4. entities['user_mentions'] = LIST of users mentioned in the tweet.
'''
def screen_name_To_CSV(screen_name):
	#Twitter only allows access to a users most recent 3240 tweets with this method
	alltweets = get_allTweets_by_screen_name(api, screen_name)
	
	#transform the tweepy tweets into a 2D array that will populate the csv	
	outtweets = [[tweet.id_str, tweet.created_at, tweet.full_text] for tweet in alltweets]

	#write the csv
	#1) AVOID "'cp949' codec" encoding error <UTF-8>
	#2) make NEWLINE ''. So that extra line(\n => \r\n Translation happen...) : https://stackoverflow.com/a/3348664
	with open('%s_tweets.csv' % screen_name, 'w',newline='', encoding='UTF-8') as f:
		writer = csv.writer(f, delimiter='\t')
		writer.writerow(["id","created_at","text"])
		writer.writerows(outtweets)


if __name__ == '__main__':
	#pass in the username of the account you want to download
	screen_name_To_CSV("elonmusk")