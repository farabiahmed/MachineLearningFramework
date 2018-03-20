from abc import ABCMeta, abstractmethod


class Memory(metaclass=ABCMeta):

    @abstractmethod
    def add(self, sample):
        raise NotImplementedError()

    @abstractmethod
    def sample(self, n):
        raise NotImplementedError()

    @abstractmethod
    def update(self, data):
        raise NotImplementedError()

    @abstractmethod
    def length(self):
        raise NotImplementedError()