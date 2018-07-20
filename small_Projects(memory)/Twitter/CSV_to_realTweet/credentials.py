# SETS up everything you need for tweepy. Just edit 4 lines below to match your api.

import tweepy

consumer_key = 'Yours'
consumer_secret = 'Yours'
access_key = 'Yours'
access_secret = 'Yours'

#authorize twitter, initialize tweepy
auth = tweepy.OAuthHandler(consumer_key, consumer_secret)
auth.set_access_token(access_key, access_secret)
api = tweepy.API(auth)