from copy import deepcopy 
from random import choice

empty, me, foe = range(3)
board = [[empty for _ in range(3)] for _ in range(3)]

def win(player, b):
    for i in range(3):
        if all([b[i][j] == player for j in range(3)]):
            return 1
        
    for j in range(3):
        if all([b[i][j] == player for i in range(3)]) :
            return 1
    return 1 if b[0][0] == b[1][1] == b[2][2] == player or b[0][2] == b[1][1] == b[2][0] == player else 0

def generate_moves(b):
    moves = []
    for i in range(3):
        for j in range(3):
            if b[i][j] == empty:
                moves.append((i, j))
    return moves 

def can_continue_game(b):
    return any([b[i][j] == empty for i in range(3) for j in range(3)]) and not win(me, b) and not win(foe, b)

def simulate(no_sim):
    wins = 0
    
    for _ in range(no_sim):
        board_copy = deepcopy(board)
        player = foe
        
        while can_continue_game(board_copy):
            moves = generate_moves(board_copy)
            row, col = choice(moves)
            board_copy[row][col] = player 
            player = me if player == foe else foe
            
        if win(me, board_copy):
            wins += 1
        elif win(foe, board_copy):
            wins -= 2
            
    return wins

if __name__=="__main__":
    while True:
        opponent_row, opponent_col = [int(i) for i in input().split()]
        valid_action_count = int(input())
       
        va = [] 
        for _ in range(valid_action_count):
            row, col = [int(act) for act in input().split()]
            va.append((row, col))
            
        if opponent_row != -1:
            board[opponent_row][opponent_col] = foe
        
        moves = generate_moves(board) 
        ans = [0] * len(moves)
        
        for i in range(len(moves)):
            row, col = moves[i]
            
            board[row][col] = me  
            ans[i] = simulate(100) 
            board[row][col] = empty
        
        best = ans.index(max(ans))
        row, col = moves[best]
        board[row][col] = me
        print(row, col)
            