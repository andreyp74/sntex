
import datetime
import time
import threading
from queue import Queue

class Producer:

    def __init__(self):
        self.shared_queue = Queue()
        self.shared_queue_cond = threading.Condition()

        self.thread = threading.Thread(name='generator', daemon=True, target=self.generator_task)
        self.thread.start()

    def stop(self):
        self.shared_queue.put(None)
        self.thread.join()

    @staticmethod
    def generate_item():
        return str(round(time.time()*1000))

    def generator_task(self):
        print('Start generator')

        while True:
            try:
                with self.shared_queue_cond:

                    #time.sleep(0.1)
                    time.sleep(1)

                    item = self.generate_item()
                    if item is None:
                        break

                    print('Item:', item)
                    self.shared_queue.put(item)

                    print('Notify put queue')
                    self.shared_queue_cond.notifyAll()

            except Exception as e:
                print('Generate task failed:', e)


    def get_item(self):
        try:
            with self.shared_queue_cond:

                while self.shared_queue.empty():
                    print('Queue is empty. Waiting for data...')
                    self.shared_queue.wait()
                else:
                    print('Queue is not empty')
                    item = self.shared_queue.get_nowait()
                    print('Got item', item)

                self.queue.task_done()

        except Exception as e:
            print('Get items failed:', e)


def main():
    producer = Producer()
    
    while True:
        s = input("Press Enter to continue...\n>")
        if s == 'quit':
            break    

        item = producer.get_item()
        print('Receive:', item)
    
    producer.stop()

if __name__ == "__main__":
    main()
