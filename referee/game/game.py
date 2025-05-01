import random
from typing import Tuple, List
from game.comunicate import Comunicate

class Game:
    def __init__(self, bot_0_exec: str, bot_1_exec: str):
        self.bot_0_exec = bot_0_exec
        self.bot_1_exec = bot_1_exec
        
        l = [bot_0_exec, bot_1_exec]
        random.shuffle(l)
        self.bot_x_exec = l[0]
        self.bot_o_exec = l[1]
        
        self.ultimate_board = [[None for _ in range(9)] for _ in range(9)]
        self.main_board = [[None for _ in range(3)] for _ in range(3)]
        self.current_player = "x"
        
    def generate_moves(self, last_x = None, last_y = None) -> List[Tuple[int, int]]:
        
        def generate_moves_in_all_subboards(): 
            possible_moves = []
            for main_i in range(3):
                for main_j in range(3): 
                    if self.main_board[main_i][main_j]: 
                        continue 
                
                    start_i = main_i * 3
                    start_j = main_j * 3 
                    for i in range(3):
                        for j in range(3):
                            if self.ultimate_board[start_i + i][start_j + j] is None:
                                possible_moves.append((start_i + i, start_j + j))
                            
            return possible_moves
        
        def generate_moves_in_subboard(start_x, start_y):
            possible_moves = []
            for i in range(3):
                for j in range(3):
                    if self.ultimate_board[start_x + i][start_y + j] is None:
                        possible_moves.append((start_x + i, start_y + j))
                        
            return possible_moves
        
        
        if last_x == None:
            return generate_moves_in_all_subboards()
        
        next_x, next_y = last_x % 3, last_y % 3
        if self.main_board[next_x][next_y] is not None:
            return generate_moves_in_all_subboards()
        
        return generate_moves_in_subboard(next_x * 3, next_y * 3)
    
    def winner_of_subboard(self, subboard: list[list[str]]) -> str:
        for i in range(3):
            if subboard[i][0] == subboard[i][1] == subboard[i][2] and subboard[i][0]:
                return subboard[i][0]
            if subboard[0][i] == subboard[1][i] == subboard[2][i] and subboard[0][i]:
                return subboard[0][1]
            
        if subboard[0][0] == subboard[1][1] == subboard[2][2] and subboard[0][0]:
            return subboard[0][0]
        if subboard[0][2] == subboard[1][1] == subboard[2][0] and subboard[0][2]:
            return subboard[0][2]
        
        return None
    
    def valid_move(self, x, y):
        if x < 0 or x > 8: return 0 
        if y < 0 or y > 8: return 0 
        
        main_i = x // 3 
        main_j = y // 3 
        
        if self.ultimate_board[x][y] is not None: return 0 
        if self.main_board[main_i][main_j] is not None: return 0
        
        return 1
         
    def make_move(self, x, y):
        self.ultimate_board[x][y] = self.current_player
        
        ssx = (x // 3) * 3
        ssy = (y // 3) * 3 
        subboard = [[self.ultimate_board[ssx + i][ssy + j] for j in range(3)] for i in range(3)]
        
        winner = self.winner_of_subboard(subboard) 
        if winner:
            self.main_board[x // 3][y // 3] = winner
        
        
    def end_game(self, last_x = None, last_y = None):
        if self.winner_of_subboard(self.main_board): return 1
        return self.generate_moves(last_x, last_y) == []
        
    def play_game(self) -> tuple[int, int]:
        comunicate_x = Comunicate(self.bot_x_exec)
        comunicate_o = Comunicate(self.bot_o_exec)
        
        opponent_x, opponent_y = -1, -1
        self.moves_cnt = 0
        self.timeout = 2.0
        
        while not self.end_game():
            if self.current_player == 'x':
                comunicate = comunicate_x 
            else: 
                comunicate = comunicate_o
            
            valid_actions = self.generate_moves(opponent_x if opponent_x != -1 else None, opponent_y if opponent_y != -1 else None)
            comunicate.send_message(opponent_x, opponent_y, len(valid_actions), valid_actions)
            [opponent_x, opponent_y] = comunicate.get_message(self.timeout)
            
            if not self.valid_move(opponent_x, opponent_y):
                raise ValueError(f"innapropriate response {opponent_x} {opponent_y} from bot {comunicate.exec_path}")

            self.make_move(opponent_x, opponent_y)
            self.current_player = 'x' if self.current_player == 'o' else 'o'
            
            self.moves_cnt += 1 
            if self.moves_cnt > 1:
                self.timeout = 0.2
        
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
            
        comunicate_o.kill()
        comunicate_x.kill()
        
        if winner == 'x':
            ans = (1, 0) if self.bot_0_exec == self.bot_x_exec else (0, 1)
        else: 
            ans = (1, 0) if self.bot_0_exec == self.bot_o_exec else (0, 1)
    
        return ans