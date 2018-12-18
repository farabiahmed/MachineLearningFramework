import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt

# 0. Set Simulation Constants
# ===============================
learning_rate = 0.01
training_epochs = 5000
display_step = 1

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

# Optimizer embedded in API
optimizer = tf.train.GradientDescentOptimizer(learning_rate)
train = optimizer.minimize(cost)

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
        _, c = sess.run([train, cost], feed_dict={x: x_data,
                                                  y: y_data})

        # Display logs per epoch step
        if (epoch + 1) % display_step == 0:
            print ("Epoch:", '%04d' % (epoch + 1), "cost=", "{:.9f}".format(c))

    print ("Optimization Finished!")

    y_predict = sess.run(pred, feed_dict={x: x_data,
                                          y: y_data})

    # Test model
    # correct_prediction = tf.equal(tf.argmax(pred, 1), tf.argmax(y, 1))
    # # Calculate accuracy for 3000 examples
    # accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
    # print ("Accuracy:", accuracy.eval({x: mnist.test.images[:3000], y: mnist.test.labels[:3000]}))

plt.plot(x_data,y_predict,color='Red')
plt.show()