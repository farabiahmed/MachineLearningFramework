import sys

try:
	# theano
	import theano
	print('theano: %s' % theano.__version__)
except:
	print('Theano not found')
	
try:
	# tensorflow
	import tensorflow
	print('tensorflow: %s' % tensorflow.__version__)
except:
	print('Tensorflow not found')
	
try:
	# keras
	import keras
	print('keras: %s' % keras.__version__)
except:
	print('Keras not found')
	
try:
	# scipy
	import scipy
	print('scipy: %s' % scipy.__version__)
except:
	print('Scipy not found')
	
	
try:
	# numpy
	import numpy
	print('numpy: %s' % numpy.__version__)
except:
	print('Numpy not found')

try:
	# matplotlib
	import matplotlib
	print('matplotlib: %s' % matplotlib.__version__)
except:
	print('Matplotlib not found')

try:
	# pandas
	import pandas
	print('pandas: %s' % pandas.__version__)
except:
	print('Pandas not found')
	
try:
	# statsmodels
	import statsmodels
	print('statsmodels: %s' % statsmodels.__version__)
except:
	print('Statsmodels not found')
	
try:
	# scipy
	import scipy
	print('scipy: %s' % scipy.__version__)
except:
	print('scipy not found')

try:
	# scikit-learn
	import sklearn
	print('sklearn: %s' % sklearn.__version__)
except:
	print('Scikit-learn not found')


try:
	# tflearn
	import tflearn
	print('tflearn: %s' % tflearn.__version__)
except:
	print('tflearn not found')


