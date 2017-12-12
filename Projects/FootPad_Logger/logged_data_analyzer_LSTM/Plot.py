import numpy as np
import matplotlib.pyplot as plt
from Base import FootLog

def Plot_FootLog(footlog):
	Arr = []
	count = 0
	for datum in footlog.Data:#Each Server-uploads...
		for timestamp in datum[2]:#TimeStamp arr in index '2'
			Arr.append(get_TimeStamp_vector(timestamp))
			count += 1
	np_Arr = np.array(Arr)
	tp_Arr = np_Arr.transpose()

	fig, ax = plt.subplots()
	ax.set_title(str(footlog))
	start = footlog.Data[0][0]
	stop = footlog.Data[-1][0]
	delta = TimeDiff(start,stop)
	dt_per_1000Step = round((delta/count)*1000,0)
	ax.set_xlabel(start+'~'+stop +' | ' + str(dt_per_1000Step) + '[sec/1kStep] | ' + str(delta) + '[s], ' + str(count) + '[TimeStamp]')#ITME of upload. (start ~ stop)
	ax.imshow(tp_Arr,aspect='auto',cmap = 'binary')
	plt.show()

if __name__ == '__main__':
	logs = get_Logs('Mega.csv')
	