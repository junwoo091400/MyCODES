'''
<kudos to>
ORIGINAL CODE(http://nitratine.net/get-wifi-passwords-with-python/)
KOREAN_ENCODING_cp949_help(http://guzene.tistory.com/150)
---------------------------------------------------------------------------------
Junwoo HWANG (04/29/2018) Imported for use in 'Korean' windows System.

Pretty Fun to watch all those 'legacy' wifi & it's passwords, still stored inside my computer.
'''

import subprocess

ALL_USER_PROFILE_KOREAN_STRING = '모든 사용자 프로필'

KEY_CONTENT_KOREAN_STRING = '키 콘텐츠'

a = subprocess.check_output(['netsh', 'wlan', 'show', 'profiles']).decode('cp949').split('\n')
a = [i.split(":")[1][1:-1] for i in a if ALL_USER_PROFILE_KOREAN_STRING in i] # ONLY Wi-Fi Names Cutout.
for i in a:
	results = subprocess.check_output(['netsh', 'wlan', 'show', 'profile', i, 'key=clear']).decode('cp949').split('\n')
	results = [b.split(":")[1][1:-1] for b in results if KEY_CONTENT_KOREAN_STRING in b]
	try:
		print ("{:<30}|  {:<}".format(i, results[0]))#VALID Password.
	except IndexError:
		print ("{:<30}|  {:<}".format(i, ""))#Unvalid password (non-existent)

input('Waiting...')
