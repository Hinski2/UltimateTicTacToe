class Response():
    def __init__(self, proc, turbo_debug: bool):
        self.proc = proc
        self.turbo_debug = turbo_debug
        self.ultimate_board = []
        self.main_board = []
    
    def read(self) -> "Response":
        line = self.proc.stdout.readline().strip()
        self.move_x, self.move_y = map(int, line.split())
                
        if self.turbo_debug:
            for _ in range(9):
                line = self.proc.stdout.readline().strip()
                line = [int(i) for i in line.split()]
                self.ultimate_board.append(line)

            for _ in range(3):
                line = self.proc.stdout.readline().strip()
                line = [int(i) for i in line.split()]
                self.main_board.append(line)
                
        return self
        
    def decode_response(self, me, foe, empty):
        transform = {
            0: me,
            1: foe, 
            2: empty
        }
        
        for i in range(9):
            for j in range(9):
                self.ultimate_board[i][j] = transform[self.ultimate_board[i][j]]
        
        for i in range(3):
            for j in range(3):
                self.main_board[i][j] = transform[self.main_board[i][j]]
         
    