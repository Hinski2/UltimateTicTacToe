import random
from typing import Tuple, List
from game.comunicate import Comunicate
from game.board import Board

class Game:
    def __init__(self, bot_0_exec: str, bot_1_exec: str):
        self.bot_0_exec = bot_0_exec
        self.bot_1_exec = bot_1_exec
        
        l = [bot_0_exec, bot_1_exec]
        random.shuffle(l)
        self.bot_x_exec = l[0]
        self.bot_o_exec = l[1]
        
        self.board = Board() 
        
    def play_one_move(self, debug):
        if self.current_player == 'x':
            comunicate = self.comunicate_x 
        else: 
            comunicate = self.comunicate_o
            
        valid_actions = self.board.generate_moves(self.opponent_x, self.opponent_y)
        
        if debug:
            self.board.debug_print_next_moves(valid_actions, self.opponent_x, self.opponent_y)
        
        comunicate.send_message(opponent_x, opponent_y, len(valid_actions), valid_actions)
        [opponent_x, opponent_y] = comunicate.get_message(self.timeout)
            
        if not self.valid_move(opponent_x, opponent_y):
            raise ValueError(f"innapropriate response {opponent_x} {opponent_y} from bot {comunicate.exec_path}")

        self.make_move(opponent_x, opponent_y)
        self.current_player = 'x' if self.current_player == 'o' else 'o'
            
        self.moves_cnt += 1 
        if self.moves_cnt > 1:
            self.timeout = 0.2
        
    def play_game(self, debug=True) -> tuple[int, int]:
        self.comunicate_x = Comunicate(self.bot_x_exec)
        self.comunicate_o = Comunicate(self.bot_o_exec)
        
        self.opponent_x, self.opponent_y = -1, -1
        self.moves_cnt = 0
        self.timeout = 2.0
        
        while not self.board.end_game():
            self.play_one_move(debug)
        
        winner = self.winner_of_subboard(self.main_board)
        if winner is None:
            x, y = 0, 0
            for i in range(3):
                for j in range(3):
                    if self.main_board[i][j] == 'x':
                        x += 1 
                    elif self.main_board[i][j] == 'o':
                        y += 1 
            winner = 'x' if x >= y else 'o'
            
        self.comunicate_o.kill()
        self.comunicate_x.kill()
        
        if winner == 'x':
            ans = (1, 0) if self.bot_0_exec == self.bot_x_exec else (0, 1)
        else: 
            ans = (1, 0) if self.bot_0_exec == self.bot_o_exec else (0, 1)
    
        return ans