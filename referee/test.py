'''
PYTHONDONTWRITEBYTECODE=1 python test.py
'''

from utils.compiler import Compiler
from game.game import Game

import sys
sys.dont_write_bytecode = True

bot_0_path = "../bots/mcts_heura/test.cpp"
bot_1_path = "../bots/mcts_heura/bot8.cpp"


flags = [
    "-std=c++20",
    "-O0",
    "-g",                     # debug info
]

compiler = Compiler(bot_0_path, bot_1_path, flags)
[bot_0_exec, bot_1_exec] = compiler.build()

game = Game(bot_0_exec, bot_1_exec, 0)
[bot_0_score, bot_1_score] = game.play_game()
print(bot_0_score, bot_1_score)