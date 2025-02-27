#include <bits/stdc++.h>
using namespace std;

/*
    game: ultimate tic tac toe
    algoright: flat mc 
    performacne: 

*/

//! consts
constexpr bool debug = true;
constexpr bool test = false;

//! enums
enum enum_player{ 
    me, 
    foe,
    empty
};

enum enum_ultra_col{
    fi,
    se, 
    th, 
    all
};

//! utils
namespace utils{

};

//! pos
struct struct_pos{
    int_fast32_t bit;
    int_fast32_t ultra_row;
};

constexpr struct_pos pos_of_coord[9][9] = {
    {{0, 0}, {1, 0}, {2, 0},    {9, 0},  {10, 0}, {11, 0},   {18, 0}, {19, 0}, {20, 0}},
    {{3, 0}, {4, 0}, {5, 0},    {12, 0}, {13, 0}, {14, 0},   {21, 0}, {22, 0}, {23, 0}},
    {{6, 0}, {7, 0}, {8, 0},    {15, 0}, {16, 0}, {17, 0},   {24, 0}, {25, 0}, {26, 0}},

    {{0, 1}, {1, 1}, {2, 1},    {9, 1},  {10, 1}, {11, 1},   {18, 1}, {19, 1}, {20, 1}},
    {{3, 1}, {4, 1}, {5, 1},    {12, 1}, {13, 1}, {14, 1},   {21, 1}, {22, 1}, {23, 1}},
    {{6, 1}, {7, 1}, {8, 1},    {15, 1}, {16, 1}, {17, 1},   {24, 1}, {25, 1}, {26, 1}},

    {{0, 2}, {1, 2}, {2, 2},    {9, 2},  {10, 2}, {11, 2},   {18, 2}, {19, 2}, {20, 2}},
    {{3, 2}, {4, 2}, {5, 2},    {12, 2}, {13, 2}, {14, 2},   {21, 2}, {22, 2}, {23, 2}},
    {{6, 2}, {7, 2}, {8, 2},    {15, 2}, {16, 2}, {17, 2},   {24, 2}, {25, 2}, {26, 2}},
};

//! board
namespace board{
    int_fast32_t ultimate_board_me[3];
    int_fast32_t ultimate_board_foe[3];
    int_fast16_t main_baord_me;
    int_fast16_t main_baord_foe;

    // copy for simulation
    int_fast32_t copy_ultimate_board_me[3];
    int_fast32_t copy_ultimate_board_foe[3];
    int_fast16_t copy_main_board_me;
    int_fast16_t copy_main_board_foe;

    constexpr bool winning_board[] = {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    constexpr int_fast32_t board_mask[] = {511, 261632, 133955584, 134217727}; // fi ultra_col, se ultra_col, th ultra_coll, all ultra_coll

    // valid positions that we are given
    int_fast32_t no_valid_pos_from_input;
    pair<int_fast32_t, int_fast32_t> valid_pos_from_input[81];

    static inline bool check_for_win(const int_fast32_t player, int_fast32_t ultimate_row, int_fast32_t ultimate_col){
        if(player == me){
            return winning_board[(ultimate_board_me[ultimate_row] & board_mask[ultimate_col]) << (9 * ultimate_col)];
        }

        return winning_board[(ultimate_board_foe[ultimate_row] & board_mask[ultimate_col]) << (9 * ultimate_col)];
    }
};


//! global functions
// input
static inline void initialize();
static inline void get_input();

// gameplay
static inline void make_move();
static inline int_fast32_t flat_mc(int player, int_fast32_t next_ultimate_rows, int_fast32_t next_ultimate_cols);


//! main
int main(){
    initialize();
    while(true){
        get_input();
        make_move();
    }
}

static inline void initialize(){
};

static inline void get_input(){
    int_fast32_t row, col;
    cin >> row >> col;
    if(debug){
        cerr << row << ' ' << col << endl;
    }

    if(row != -1){
        auto [bit, ultimate_row] = pos_of_coord[row][col];
        board::ultimate_board_foe[ultimate_row] |= (1 << bit);

        if(board::check_for_win(foe, ultimate_row, bit / 9)){
            board::main_baord_foe |= (1 << (3 * ultimate_row + bit / 9));
        }
    }

    cin >> board::no_valid_pos_from_input;
    if(debug){
        cerr << board::no_valid_pos_from_input << endl;
    }
    for(int i = 0; i < board::no_valid_pos_from_input; i++){
        cin >> row >> col;
        board::valid_pos_from_input[i] = {row, col};

        if(debug){
            cerr << row << ' ' << col << endl;
        }
    }
}

static inline bool game_copy_finish(){ //TODO ta funkcja będzie niepotrzebna jak napiszemy generate_moves, da się to zoptymalizować
    // sb won
    if(board::winning_board[board::main_baord_foe] or board::winning_board[board::copy_main_board_me]) return true;

    // main board is full
    if(board::main_baord_foe | board::main_baord_me == 511) return true;

    // ultimate baord is full
    if(board::ultimate_board_me[0] | board::ultimate_board_foe[0] == )
}

static inline int_fast32_t simulate_till_end(int player, int_fast32_t next_ultimate_rows, int_fast32_t next_cols_mask){
    while(!game_copy_finish()){

    } 
}

static int_fast32_t flat_mc(int player, int_fast32_t next_ultimate_rows, int_fast32_t next_cols_mask, int_fast32_t no_sim){
    int_fast32_t wins = 0;
    for(int_fast32_t i = 0; i < no_sim; i++){
        // copy
        for(int_fast32_t j = 0; j < 3; j++){
            board::copy_ultimate_board_foe[j] = board::ultimate_board_foe[j];
            board::copy_ultimate_board_me[j] = board::ultimate_board_me[j];
        }

        board::copy_main_board_me = board::main_baord_me;
        board::copy_main_board_foe = board::main_baord_foe;

        // make sim
        wins += simulate_till_end(player, next_ultimate_rows, next_cols_mask);
    }
}

// returns {next_ultimate_rows, next_cols_mask}
static inline pair<int_fast32_t, int_fast32_t> get_next_board(int_fast32_t bit, int_fast32_t ultimate_row){
    int_fast32_t next_ultiamte_row = (bit % 9) / 3;
    int_fast32_t next_ultimate_col = bit % 3;

    // if subgame is finished
    if(board::main_baord_foe & (1 << next_ultiamte_row) or board::main_baord_me & (1 << next_ultiamte_row))
        return {all, board::board_mask[all]};

    // if there is still palce
    if((~(board::ultimate_board_foe[next_ultiamte_row] | board::ultimate_board_me[next_ultiamte_row])) & board::board_mask[next_ultimate_col]) 
        return {next_ultiamte_row, board::board_mask[next_ultimate_col]};
    
    return {all, board::board_mask[all]};
}

static void make_move(){
    int_fast32_t wins[81] = {0};
    for(int i = 0; i < board::no_valid_pos_from_input; i++){
        auto [bit, ultimate_row] = pos_of_coord[board::valid_pos_from_input[i].first][board::valid_pos_from_input[i].second];
        bool main_board_andujst = false;

        board::ultimate_board_me[ultimate_row] |= (1 << bit);
        if(board::check_for_win(me, ultimate_row, bit / 9)){
            main_board_andujst = true;
            board::main_baord_me |= (1 << (3 * ultimate_row + bit / 9));
        }

        auto [next_ultimate_rows, next_cols_mask] = get_next_board(bit, ultimate_row);
        wins[i] = flat_mc(foe, next_ultimate_rows, next_cols_mask, 1000);
    }  
}