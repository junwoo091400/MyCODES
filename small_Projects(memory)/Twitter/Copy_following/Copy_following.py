#!/usr/bin/env python
# encoding: utf-8

#July 20th, 2018. 9PM ugh!!

from credentials import * # IDEA from https://www.digitalocean.com/community/tutorials/how-to-create-a-twitterbot-with-python-3-and-the-tweepy-library

def am_already_following_by_id(id):
	#Helped : https://stackoverflow.com/questions/36118490/checking-friendship-in-tweepy
	status = api.show_friendship(source_id = api.me().id, target_id = id)
	return status[0].following#On 'source' viewpoint = 0th of the Tuple.

def copy_Following_by_screen_name(screen_name):
	following_id_list = api.friends_ids(screen_name = screen_name)
	#print(following_list)
	for following_id in following_id_list:
		if(not am_already_following_by_id(following_id)):#Am not following prev.
			print('Following',end=' ')
			api.create_friendship(id = following_id)
		else:
			print('Already following',end=' ')
		user = api.get_user(id = following_id)
		print('name =', user.name, '/ screen name:', user.screen_name)
	print('Done.')

if __name__ == '__main__':
	copy_Following_by_screen_name("elonmusk")