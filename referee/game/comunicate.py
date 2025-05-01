import subprocess 
from typing import Tuple, List
import concurrent.futures

class Comunicate:
    def __init__(self, exec_path: str):
        self.exec_path = exec_path 
        self.proc = subprocess.Popen(
            [self.exec_path], 
            stdin=subprocess.PIPE, 
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            text=True 
        )
        
        self.executor = concurrent.futures.ThreadPoolExecutor(max_workers=1)
        
    def kill(self):
        if self.proc and self.proc.poll() is None:
            self.proc.kill()
        self.executor.shutdown(wait=False)
    
    def send_message(self, opponent_x: int, opponent_y: int, valid_actions_no: int, valid_actions: List[Tuple[int, int]]):
        if self.proc.stdin:
            self.proc.stdin.write(f"{opponent_x} {opponent_y}\n")
            self.proc.stdin.write(f"{valid_actions_no}\n")
            for action in valid_actions:
                self.proc.stdin.write(f"{action[0]} {action[1]}\n")
            self.proc.stdin.flush()
    
    def read_response(self) -> Tuple[int, int]:
        try:
            if self.proc.stdout:
                response = self.proc.stdout.readline().strip()
                if response:
                    x, y = map(int, response.split())
                    return x, y
        except Exception as e:
            raise ValueError(f"Error reading response: {e}")
    
    def get_message(self, timeout: float) -> Tuple[int, int]:
        future = self.executor.submit(self.read_response)
        
        try:
            return future.result(timeout=timeout)
        except concurrent.futures.TimeoutError:
            raise TimeoutError(f"timeout error {self.exec_path}")
