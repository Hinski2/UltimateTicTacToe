'''
PYTHONDONTWRITEBYTECODE=1 python main.py --bot_0 "../bots/mcts/bot3.cpp" --bot_1 "../bots/mcts/bot3.cpp" --games 1000 --batch_size 50
PYTHONDONTWRITEBYTECODE=1 python main.py --bot_0 "../bots/flat_mc/bot1.cpp" --bot_1 "../bots/flat_mc/bot1.cpp" --games 1000 --batch_size 50
PYTHONDONTWRITEBYTECODE=1 python main.py --bot_0 "../bots/mcts/bot3.cpp" --bot_1 "../bots/mcts_tree_reuse/bot4.cpp" --games 1000 --batch_size 50
'''
from utils.compiler import Compiler
from game.game import Game
from typing import Tuple

import argparse
import concurrent.futures

import sys
sys.dont_write_bytecode = True

def parse_args():
    parser = argparse.ArgumentParser(description="Ultimate Tic Tac Toe Referee")
    parser.add_argument("--bot_0", type=str, required=True, help="Path to the first bot source file.")
    parser.add_argument("--bot_1", type=str, required=True, help="Path to the second bot source file.")
    parser.add_argument("--games", type=int, default=100, help="Number of games to play.")
    parser.add_argument("--batch_size", type=int, default=20, help="size of batch.")
    return parser.parse_args() 

def run_game(bot_0_exec: str, bot_1_exec: str) -> Tuple[int, int]:
    game = Game(bot_0_exec, bot_1_exec)
    [bot_0_score, bot_1_score] = game.play_game()
    return [bot_0_score, bot_1_score]

def run_multiple_games(bot_0_exec, bot_1_exec, games, batch_size):
    bot_0_score, bot_1_score = 0, 0
    for _ in range((games + batch_size - 1) // batch_size):
        b0s, b1s = run_batch(bot_0_exec, bot_1_exec, min(games, batch_size))
        bot_0_score += b0s
        bot_1_score += b1s
        games -= batch_size
        print(f"bot0 score: {bot_0_score}, bot1 score: {bot_1_score}")
        
    return [bot_0_score, bot_1_score]

def run_batch(bot_0_exec, bot_1_exec, games, max_workers=8):
    bot_0_score, bot_1_score = 0, 0
    with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
        futures = [
            executor.submit(run_game, bot_0_exec, bot_1_exec) for _ in range(games)
        ]
        
        for future in concurrent.futures.as_completed(futures):
            b0s, b1s = future.result()
            bot_0_score += b0s
            bot_1_score += b1s
    
    return [bot_0_score, bot_1_score]

if __name__ == "__main__":
    # parse command line arguments
    args = parse_args()
    
    bot_0_path = args.bot_0
    bot_1_path = args.bot_1
    games = args.games
    batch_size = args.batch_size
    
    # compile bots 
    flags = ["-std=c++20"]
    compiler = Compiler(bot_0_path, bot_1_path, flags)
    [bot_0_exec, bot_1_exec] = compiler.build()
    
    # run games
    [bot_0_score, bot_1_score] = run_multiple_games(bot_0_exec, bot_1_exec, games, batch_size)
    
    print(f"Bot {bot_0_path} score: {bot_0_score}")
    print(f"Bot {bot_1_path} score: {bot_1_score}")
    
    
    
    