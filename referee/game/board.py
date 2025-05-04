from typing import List, Tuple
from game.response import Response
from game.game_history import Game_history

class Board:
    def __init__(self, history: Game_history):
        self.game_history = history
        self.ultimate_board = [[None for _ in range(9)] for _ in range(9)]
        self.main_board = [[None for _ in range(3)] for _ in range(3)]
        self.current_player = "x" 
    
    def generate_moves(self, last_x = -1, last_y = -1) -> List[Tuple[int, int]]:
        
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
        
        
        if last_x == -1:
            return generate_moves_in_all_subboards()
        
        next_x, next_y = last_x % 3, last_y % 3
        if self.main_board[next_x][next_y] is not None:
            return generate_moves_in_all_subboards()
        
        return generate_moves_in_subboard(next_x * 3, next_y * 3)
    
    def winner_of_subboard(self, subboard_x, subboard_y) -> str | None:
        ssx = subboard_x * 3
        ssy = subboard_y * 3
         
        for i in range(3):
            if self.ultimate_board[ssx + i][ssy] == self.ultimate_board[ssx + i][ssy + 1] == self.ultimate_board[ssx + i][ssy + 2] and self.ultimate_board[ssx + i][ssy] in ('x', 'o'):
                return self.ultimate_board[ssx + i][ssy]
            if self.ultimate_board[ssx][ssy + i] == self.ultimate_board[ssx + 1][ssy + i] == self.ultimate_board[ssx + 2][ssy + i] and self.ultimate_board[ssx][ssy + i] in ('x', 'o'):
                return self.ultimate_board[ssx][ssy + i]
            
        if self.ultimate_board[ssx][ssy] == self.ultimate_board[ssx + 1][ssy + 1] == self.ultimate_board[ssx + 2][ssy + 2] and self.ultimate_board[ssx][ssy] in ('x', 'o'):
            return self.ultimate_board[ssx][ssy]
        if self.ultimate_board[ssx][ssy + 2] == self.ultimate_board[ssx + 1][ssy + 1] == self.ultimate_board[ssx + 2][ssy] and self.ultimate_board[ssx][ssy + 2] in ('x', 'o'):
            return self.ultimate_board[ssx][ssy + 2]
        
        return None
    
    def is_subboard_full(self, subboard_x, subboard_y) -> bool:
        ssx = subboard_x * 3
        ssy = subboard_y * 3
        
        for i in range(3):
            for j in range(3):
                if self.ultimate_board[ssx + i][ssy + j] is None:
                    return False
        return True
    
    def winner_of_main_board(self) -> str | None:
        for i in range(3):
            if self.main_board[i][0] == self.main_board[i][1] == self.main_board[i][2] and self.main_board[i][0] in ('x', 'o'):
                return self.main_board[i][0]
            if self.main_board[0][i] == self.main_board[1][i] == self.main_board[2][i] and self.main_board[0][i] in ('x', 'o'):
                return self.main_board[0][i]
            
        if self.main_board[0][0] == self.main_board[1][1] == self.main_board[2][2] and self.main_board[0][0] in ('x', 'o'):
            return self.main_board[0][0]
        if self.main_board[0][2] == self.main_board[1][1] == self.main_board[2][0] and self.main_board[0][2] in ('x', 'o'):
            return self.main_board[0][2]
        
        return None
    
    def valid_move(self, x, y, valid_actions: List[Tuple[int, int]]) -> bool:
        if x < 0 or x > 8: return False
        if y < 0 or y > 8: return False
        
        main_i = x // 3 
        main_j = y // 3 
        
        if self.ultimate_board[x][y] is not None: return False
        if self.main_board[main_i][main_j] is not None: return False
        
        return (x, y) in valid_actions
    
    def make_move(self, x, y):
        self.ultimate_board[x][y] = self.current_player
        
        
        subboard_x = x // 3 
        subboard_y = y // 3
         
        winner = self.winner_of_subboard(subboard_x, subboard_y) 
        if winner:
            self.main_board[subboard_x][subboard_y] = winner 
        elif self.is_subboard_full(subboard_x, subboard_y):
            self.main_board[subboard_x][subboard_y] = 'T'
            
    def end_game(self, last_x = -1, last_y = -1):
        if self.winner_of_main_board(): return True
        return self.generate_moves(last_x, last_y) == []
    
    def debug_print_board(self) -> None:
        for i in range(9):
            row = []
            for j in range(9):
                cell = self.ultimate_board[i][j] or '.'
                row.append(cell)
                if j % 3 == 2:
                    row.append(' ')
            print(''.join(row))
            if i % 3 == 2:
                print()
        
        for i in range(3):
            row = []
            for j in range(3):
                cell = self.main_board[i][j] or '.'
                row.append(cell)
            print(''.join(row))
    
    def debug_print_next_moves(self, valid_actions: List[Tuple[int, int]], last_x: int, last_y: int, file = None) -> None:
        if file: file.write(''.join(['@' for _ in range(15)]) + '\n')
        else: print(''.join(['@' for _ in range(15)]))
        
        ub = [[self.ultimate_board[i][j] or '.' for j in range(9)] for i in range(9)]
        for [x, y] in valid_actions:
            if ub[x][y] != '.':
                self.game_history.retrive_game()
                raise ValueError(f"innapropriate valid pos {x} {y}")
            ub[x][y] = '#'
        
        if last_x != -1:
            ub[last_x][last_y] = 'L'
        
        for i in range(9):
            row = ['@ ']
            for j in range(9):
                row.append(ub[i][j])
                if j % 3 == 2:
                    row.append(' ')
            if file: file.write(''.join(row + ['@']) + '\n')
            else: print(''.join(row + ['@']))
            if i % 3 == 2:
                if file: file.write(''.join(['@'] + [' ' for _ in range(13)] + ['@']) + '\n')
                else: print(''.join(['@'] + [' ' for _ in range(13)] + ['@']))
                
        for i in range(3):
            row = ['@ ']
            for j in range(3):
                cell = self.main_board[i][j] or '.'
                row.append(cell)
            if file: file.write(''.join(row + [' ' for _ in range(9)] + ['@']) + '\n')
            else: print(''.join(row + [' ' for _ in range(9)] + ['@']))
        
        if file:
            file.write(''.join(['@' for _ in range(15)]) + '\n')
            file.write('\n')
        else: 
            print(''.join(['@' for _ in range(15)]))
            print()
    
    def validate_players_ultimate_board(self, response: Response):
        for i in range(9):
            for j in range(9):
                if self.ultimate_board[i][j] != response.ultimate_board[i][j]:
                    self.game_history.retrive_game()
                    raise ValueError(f"{self.ultimate_board} {response.ultimate_board}")
    def validate_players_main_board(self, response: Response):
        for i in range(3):
            for j in range(3):
                if self.main_board[i][j] != response.main_board[i][j] and self.main_board[i][j] != 'T':
                    self.game_history.retrive_game()
                    raise ValueError(f"{self.main_board} {response.main_board}")