/*
    game: tic tac toe
    algorithm: min-max (optimal)
*/

#include <bits/stdc++.h>
using namespace std;

constexpr bool debug = 0;

enum sign{
    ME, 
    FOE,
    EMPTY
};

constexpr int score_none = 0, score_me = 1, scoere_foe = -1;

int board[3][3];
void initialize(){
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            board[i][j] = EMPTY;
}

void get_input(){
    int row, col; cin >> row >> col; 
    if(row != -1){
        board[row][col] = FOE;
    }

    if(debug){
        cerr << row << ' ' << col << endl;
    }

    int valid_actions_no; cin >> valid_actions_no;
    if(debug) cerr << valid_actions_no << endl;

    for(int i = 0; i < valid_actions_no; i++){
        cin >> row >> col; 
        if(debug) cerr << row << ' ' << col << endl;
    } 
}

vector<pair<int, int>> generate_moves(){
    vector<pair<int, int>> moves;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(board[i][j] == EMPTY) moves.push_back({i, j});
        }
    }

    return moves;
}

int winner(){
    for(int i = 0; i < 3; i++){
        if(board[i][0] == board[i][1] and board[i][1] == board[i][2] and board[i][0] != EMPTY) return board[i][0];
    } 
    
    for(int j = 0; j < 3; j++){
        if(board[0][j] == board[1][j] and board[1][j] == board[2][j] and board[0][j] != EMPTY) return board[0][j];
    } 

    if(board[0][0] == board[1][1] and board[1][1] == board[2][2] and board[2][2] != EMPTY) return board[0][0];
    if(board[2][0] == board[1][1] and board[1][1] == board[0][2] and board[0][2] != EMPTY) return board[2][0];
    return EMPTY;
}

bool endgame(){
    int empty_spaces = 0;
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            empty_spaces += board[i][j] == EMPTY;
    
    if(empty_spaces == 0) return 1;
    if(winner() != EMPTY) return 1;
    return 0;
}

int minMax(int player){
    if(endgame()){
        int w_player = winner();
        if(w_player == EMPTY) return 0;
        if(w_player == ME) return 1;
        return -1;
    }

    vector<pair<int, int>> moves = generate_moves();
    int best_move_idx = 0, best_move_score = (player == ME ? -1 : 1);

    int idx = 0;
    for(auto &[row, col]: moves){
        board[row][col] = player;

        int score = minMax(player^1);
        if(player == ME and score > best_move_score){
            best_move_score = score;
            best_move_idx = idx;
        }
        else if(player == FOE and score < best_move_score){
            best_move_score = score;
            best_move_idx = idx;
        }

        board[row][col] = EMPTY;
        idx++;
    }

    return best_move_score;
}

void play(){
    vector<pair<int, int>> moves = generate_moves();
    int best_move_idx = 0, best_move_score = -1;

    for(int i = 0; i < moves.size(); i++){
        auto &[row, col] = moves[i];
        board[row][col] = ME;

        int score = minMax(FOE);
        if(score > best_move_score){
            best_move_score = score;
            best_move_idx = i;
        }

        board[row][col] = EMPTY;
    }

    board[moves[best_move_idx].first][moves[best_move_idx].second] = ME;
    cout << moves[best_move_idx].first << ' ' << moves[best_move_idx].second << endl;
}

int main(){
    initialize();
    while(true){
        get_input();
        play();
    } 
}