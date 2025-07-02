'''
PYTHONDONTWRITEBYTECODE=1 python main.py
'''

from utils.compiler import Compiler
from utils.atomic_integer import AtomicInteger
from game.game import Game
from typing import Tuple

import time
import concurrent.futures

import sys
sys.dont_write_bytecode = True

args = {
    "bot0": "../bots/mcts_heura/test.cpp",
    "bot1": "../bots/mcts_heura/bot7.cpp",
    "games": 2500,
    "batch_size": 100,
}

atomic_seed = AtomicInteger()

def run_game(bot_0_exec: str, bot_1_exec: str, seed: int) -> Tuple[int, int]:
    game = Game(bot_0_exec, bot_1_exec, seed)
    [bot_0_score, bot_1_score] = game.play_game()
    return (bot_0_score, bot_1_score)

def run_multiple_games(bot_0_exec, bot_1_exec, games, batch_size):
    bot_0_score, bot_1_score = 0, 0
    
    for _ in range((games + batch_size - 1) // batch_size):
        b0s, b1s = run_batch(bot_0_exec, bot_1_exec, min(args["games"], args["batch_size"]))
        bot_0_score += b0s
        bot_1_score += b1s
        games -= batch_size
        print(f"bot0: {args['bot0']} score: {bot_0_score}, bot1: {args['bot1']} score: {bot_1_score}")
        
    return [bot_0_score, bot_1_score]

def run_batch(bot_0_exec, bot_1_exec, games, max_workers=16):
    bot_0_score, bot_1_score = 0, 0
    seed = 0
    
    with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
        futures = [
            executor.submit(run_game, bot_0_exec, bot_1_exec, atomic_seed.get_and_increment()) for _ in range(games)
        ]
        
        for future in concurrent.futures.as_completed(futures):
            b0s, b1s = future.result()
            bot_0_score += b0s
            bot_1_score += b1s
    
    return [bot_0_score, bot_1_score]

if __name__ == "__main__":
    start = time.time() 
    # compile bots 
    flags = [
        "-std=c++20",
        "-fsanitize=address",     
        "-fsanitize=undefined",   
        "-fno-omit-frame-pointer",
        "-O2"
    ]

    compiler = Compiler(args["bot0"], args["bot1"], flags)
    [bot_0_exec, bot_1_exec] = compiler.build()
    
    # run games
    [bot_0_score, bot_1_score] = run_multiple_games(bot_0_exec, bot_1_exec, args["games"], args["batch_size"])
    
    print(f"Bot {args["bot0"]} score: {bot_0_score}")
    print(f"Bot {args["bot1"]} score: {bot_1_score}")
    
    end = time.time()
    print(f"Elapsed time: {(end - start):.6f} seconds")
    
    
    
    