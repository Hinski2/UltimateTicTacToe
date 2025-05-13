from typing import List, Tuple

class Game_history:
    def __init__(self):
        self.moves_history: List[Tuple[int, int]] = []
        
    def add_move_to_history(self, x: int, y: int):
        self.moves_history.append((x, y))
        
    def retrive_game(self):
        from game.board import Board
        with open("../logs.txt", "w") as f:
            board = Board(history=self)
            
            last_x, last_y = -1, -1 
            for [x, y] in self.moves_history:
                valid_actions = board.generate_moves(last_x, last_y)
                board.debug_print_next_moves(valid_actions, last_x, last_y, f)
                board.make_move(x, y)
                last_x, last_y = x, y
                board.current_player = 'x' if board.current_player == 'o' else 'o'