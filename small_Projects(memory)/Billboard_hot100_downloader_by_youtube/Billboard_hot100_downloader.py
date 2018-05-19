'''
A Program that scrapes the billboard top 100 list and then search for that title/artist on Youtube,
then crudely selects the most-likely-to-be-music url out of the search results, then downloads all
the songs into a designated folder.

Created on 2018/May/18 by Junwoo HWANG.

References
------------------------------------------------------------------------------------------------
1. Returning list from Youtube Query Search : https://stackoverflow.com/questions/29069444/returning-the-urls-as-a-list-from-a-youtube-search-query
2. Downloading Youtube mp3 files : http://willdrevo.com/downloading-youtube-and-soundcloud-audio-with-python-and-pandas
3. Billboard charts package : https://github.com/guoguo12/billboard-charts
4. Check if file exists(or, is a file) : https://stackoverflow.com/questions/82831/how-to-check-whether-a-file-exists
5. Getting current time in format : https://stackoverflow.com/questions/415511/how-to-get-current-time-in-python
6. Youtube-dl options : https://github.com/rg3/youtube-dl/blob/master/youtube_dl/YoutubeDL.py
------------------------------------------------------------------------------------------------

Note
------------------------------------------------------------------------------------------------
- maybe use '--no-mtime' option to set time-stamp of Files as the time of creation on memory...

+) There is a Program that collects the most 'likely' to be music from youtube in
more thoughtful way! => (https://github.com/ritiek/spotify-downloader)

+) There EXISTS a project that 'automatically creates spotify playlist by using billboard module',
=> (http://aguo.us/writings/spotify-billboard.html)

+) So if I just add the 'downloader' part, the whole top_100 would be downloaded with album cover
and everything... Muucchhh better!
------------------------------------------------------------------------------------------------
'''

import urllib.request, urllib.parse, urllib.error
import urllib.request, urllib.error, urllib.parse
from bs4 import BeautifulSoup

import youtube_dl
import os

import billboard

import time

################################################################################

def get_vid_links_by_Search(text):
	query = urllib.parse.quote(text)
	url = "https://www.youtube.com/results?search_query=" + query
	response = urllib.request.urlopen(url)
	html = response.read()
	soup = BeautifulSoup(html, "html.parser")

	hrefs = [vid['href'] for vid in soup.findAll(attrs={'class':'yt-uix-tile-link'})]
	vid_links = [('https://www.youtube.com' + l) for l in hrefs if (l[:9] == '/watch?v=') ]#Select only 'videos'. Not Channel or anything.
	#print(('https://www.youtube.com' + vid['href']))

	return vid_links

################################################################################
#############################
Default_save_folder = 'MP3_files'

# create YouTube downloader
options = {
	'format': 'bestaudio/best', # choice of quality
	'extractaudio' : True,      # only keep the audio
	'audioformat' : "mp3",      # convert to mp3 
	'outtmpl': '%(id)s',        # name the file the ID of the video
	'noplaylist' : True,       # only download single song, not playlist
	'quiet' : True    #Do not print messages to stdout.
	}

ydl = youtube_dl.YoutubeDL(options)

def delete_files_in_folder_by_asking(folder_name):
	listdirs = os.listdir(folder_name)
	if(len(listdirs) == 0):#Nothing to erase or worry about here.
		return

	print('\n\nWARNING! the files in', folder_name, 'will be DELETED!!!')
	print('SAMPLE files inside include:')
	for i in range( min( 7, len(listdirs) ) ):#Might not have enough(7) items to show, So....
		print(listdirs[i])
	answer = input('\nAre you sure? (Y/N)')
	if(answer != 'y' and answer !='Y'):
		print('Answer was not Yes, so not gonna delete them!')
		return

	for the_file in listdirs:
		file_path = os.path.join(folder_name, the_file)
		try:
			if os.path.isfile(file_path):
				os.unlink(file_path)
			#elif os.path.isdir(file_path): shutil.rmtree(file_path)
		except Exception as e:
			print(e)

def download_mp3_files(link_list, wanted_filename, folder_name = Default_save_folder):
	
	if(len(link_list) != len(wanted_filename)):
		print("ERROR : Links and Filenames list doesn't have Equal Length!")
		print('Links', len(link_list), ', Filenames', len(wanted_filename))
		return
	
	if not os.path.exists(folder_name):#If this directory doesn't exist.
		os.makedirs(folder_name)
	else:#Already existed....
		delete_files_in_folder_by_asking(folder_name)#Attempt to delete if existing files by asking permission!!

	for i in range(len(link_list)):
		print('-> Downloading',wanted_filename[i])#Some visual signal to alarm the user...
		Target_file_dir = os.path.join(folder_name, wanted_filename[i] + '.mp3')

		if(os.path.exists(Target_file_dir)):#If it exists either in Folder / File... Don't mess with it.
			print('File',Target_file_dir,'already exists, backing off...')
			continue
		else:
			result = ydl.extract_info(link_list[i], download=True)#RICH of INFORMATION about that VIDEO tho!!!!!!
			os.rename(result['id'], Target_file_dir) # Move file.

################################################################################

def Scrape_latest_Billboard_Top_100(max_count = 10, save_links_to_txt = False):
	print('Scrape_latest_Billboard_Top_100 STARTING...')

	counter = 0
	Links = []
	FileNames = []

	YMDHMS_str = time.strftime("%Y-%m-%d_%H%M%S", time.gmtime())#TIMESTAMP.
	Save_link_txt_dir = 'hot-100_' + YMDHMS_str + '.txt'#BASIC filename...
	link_save_stream = open(Save_link_txt_dir, 'wt')

	chart = billboard.ChartData('hot-100')
	Start_time = time.time()
	for song in chart:
		search_text = song.artist + ' - ' + song.title
		print('Adding candidate :', search_text)#Friendly message
		link_candidates = get_vid_links_by_Search(search_text)
		Links.append(link_candidates[0])#Most likely to be the official release(most likely MV)
		FileNames.append(search_text)

		if(save_links_to_txt):
			#Format, argh. did %s first time. got %s,%s,%s... annoying.
			Formated_str = '{},{},{}\n'.format(song.artist,song.title,link_candidates[0])
			link_save_stream.write(Formated_str)

		counter += 1
		if(counter % 5 == 0):#Show some stats.
			Sec_per_candidate = (time.time() - Start_time) / counter
			print('Counter:',counter,'/',max_count)
			print('Sec per selecting Candidate:', Sec_per_candidate)
			print('Remaining time:', (max_count - counter) * Sec_per_candidate, 'Seconds' )

		if(counter >= max_count):
			break#Out of for statement.
	
	if(save_links_to_txt):
		link_save_stream.close()
	
	download_mp3_files(Links, FileNames, 'Hot_100')#DOWNLOAD!

################################################################################

#first = get_vid_links_by_Search('Rick Rolling')[0]
#print(get_vid_links_by_Search('HAHAHA'))
#download_mp3_files([first], ['BlahBlah'])

Scrape_latest_Billboard_Top_100(100, True)

input('Waiting...')

################################################################################