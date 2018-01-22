import os, sys

'''
180122(Monday) - Written.

<Things I learned>

1) Use 'bytearray' class for Writing in Binary-File.
2) os.path.dirname(str) Doesn't access that file to Check it. It only works internally, with only information given with 'str'.
3) So, os.path.dirname('./Hello/Dir') WILL BE './Hello' . So to EXTRACT directory name, Check if the give nstr is FileName or not!!!

4) os.path.join(a,b) : When b Starts with '\', then the Function Freaks Out, even when a doesn't have '\'. It will not give the result you want!

<What Program Does>

Encrypts File from One Place to Another using simple 'BitWise-inverting(~)' method.
With 'Drag-N-Drop', you can temporarily Encrypt All the File inside one Directory, including Sub-directories if you want.

Note that Since 2-times Encrypted Means Original, this Program will 'decrypt' the Encrypted file And 'encrypt' the Decrypted file.

Pretty Handy, if you want to hide some pictures / files from some folder with a lot of sub-directories, but small-size.

**WARNING : If you use this on directory with Big-Sized File(100Mb ?), open(str,'rb').read() function might BLOW-UP.
**I only use this for small-size file. Never with potentially big-files(Video, ZipFile)
'''

def EncryptTheFile(From,To):
	print('Encrypt From({}) -> ({})'.format(From,To))
	FromFile = open(From,'rb')
	ToFile = open(To,'wb')
	Buf = FromFile.read()
	newBuf = [(~b % 256) for b in Buf] # BITWISE-inversion. Also, don't forget to keep it in (0,256)

	newBuf = bytearray(newBuf) # Make into 'bytearray' for File-Write.
	ToFile.write(newBuf)

	FromFile.close()
	ToFile.close()

def Main():
	print("-AnythingEncrypter- : Basic BitInversion Encrypting.")

	From = input("From Where?(Must be Directory) :")
	#From = From[1:-1] # Drag-and-Drop contains unnecessary " " on both ends. So eliminate them.
	if os.path.isfile(From):
		From = os.path.dirname(From) # Extract ONLY the Directory.
	fromLen = len(From)

	To = input("To Where?(Must be Directory) :")
	#To = To[1:-1] # Drag-and-Drop contains unnecessary " " on both ends. So eliminate them.
	if os.path.isfile(To):
		From = os.path.dirname(To) # Extract ONLY the Directory.

	if (not os.path.isdir(From)) or (not os.path.isdir(To)):
		print("Uh oh, at least one of From/To directory doesn't seem legit...")
		print("FROM: ", From)
		print("TO: ", To)
		return

	if (To in From) and (not To == From):# Own -> Own copying is Allowed(b/c it Works on Same Depth).
		print("Can't do Parent -> Child Directory copying!(Might get recurrent)")
		return

	#'''
	print('Debugging')
	print(From)
	print(To)
	#'''

	Recur = input('Do you want Recurrent Copying?(Y/N)')
	Recur = (Recur == 'Y') or (Recur == 'y')

	if Recur:
		for root, dirs, files in os.walk(From):
			ToDir = To + root[fromLen:] # Crops 'From' string out of 'root'. Then make it under 'To'
			# Reason why not 'os.path.join()' is because root[fromLen:] STARTS WITH '\'. The Function Freaks out when given string starting with '\'.

			# 1) Copy Files.
			for file in files:
				EncryptTheFile(os.path.join(root,file),os.path.join(ToDir,file))
			# 2) Make Directories
			for _dir in dirs:
				os.makedirs(os.path.join(ToDir,_dir))
	else:
		files = [f for f in os.listdir(From) if os.path.isfile(os.path.join(From,f))] # Cherry-Pick File-Names.
		for file in files:
			EncryptTheFile(os.path.join(From,file),os.path.join(To,file))

	print('Program DONE! :)')

if __name__ == "__main__":
	Main()