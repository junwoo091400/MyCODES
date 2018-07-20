#REMIX of http://nitratine.pythonanywhere.com/blog/python-retweet-bot
#Added ALWAYS_ options for maximum insanity. Plus, handy credentials import feature.

from credentials import *

queries = ["Trump", "Korea", "Retweet", "Mark Zuckerberg", "Twitter", "Elon Musk"]
tweets_per_query  = 30

##
ALWAYS_RETWEET = True
ALWAYS_LIKE = True
ALWAYS_FOLLOW = True
##

new_tweets = 0
for querry in queries:
	print ("Starting new querry: " + querry)
	for tweet in tweepy.Cursor(api.search, q=querry, tweet_mode="extended").items(tweets_per_query ):

		user = tweet.user.screen_name
		id = tweet.id
		url = 'https://twitter.com/' + user +  '/status/' + str(id)
		print (url)

		try:
			text = tweet.retweeted_status.full_text.lower()
		except:
			text = tweet.full_text.lower()
		if ALWAYS_RETWEET or "retweet" in text or "rt" in text:#Flag for 'retweeting'
			if not tweet.retweeted:#Make sure this Message is 'ORIGINAL', not re-tweeted.
				try:
					tweet.retweet()
					print("\tRetweeted")
					new_tweets += 1
				except tweepy.TweepError as e:#Can't retweet 'already' retweeted one.
					print('\tAlready Retweeted')

		if ALWAYS_LIKE or "like" in text or "fav" in text:#Flag for 'Favorite'.
			try:
				tweet.favorite()
				print('\t' + "Liked")
			except:
				print('\tAlready Liked')
		
		if ALWAYS_FOLLOW or "follow" in text:#Flag for 'following.'
			try:
				to_follow = [tweet.retweeted_status.user.screen_name] + [i['screen_name'] for i in tweet.entities['user_mentions']]
			# Don't follow origin user (person who retweeted)
			except:
				to_follow = [user] + [i['screen_name'] for i in tweet.entities['user_mentions']]#ORIGINAL TWEET.

			for screen_name in list(set(to_follow)):
				#print('TRYING baby',screen_name)
				api.create_friendship(screen_name)#FOLLOW!
				print('\t' + "Followed: " + screen_name)

print ("New Tweets: " + str(new_tweets))

input('Waiting...')