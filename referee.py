import subprocess
import time 
import os
import numpy as np
import concurrent.futures
import random
empty, x_sign, y_sign = range(3) 

class player:
    def __init__(self, bot0exe, bot1exe):
        x = random.randint(1, 2)
        if(x == 1):
            proc1 = subprocess.Popen(bot0exe, stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True, bufsize=)
            
         
    

class Referee:
    # bot0: botX.cpp, bot1: botY.cpp
    def __init__(self, bot0: str, bot1: str) -> None:
        self.compile(os.path.join('bots', bot0), 'bots/bot0')
        self.bot0exe = 'bots/bot0'
        self.compile(os.path.join('bots', bot1), 'bots/bot1')
        self.bot1exe = 'bots/bot1'
        
    def compile(self, program, output):
        compile_command = ['g++', program, '-o', output]
        subprocess.run(compile_command, check=True)
        
    def multi_battles(self, no: int):
        with concurrent.futures.ProcessPoolExecutor(max_workers=10) as executor:
            futures = [executor.submit(self.battles, no) for _ in range(10)]
            results = [future.result() for future in futures]
            
            self.bot_0_wins = sum(r[0] for r in results)
            self.bot_1_wins = sum(r[1] for r in results)
            
        print(self.bot_0_wins, self.bot_1_wins)
    
    def battles(self, no: int):
        ans = (0, 0)
        for _ in range(no):
            b = self.battle()
            ans = (ans[0] + b[0], ans[1] + b[1])
        return ans
    
    def battle(self):
        board = [['.']]
        
ref = Referee('bot0.cpp', 'bot0.cpp')
ref.multi_battles(1)
