
import datetime
import time
import threading
from queue import Queue
from generator import Generator


class Producer:
    def __init__(self, generation_period_ms=200):
        self.end = False

        self.gen = Generator()
        self.generation_period_ms = generation_period_ms

        self.queue = Queue()
        self.thread = threading.Thread(daemon=True, target=self.run)
        self.thread.start()

    def stop(self):
        self.end = True
        self.queue.join()
        self.thread.join()

    def run(self):
        print('Start producer')
        while not self.end:
            try:
                time.sleep(self.generation_period_ms/1000)

                item = self.gen.new_value()
                print('Item:', item)

                self.queue.put(item)

            except Exception as e:
                print('Producer task failed:', e)

    def get_value(self):
        item = self.queue.get(block=True)
        self.queue.task_done()
        return item




