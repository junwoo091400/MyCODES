
from __future__ import print_function, division
import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt

import ipdb

def RNN_LSTM(batch_size_in = 5, total_len_in = 30000, pad_len_in = 5, backprop_len_in = 50, state_size_in = 10, num_class_in = 32):
	# total_len_in = (backprop_len_in) * (num_batches)
	# Get inputs.
	batch_size = batch_size_in

	total_series_length = total_len_in
	pad_length = pad_len_in
	truncated_backprop_length = backprop_len_in

	state_size = state_size_in
	num_classes = num_class_in

	num_batches = total_series_length // truncated_backprop_length

	#Model generate
	batchX_placeholder = tf.placeholder(tf.float32, [batch_size, truncated_backprop_length, pad_length])
	batchY_placeholder = tf.placeholder(tf.int32, [batch_size, truncated_backprop_length])

	cell_state = tf.placeholder(tf.float32, [batch_size, state_size])
	hidden_state = tf.placeholder(tf.float32, [batch_size, state_size])
	init_state = tf.nn.rnn_cell.LSTMStateTuple(cell_state, hidden_state)

	# LSTM -> classes.
	W2 = tf.Variable(np.random.rand(state_size, num_classes),dtype=tf.float32)
	b2 = tf.Variable(np.zeros((1, num_classes)), dtype=tf.float32)

	# Unpack columns
	inputs_series = tf.unstack(batchX_placeholder, axis=1)
	labels_series = tf.unstack(batchY_placeholder, axis=1) # Becomes [truncated_len, batch_size]

	# Forward passes
	cell = tf.contrib.rnn.BasicLSTMCell(state_size, state_is_tuple=True)
	states_series, current_state = tf.contrib.rnn.static_rnn(cell, inputs_series, init_state)#Input 'init_state' + 'inputs_series' + 'cell'

	logits_series = [tf.matmul(state, W2) + b2 for state in states_series] #Broadcasted addition
	predictions_series = [tf.nn.softmax(logits) for logits in logits_series]

	losses = [tf.nn.sparse_softmax_cross_entropy_with_logits(logits=logits, labels=labels) for logits, labels in zip(logits_series,labels_series)]
	total_loss = tf.reduce_mean(losses)

	train_step = tf.train.AdagradOptimizer(0.3).minimize(total_loss)

	return (batchX_placeholder, batchY_placeholder, cell_state, hidden_state, current_state, predictions_series, W2, b2, cell, train_step, total_loss)