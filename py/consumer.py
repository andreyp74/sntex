
import threading
import time

class Consumer:

    def __init__(self):
        self.thread = threading.Thread(name='consumer', daemon=True, target=self.consumer_task)
        self.thread.start()

    def stop(self):
        self.thread.join()

    def get_item(self):
        return str(round(time.time()*1000))

    def consumer_task(self):
        while True:
            s = input("Press Enter to continue...\n>")
            if s == 'quit':
                break    

            item = self.get_item()
            print('Receive:', item)


def main():
    consumer = Consumer()
    consumer.stop()


if __name__ == "__main__":
    main()