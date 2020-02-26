import random
import sys


class Generator:
    def __init__(self):
        random.seed()

    def new_value(self):
        return random.randint(0, sys.maxsize)

