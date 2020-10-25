import gym
env = gym.make("CartPole-v0")

print(env.action_space)
print(env.action_space.shape)
print(env.action_space.shape[0])