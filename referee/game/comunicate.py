import subprocess 
from typing import Tuple, List
import concurrent.futures
from game.response import Response
from pathlib import Path

class Comunicate:
    def __init__(self, exec_path: str, turbo_debug: bool, seed: int, callgrind: bool = False):
        self.exec_path = exec_path 
        self.seed = seed
        self.callgrind = callgrind
        
        cmd: List[str] = [] 
        if self.callgrind:
            cmd.extend(
                [
                    "valgrind",
                    "--tool=callgrind",
                    "--trace-children=yes",            
                ]
            )
        
        cmd.extend([self.exec_path, str(self.seed)])
        
        self.proc = subprocess.Popen(
            cmd, 
            stdin=subprocess.PIPE, 
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            text=True 
        )
        
        self.executor = concurrent.futures.ThreadPoolExecutor(max_workers=1)
        self.turbo_debug = turbo_debug
        
    def kill(self):
        if self.proc and self.proc.poll() is None:
            self.proc.terminate()
            
            try:
                self.proc.wait(timeout=1)
            except subprocess.TimeoutExpired:
                self.proc.kill()
                
        self.executor.shutdown(wait=False)
    
    def send_message(self, opponent_x: int, opponent_y: int, valid_actions_no: int, valid_actions: List[Tuple[int, int]]):
        if self.proc.stdin:
            self.proc.stdin.write(f"{opponent_x} {opponent_y}\n")
            self.proc.stdin.write(f"{valid_actions_no}\n")
            for action in valid_actions:
                self.proc.stdin.write(f"{action[0]} {action[1]}\n")
            self.proc.stdin.flush()
    
    def get_message(self, timeout: float) -> Response:
        response = Response(self.proc, self.turbo_debug)
        future = self.executor.submit(response.read)
        
        try:
            return future.result(timeout=timeout if not self.callgrind else 500)
        except concurrent.futures.TimeoutError:
            raise TimeoutError(f"timeout error {self.exec_path}")
