from Base import *
from RNN_LSTM import *
from Data_manipulation import *
import ipdb

def log_2_Batch_Y32(log):
	# Encodes Y as 0~31.
	print('log_2_Batch. log Data raw count:',len(log.Data)*200,str(log))
	X = []#Must be 5 seconds/ each.
	Y = []
	idx = 0
	for datum in log.Data:#Each server uploads -> 20 seconds.
		for ts in datum[2]:
			X.append(Encrpted_to_List5(ts))#int -> list.
			Y.append(ts)#RAW int value.
			idx += 1
	X = X[:-5]
	Y = Y[1:]
	print('log_2_Batch. x,y len : ',len(X),len(Y))
	return (X,Y)

def isIgnored(log):
	if(log.State == DateHandler.NOT_VALID):
		return True
	if(log.State != DateHandler.LUNCH):
		return True
	if(Datestr_to_Int(log.Date) < 171107):
		return True
	if(Datestr_to_Int(log.Date) > 171111):
		return True
	return False

def plot(loss_list, predictions_series, batchX, batchY, backprop_length, num_classes):
	plt.subplot(2, 3, 1)
	plt.cla()
	plt.plot(loss_list)

	for batch_series_idx in range(5):
		one_hot_output_series = np.array(predictions_series)[:,batch_series_idx, :]#Becomes (50,32)
		single_output_series = np.array([find_MaxValue_Idx(out) for out in one_hot_output_series])
		#ipdb.set_trace()
		plt.subplot(2, 3, batch_series_idx + 2)
		plt.cla()
		plt.axis([0, backprop_length, 0, num_classes])
		left_offset = range(backprop_length)
		#plt.bar(left_offset, batchX[batch_series_idx, :], width=1, color="blue")
		plt.bar(left_offset, batchY[batch_series_idx, :], width=1, color="red")
		plt.bar(left_offset, single_output_series * 0.5, width=1, color="green")
		#ipdb.set_trace()#REMOVE me.

	plt.draw()
	plt.pause(0.0001)

def main():
	logs = get_Logs('Mega.csv')
	X_bat, Y_bat = [], []
	Log_bat = []
	for flg in logs:
		if( not isIgnored(flg)):
			x,y = log_2_Batch_Y32(flg)
			X_bat.append(x)
			Y_bat.append(y)
			Log_bat.append(flg)
	print('Total X_bat count : ', len(X_bat) )
	#
	backprop_len = 50
	batch_size = len(X_bat)
	total_len = min([len(x_batch) for x_batch in X_bat]) # In 'unit' of 1 TIMESTAMPs!
	#total_len = total_len - total_len % backprop_len # Make it 'multiple' of backprop_len. It is already multiple of 5.
	X_bat = [x_bat[:total_len] for x_bat in X_bat]
	Y_bat = [y_bat[:total_len] for y_bat in Y_bat]#There should exist one y value for each timestamp in X.
	state_size = 10
	num_class = 32
	print('*'*10)
	for log in Log_bat:
		print(str(log)) 
	print('*'*10)
	print('batch_size ',batch_size)
	print('total_len ',total_len)
	print('backprop_len ',backprop_len)
	print('state_size ',state_size)
	print('num_class ',num_class)
	print('*'*10)
	input("These are the details. Proceed?")
	#
	pad_length = 5
	num_epochs = 501 # SET ME!
	#
	batchX_placeholder, batchY_placeholder, cell_state, hidden_state, current_state, predictions_series, W2, b2, cell, train_step, total_loss = RNN_LSTM(batch_size, total_len, pad_length, backprop_len, state_size, num_class)
	loss_printer = open('v4LossPrint.csv','w',1)#Line buffered.
	loss_printer.write('v4epoch_idx,batch_idx,_total_loss\n')
	saver = tf.train.Saver(max_to_keep=None)
	with tf.Session() as sess:
		sess.run(tf.initialize_all_variables())
		plt.ion()
		plt.figure()
		plt.show()
		loss_list = []
		
		x = np.array(X_bat).reshape(batch_size,-1,pad_length)
		y = np.array(Y_bat).reshape(batch_size,-1)
		for epoch_idx in range(num_epochs):
			_current_cell_state = np.zeros((batch_size, state_size))
			_current_hidden_state = np.zeros((batch_size, state_size))
			print("New data, epoch", epoch_idx)
			for batch_idx in range(total_len // backprop_len):
				start_idx = batch_idx * backprop_len
				end_idx = start_idx + backprop_len
				batchX = x[:,start_idx:end_idx]
				batchY = y[:,start_idx:end_idx]

				_total_loss, _train_step, _current_state, _predictions_series = sess.run(
					[total_loss, train_step, current_state, predictions_series],
					feed_dict={
						batchX_placeholder: batchX,
						batchY_placeholder: batchY,
						cell_state: _current_cell_state,
						hidden_state: _current_hidden_state
					})
				_current_cell_state, _current_hidden_state = _current_state
				loss_printer.write('{},{},{}\n'.format(epoch_idx,batch_idx,_total_loss))
				loss_list.append(_total_loss)

				if batch_idx%100 == 0:
					print("Step",batch_idx, "Batch loss", _total_loss)
					plot(loss_list, _predictions_series, batchX, batchY,backprop_len,num_class)

			if(epoch_idx % 10 == 0):
				saver.save(sess, "saver/v4epoch_{}.ckpt".format(epoch_idx))
	plt.ioff()
	plt.show()
if __name__ == '__main__':
	main()