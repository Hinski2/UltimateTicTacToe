import random
from game.comunicate import Comunicate
from game.board import Board
from game.response import Response
from game.game_history import Game_history

class Game:
    def __init__(self, bot_0_exec: str, bot_1_exec: str):
        self.bot_0_exec = bot_0_exec
        self.bot_1_exec = bot_1_exec
        
        l = [bot_0_exec, bot_1_exec]
        random.shuffle(l)
        self.bot_x_exec = l[0]
        self.bot_o_exec = l[1]
        
        self.game_history = Game_history()
        self.board = Board(self.game_history) 
        
    def play_one_move(self, debug: bool, turbo_debug: bool):
        if self.board.current_player == 'x':
            comunicate = self.comunicate_x 
        else: 
            comunicate = self.comunicate_o
            
        valid_actions = self.board.generate_moves(self.opponent_x, self.opponent_y)
        
        if debug:
            self.board.debug_print_next_moves(valid_actions, self.opponent_x, self.opponent_y)
        
        comunicate.send_message(self.opponent_x, self.opponent_y, len(valid_actions), valid_actions)
        try:
            response: Response = comunicate.get_message(self.timeout)
        except:
            self.game_history.retrive_game()
            raise
        
        self.opponent_x, self.opponent_y = response.move_x, response.move_y
        self.game_history.add_move_to_history(self.opponent_x, self.opponent_y)
            
        if not self.board.valid_move(self.opponent_x, self.opponent_y, valid_actions):
            self.game_history.retrive_game()
            raise ValueError(f"innapropriate response {self.opponent_x} {self.opponent_y} from bot {comunicate.exec_path}")

        self.board.make_move(self.opponent_x, self.opponent_y)
        if turbo_debug:
            response.decode_response(self.board.current_player, 'x' if self.board.current_player == 'o' else 'o', None)
            self.board.validate_players_ultimate_board(response)
            self.board.validate_players_main_board(response) 
        
        
        self.board.current_player = 'x' if self.board.current_player == 'o' else 'o'
            
        self.moves_cnt += 1 
        if self.moves_cnt > 1:
            self.timeout = 0.2
        
    def play_game(self, debug=False, turbo_debug=False) -> tuple[int, int]:
        self.comunicate_x = Comunicate(self.bot_x_exec, turbo_debug)
        self.comunicate_o = Comunicate(self.bot_o_exec, turbo_debug)
        
        self.opponent_x, self.opponent_y = -1, -1
        self.moves_cnt = 0
        self.timeout = 2.0
        
        while not self.board.end_game():
            self.play_one_move(debug, turbo_debug)
            
        if debug:
            self.board.debug_print_board()
        
        winner = self.board.winner_of_main_board()
        if winner is None:
            x, y = 0, 0
            for i in range(3):
                for j in range(3):
                    if self.board.main_board[i][j] == 'x':
                        x += 1 
                    elif self.board.main_board[i][j] == 'o':
                        y += 1 
            winner = 'x' if x >= y else 'o'
            
        self.comunicate_o.kill()
        self.comunicate_x.kill()
        
        if winner == 'x':
            ans = (1, 0) if self.bot_0_exec == self.bot_x_exec else (0, 1)
        else: 
            ans = (1, 0) if self.bot_0_exec == self.bot_o_exec else (0, 1)
    
        return ans