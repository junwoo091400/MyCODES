import os, sys, time, tempfile, subprocess
###################################################################

###################################################################


###################################################################

###################################################################
import win32event, win32api, winerror
from winreg import *

AutoRun_reg_str = r'Software\Microsoft\Windows\CurrentVersion\Run'

def get_autorun_key_Values(user, printOption = False):
	# Queries Windows registry for the autorun key value
	# Stores the key values in runkey array
	key = OpenKey(user, AutoRun_reg_str)
	runkey =[]
	try:
		i = 0
		while True:
			subkey = EnumValue(key, i)
			if printOption:
				print(subkey)
			runkey.append(subkey[0])
			i += 1
	except WindowsError:
		pass
	return runkey

def AddProgramToStartup(from_file, keyName = 'Startup_Mixer'):
	user = HKEY_CURRENT_USER
	Existing_autorunKeys = get_autorun_key_Values(user)
	keyVal = AutoRun_reg_str
	key2change= OpenKey(user, keyVal, 0, KEY_ALL_ACCESS)
	SetValueEx(key2change, keyName, 0, REG_SZ, "\"" + from_file + "\"")

def AddProgramToStartup_with_copying(from_file, dest_dir):
	#from_dir = os.path.realpath('')
	from_dir = os.path.dirname(from_file)
	#print('From Dir :', from_dir)
	
	#fp=os.path.dirname(os.path.realpath(__file__))
	if(from_dir != dest_dir):#Don't do same thing twice, file already exists in destination....(?)
		#user = HKEY_LOCAL_MACHINE
		user = HKEY_CURRENT_USER
		Existing_autorunKeys = get_autorun_key_Values(user)
		#print('sysARGV:', sys.argv[0])
		file_name = os.path.basename(from_file)#"maleware.py"

		new_file_path = os.path.join(dest_dir, file_name)

		os.system('copy %s %s'%(from_file, new_file_path))
		#new_file_path = os.path.join(fp, file_name)
		#print (new_file_path)

		keyVal = AutoRun_reg_str
		key2change= OpenKey(user, keyVal, 0, KEY_ALL_ACCESS)

		SetValueEx(key2change, "HacKeD", 0, REG_SZ, "\"" + new_file_path + "\"")

###################################################################
def getFile_line_by_line(from_file):
	ret_list = []
	if(os.path.isfile(from_file)):#Existing File(cache) exists.
		file = open(from_file, 'r')
		ret_list += [line.strip() for line in file.readlines()]#Get rid of '\n'
		file.close()
	return ret_list
###################################################################

###################################################################
###################################################################
###################################################################
###################################################################
def append_Config(config, mixer_dir):
	file = open(mixer_dir, 'a')
	for c in config:
		file.write(c.strip()+'\n')
	file.close()

def save_Config(config, mixer_dir):
	file = open(mixer_dir, 'w')
	for c in config:
		file.write(c.strip()+'\n')
	file.close()
###################################################################
def EDIT(mixer_dir):
	clearScreen()
	cur_config = getFile_line_by_line(mixer_dir)
	if(len(cur_config) == 0):
		print("Mixer doesn't Exist!!")
		time.sleep(1)
		return

	while True:
		clearScreen()
		print('<<EDIT>>')
		idx = 0
		for line in cur_config:
			print('{}\t{}'.format(idx,line))
			idx += 1
		print('Idx You want to Edit?')
		edit_idx = -1
		edit_idx_str = input()
		try:
			edit_idx = int(edit_idx_str)
		except:
			print('wrong Idx Input!')
			time.sleep(1)
			return
		
		if(edit_idx >= len(cur_config)):
			print('Idx INVALID!')
			time.sleep(1)
			return

		clearScreen()
		print('<<EDIT>>')
		print(cur_config[edit_idx])
		print('What do you want to do? D(delete)')
		select = input()
		
		if(select == 'd' or select == 'D'):
			cur_config.pop(edit_idx)
			print('Successfully Deleted.')
			save_Config(cur_config, mixer_dir)
			print('Saving Config COMPLETE')
			time.sleep(1)

		if(len(cur_config) == 0):
			print('No More Entry to Edit!')
			time.sleep(1)
			return
###################################################################
def ADD(mixer_dir):
	while True:
		clearScreen()
		print('<<ADD>>')
		print("INPUT: Folder / File / Command(ex. git) | ENTER to exit.")
		new_cmd = input()

		if(new_cmd == ''):
			print('Exiting ADD menu...')
			time.sleep(1)
			return

		if(new_cmd[-1] == "\""):#Surrounded by~
			if(new_cmd[0] != "\""):
				print('That is impossible...')
				time.sleep(1)
				return
			else:#Successful.
				new_cmd = new_cmd[1:-1]#cut off those ""
		
		if(os.path.isfile(new_cmd)):
			print('Valid File detected.')
			append_Config([new_cmd], mixer_dir)
			print('File Appended.')
		elif(os.path.isdir(new_cmd)):
			print('Valid Dir detected.')
			append_Config(['start ' + new_cmd], mixer_dir)
			print('Dir Appended.')
		else:
			print('Not File or Folder... checking if it is a Valid Command...')
			try:
				#https://stackoverflow.com/questions/25079140/python-subprocess-popen-check-for-success-and-errors
				subprocess.check_call(new_cmd)
				#proc = subprocess.Popen(new_cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
				#subprocess.call(new_cmd, stderr=subprocess.S)
				print('Seems to be working...')
				append_Config([new_cmd], mixer_dir)
				print('Command Appended')
			except:
				print('Something Bad happend...')
				#stdout_value = proc.stdout.read() + proc.stderr.read()
				#print(stdout_value)
			time.sleep(1)
			#input('Continue? Checked the Output / Program Startup?')
###################################################################
def DEVELOPER(mixer_file_name):
	onComputerBootup_Activate_Mixer(mixer_file_name)
###################################################################
def onComputerBootup_Activate_Mixer(mixer_file_name):
	try:
		mixer = getFile_line_by_line(mixer_file_name)
		for command in mixer:
			subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
	except:
		return
###################################################################
###################################################################
def copyProgramAndAddToStartup(program_path,new_dir):
	#cur_dir = os.path.dirname(program_path)
	cur_filename = os.path.basename(program_path)
	new_filename = os.path.join(new_dir,cur_filename)
	os.system('copy %s %s'%(program_path, new_filename))#Successful Copy.
	AddProgramToStartup(new_filename)

def wasExecutedByComputerBootup(program_path,new_dir):
	cur_dir = os.path.dirname(program_path)
	if(cur_dir == new_dir):
		return True
	else:
		return False
###################################################################
###################################################################
###################################################################
def clearScreen():
	os.system('cls')
	#pass
###################################################################
#bat_file_name = 'Junwoo.bat'
#Bat_file_dir = os.path.join(temp_dir, bat_file_name)
mixer_file_basename = 'Mixer_data.txt'
###################################################################
def main():
	temp_dir = tempfile.gettempdir()
	mixer_file_name = os.path.join(temp_dir, mixer_file_basename)

	if(wasExecutedByComputerBootup(sys.argv[0], temp_dir)):
		onComputerBootup_Activate_Mixer(mixer_file_name)
		return

	copyProgramAndAddToStartup(sys.argv[0], temp_dir)
	clearScreen()

	EDIT_menu = False
	ADD_menu = False
	DEVELOPER_menu = False

	while True:
		print('Startup_Mixer by Junwoo HWANG')
		print("MENU\n1) Edit Existing Mixer\n2) Add New Entries\n3) EXIT program\n4) Developer Mode")
		select_num = 0
		select = input()
		try:
			select_num = int(select)
		except:
			print('Wrong Input!')
			time.sleep(1)

		if(select_num == 1):
			EDIT_menu = True
		elif(select_num == 2):
			ADD_menu = True
		elif(select_num == 3):
			break#Get out of the program.
		elif(select_num == 4):
			DEVELOPER_menu = True
		else:
			print('byPassing input command...')

		if(EDIT_menu):
			EDIT(mixer_file_name)
			EDIT_menu = False

		if(ADD_menu):
			ADD(mixer_file_name)
			ADD_menu = False

		if(DEVELOPER_menu):
			DEVELOPER(mixer_file_name)
			DEVELOPER_menu = False

		#time.sleep(1)
		clearScreen()
###################################################################
if __name__ == '__main__':
	main()
###################################################################