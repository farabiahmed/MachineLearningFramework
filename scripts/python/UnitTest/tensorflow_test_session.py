import tensorflow as tf
from threading import Thread, Lock

graph_default = tf.get_default_graph()

def ThreadRun():
    print("Thread started.")

    with session2.as_default() as sess:
        sess.run(initialize)
        print(sess.run(variable))  # 42

    print("Thread stopped.")


graph = tf.Graph()
with graph.as_default():
  variable = tf.Variable(42, name='foo')
  initialize = tf.global_variables_initializer()
  assign = variable.assign(13)

session1 = tf.Session(graph=graph)
with session1.as_default() as sess:
    sess.run(initialize)
    sess.run(assign)
    print(sess.run(variable)) # 13

session2 = tf.Session(graph=graph)
with session2.as_default() as sess:
    sess.run(initialize)
    print(sess.run(variable)) # 42


with session1.as_default() as sess:
    print(sess.run(variable)) # 13
    print(tf.get_default_graph() == graph)  # prints false
    print(tf.get_default_graph() == session1.graph)  # prints false, !SURPRISE! since the graph not explicitly given, it will take the default one.
    print(tf.get_default_session() == session1)  # prints true
    print(tf.get_default_graph() == graph_default)  # prints true

    with graph.as_default():
        print(tf.get_default_graph() == session1.graph) # prints true, since the graph explicitly given.

print(tf.get_default_graph() == graph) # prints False since it is something different graph, which is created automatically in default.
print(tf.get_default_session() == session1) # prints False since it is something different graph, which is created automatically in default.

_thread = Thread(target=ThreadRun, args=())
_thread.start()


