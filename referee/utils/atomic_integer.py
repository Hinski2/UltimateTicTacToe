import threading

class AtomicInteger:
    def __init__(self, initial=0):
        self.value = initial
        self._lock = threading.Lock()

    def increment(self, amount:int = 1) -> None:
        with self._lock:
            self.value += amount

    def get(self) -> int:
        with self._lock:
            return self.value
        
    def get_and_increment(self, amount:int = 1) -> int: 
        with self._lock:
            self.value += amount
            return self.value - amount
