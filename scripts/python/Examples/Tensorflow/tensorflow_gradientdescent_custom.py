import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt

# 0. Set Simulation Constants
# ===============================
learning_rate = 0.0001
training_epochs = 5000
display_step = 500

# 1. Prepare Fake Data
# ===============================

# fake data
x_data = np.linspace(-1, 1, 100)[:, np.newaxis]          # shape (100, 1)

noise = np.random.normal(0, 0.5, size=x_data.shape)
y_data = 5 * x_data + noise                          # shape (100, 1) + some noise

plt.scatter(x_data,y_data,color='Blue')


# 2. Prepare Model Structure
# ===============================

# tf Graph Input
x = tf.placeholder(tf.float32, [None, 1])     # input x
y = tf.placeholder(tf.float32, [None, 1])     # label y

# Set model weights
W = tf.Variable(tf.zeros([1, 1]))
b = tf.Variable(tf.zeros([1]))

# Construct model
pred = tf.matmul(x, W) + b  # Linear Regression

# Minimize error
cost = tf.reduce_mean(tf.squared_difference(y, pred))

# Simple Custom Gradient Descent Optimizer
grad_W, grad_b = tf.gradients(xs=[W, b], ys=cost)
new_W = W.assign(W - learning_rate * grad_W)
new_b = b.assign(b - learning_rate * grad_b)

# Initialize the variables (i.e. assign their default value)
init = tf.global_variables_initializer()


# 3. Start Training
# ===============================
            
# Start training
with tf.Session() as sess:
    sess.run(init)

    # Training cycle
    for epoch in range(training_epochs):
            
        
        # Fit training using batch data
        _w, _b, c = sess.run([new_W, new_b, cost], feed_dict={x: x_data,
                                                            y: y_data})

        # Display logs per epoch step
        if (epoch) % display_step == 0:
            print ("Epoch:", '%04d' % (epoch + 1), "cost=", "{:.9f}".format(c))
            y_predict = sess.run(pred, feed_dict={x: x_data,
                                          y: y_data})
            plt.plot(x_data,y_predict,color='Red')
            plt.pause(0.05)
            
            print ("W:",'%03.4f' % (_w), "b:",'%03.4f' % (_b))

    print ("Optimization Finished!")



    # Test model
    # correct_prediction = tf.equal(tf.argmax(pred, 1), tf.argmax(y, 1))
    # # Calculate accuracy for 3000 examples
    # accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
    # print ("Accuracy:", accuracy.eval({x: mnist.test.images[:3000], y: mnist.test.labels[:3000]}))


plt.show()