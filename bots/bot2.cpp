#include <bits/stdc++.h>
using namespace std;

/*
    game: ultimate tic tac toe
    algoright: mcts
    performacne: ?

*/

//! consts
constexpr bool debug = false;
constexpr bool test = false;
constexpr int_fast32_t inf = 1e9 + 7;
constexpr int_fast32_t max_tree_size = 15'000;

struct struct_node{
    int_fast32_t win;
    int_fast32_t vis;
    int_fast32_t moves[3];
};

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
 //! utils section
    //* time utils section
    uint64_t T0;

    extern "C" {
        int gettimeofday(struct timeval* tv, void* tz);
    }

    uint64_t get_time() { //return time in microsecunds
        struct timeval t;
        utils::gettimeofday(&t, nullptr);
        return static_cast<uint64_t>(t.tv_sec) * 1000000UL + static_cast<uint64_t>(t.tv_usec);
    }

    inline int elapsed_time(){
            return get_time() - T0;
    }

    //* random() utils section
    uint64_t seed=1;
    uint32_t randomRaw(){seed=(0x5DEECE66DUL * seed + 0xBUL) & ((1UL << 48) - 1); return(seed >> 16);}
    uint32_t random_with_boud(const uint bound){return (randomRaw() * uint64_t(bound)) >> 32;}
    uint32_t random(){return (randomRaw() * uint64_t(82)) >> 32;}

    bool all_on_red(double prob){
        return (double) random() / UINT32_MAX < prob;
    }
};

namespace mcts{
    // data
    struct_node tree[max_tree_size];
    


};


//! pos
struct struct_pos{
    uint_fast32_t bit;
    uint_fast32_t ultra_row;
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
    uint_fast32_t ultimate_board_me[3];
    uint_fast32_t ultimate_board_foe[3];
    uint_fast16_t main_baord_me;
    uint_fast16_t main_baord_foe;

    // copy for simulation
    uint_fast32_t copy_ultimate_board_me[3];
    uint_fast32_t copy_ultimate_board_foe[3];
    uint_fast16_t copy_main_board_me;
    uint_fast16_t copy_main_board_foe;

    constexpr bool winning_board[] = {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    constexpr uint_fast32_t board_mask[] = {511, 261632, 133955584, 134217727}; // fi ultra_col, se ultra_col, th ultra_coll, all ultra_coll
    constexpr uint_fast32_t not_board_mask[] = {134217216, 133956095, 262143};

    // valid positions that we are given
    uint_fast32_t no_valid_pos_from_input;
    pair<uint_fast32_t, uint_fast32_t> valid_pos_from_input[81];

    static inline bool check_for_win(const int_fast32_t player, int_fast32_t ultimate_row, int_fast32_t ultimate_col){
        if(player == me){
            return winning_board[(ultimate_board_me[ultimate_row] & board_mask[ultimate_col]) >> (9 * ultimate_col)];
        }

        return winning_board[(ultimate_board_foe[ultimate_row] & board_mask[ultimate_col]) >> (9 * ultimate_col)];
    }

    static inline bool check_for_copy_win(const int_fast32_t player, int_fast32_t ultimate_row, int_fast32_t ultimate_col){
        if(player == me){
            return winning_board[(copy_ultimate_board_me[ultimate_row] & board_mask[ultimate_col]) >> (9 * ultimate_col)];
        }

        return winning_board[(copy_ultimate_board_foe[ultimate_row] & board_mask[ultimate_col]) >> (9 * ultimate_col)];
    }
};

//! global functions
// input
static inline void initialize();
static inline void get_input();

// moves utils
static inline void generate_all_moves(uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols, array<uint_fast32_t, 3> &moves);

// making moves
static inline void make_move();
static inline pair<uint_fast32_t, uint_fast32_t> get_next_board_copy(uint_fast32_t bit);
static inline pair<uint_fast32_t, uint_fast32_t> get_next_board(uint_fast32_t bit);

// bots
static inline int_fast32_t flat_mc(int player, uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols, uint_fast32_t no_sim);
static inline int_fast32_t simulate_till_end(int player, uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols);

//! main
int main(){
    initialize();
    while(true){
        get_input();
        utils::T0 = utils::get_time();

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
    for(uint_fast32_t i = 0; i < board::no_valid_pos_from_input; i++){
        cin >> row >> col;
        board::valid_pos_from_input[i] = {row, col};

        if(debug){
            cerr << row << ' ' << col << endl;
        }
    }
}

static inline void generate_all_moves(uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols, array<uint_fast32_t, 3> &moves){
    moves = {0, 0, 0};
    uint_fast32_t start = (next_ultimate_rows == all ? 0 : next_ultimate_rows), end = (next_ultimate_rows == all ? 2 : next_ultimate_rows);
    for(; start <= end; start++){
        moves[start] = ~(board::copy_ultimate_board_me[start] | board::copy_ultimate_board_foe[start]);

        if((board::copy_main_board_me | board::copy_main_board_foe) & (1 << (start * 3))) moves[start] &= board::not_board_mask[0];
        if((board::copy_main_board_me | board::copy_main_board_foe) & (1 << (start * 3 + 1))) moves[start] &= board::not_board_mask[1];
        if((board::copy_main_board_me | board::copy_main_board_foe) & (1 << (start * 3 + 2))) moves[start] &= board::not_board_mask[2];

        moves[start] &= board::board_mask[next_ultimate_cols];
    }
}

static inline int_fast32_t simulate_till_end(int player, uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols){
    array<uint_fast32_t, 3> moves;
    generate_all_moves(next_ultimate_rows, next_ultimate_cols, moves);

    while(moves[0] | moves[1] | moves[2]){
        uint_fast32_t a = popcount(moves[0]), b = popcount(moves[1]), c = popcount(moves[2]);
        uint_fast32_t sum_of_moves = a + b + c;
        uint_fast32_t choosen_move = utils::random() % sum_of_moves + 1; 
        
        uint_fast32_t start = (next_ultimate_cols == all ? 0 : next_ultimate_cols * 9), end = (next_ultimate_cols == all ? 27 : (next_ultimate_cols + 1) * 9);
        if(choosen_move <= a){
            // get choosen_move'th bit
            for(; start < end; start++){
                if(!(moves[0] & (1 << start))) continue;

                if(choosen_move == 1){
                    // adjust the board
                    auto &board = (player == me ? board::copy_ultimate_board_me[0] : board::copy_ultimate_board_foe[0]);
                    auto &board_main = (player == me ? board::copy_main_board_me : board::copy_main_board_foe);

                    board |= (1 << start);
                    if(board::check_for_copy_win(player, 0, start / 9)){
                        board_main |= (1 << (start / 9));
                    }

                    tie(next_ultimate_rows, next_ultimate_cols) = get_next_board_copy(start);
                    break;
                } else{
                    choosen_move--;
                }
            }
        } else if(choosen_move - a <= b){
            choosen_move -= a;

            // get choosen_move'th bit
            for(; start < end; start++){
                if(!(moves[1] & (1 << start))) continue;

                if(choosen_move == 1){
                    // adjust the board
                    auto &board = (player == me ? board::copy_ultimate_board_me[1] : board::copy_ultimate_board_foe[1]);
                    auto &board_main = (player == me ? board::copy_main_board_me : board::copy_main_board_foe);

                    board |= (1 << start);
                    if(board::check_for_copy_win(player, 1, start / 9)){
                        board_main |= (1 << (3 + start / 9));
                    }
                    tie(next_ultimate_rows, next_ultimate_cols) = get_next_board_copy(start);
                    break;
                } else{
                    choosen_move--;
                }
            }
        } else {
            choosen_move -= a + b;

            // get choosen_move'th bit
            for(; start < end; start++){
                if(!(moves[2] & (1 << start))) continue;

                if(choosen_move == 1){
                    // adjust the board
                    auto &board = (player == me ? board::copy_ultimate_board_me[2] : board::copy_ultimate_board_foe[2]);
                    auto &board_main = (player == me ? board::copy_main_board_me : board::copy_main_board_foe);

                    board |= (1 << start);
                    if(board::check_for_copy_win(player, 2, start / 9)){
                        board_main |= (1 << (6 + start / 9));
                    }
                    tie(next_ultimate_rows, next_ultimate_cols) = get_next_board_copy(start);
                    break;
                } else{
                    choosen_move--;
                }
            }
        }

        generate_all_moves(next_ultimate_rows, next_ultimate_cols, moves);
        player ^= 1;
    } 

    if(board::winning_board[board::copy_main_board_me]) return 1;
    if(board::winning_board[board::copy_main_board_foe]) return -1;
    return popcount(board::copy_main_board_me) > popcount(board::copy_main_board_foe);
}

static int_fast32_t flat_mc(int player, uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols, uint_fast32_t no_sim){
    int_fast32_t wins = 0;
    
    for(uint_fast32_t i = 0; i < no_sim; i++){
        // copy
        for(uint_fast32_t j = 0; j < 3; j++){
            board::copy_ultimate_board_foe[j] = board::ultimate_board_foe[j];
            board::copy_ultimate_board_me[j] = board::ultimate_board_me[j];
        }

        board::copy_main_board_me = board::main_baord_me;
        board::copy_main_board_foe = board::main_baord_foe;

        // make sim
        wins += simulate_till_end(player, next_ultimate_rows, next_ultimate_cols);
    }

    return wins;
}

// returns {next_ultimate_rows, next_ultimate_rows}
static inline pair<uint_fast32_t, uint_fast32_t> get_next_board(uint_fast32_t bit){
    uint_fast32_t next_ultiamte_row = (bit % 9) / 3;
    uint_fast32_t next_ultimate_col = bit % 3;

    // if subgame is finished
    if(board::main_baord_foe & (next_ultiamte_row * 3 + next_ultimate_col) or board::main_baord_me & (next_ultiamte_row * 3 + next_ultimate_col))
        return {all, all};

    // if there is still palce
    if((~(board::ultimate_board_foe[next_ultiamte_row] | board::ultimate_board_me[next_ultiamte_row])) & board::board_mask[next_ultimate_col]) 
        return {next_ultiamte_row, next_ultimate_col};
    
    return {all, all};
}

static inline pair<uint_fast32_t, uint_fast32_t> get_next_board_copy(uint_fast32_t bit){
    uint_fast32_t next_ultiamte_row = (bit % 9) / 3;
    uint_fast32_t next_ultimate_col = bit % 3;

    // if subgame is finished
    if(board::copy_main_board_foe & (1 << (next_ultiamte_row * 3 + next_ultimate_col)) or board::copy_main_board_me & (1 << (next_ultiamte_row * 3 + next_ultimate_col)))
        return {all, all};

    // if there is still palce
    if((~(board::copy_ultimate_board_foe[next_ultiamte_row] | board::copy_ultimate_board_me[next_ultiamte_row])) & board::board_mask[next_ultimate_col]) 
        return {next_ultiamte_row, next_ultimate_col};
    
    return {all, all};
}

static void make_move(){
    int_fast32_t wins[81] = {0};
    while(utils::elapsed_time() < 95'000){
        for(uint_fast32_t i = 0; i < board::no_valid_pos_from_input; i++){
            auto [bit, ultimate_row] = pos_of_coord[board::valid_pos_from_input[i].first][board::valid_pos_from_input[i].second];
            bool main_board_andujst = false;

            board::ultimate_board_me[ultimate_row] |= (1 << bit);
            if(board::check_for_win(me, ultimate_row, bit / 9)){
                main_board_andujst = true;
                board::main_baord_me |= (1 << (3 * ultimate_row + bit / 9));
            }

            auto [next_ultimate_rows, next_ultimate_cols] = get_next_board(bit);

            // remove adjustments
            board::ultimate_board_me[ultimate_row] ^= 1 << bit;
            if(main_board_andujst){
                board::main_baord_me ^= (1 << (3 * ultimate_row + bit / 9));
            }
        }  
    }

    // choose best move
    int best_score = -inf, arg_best_score = 0;
    for(uint_fast32_t i = 0; i < board::no_valid_pos_from_input; i++){
        if(wins[i] > best_score){
            best_score = wins[i];
            arg_best_score = i;
        }
    }

    // make move
    cout << board::valid_pos_from_input[arg_best_score].first << ' ' << board::valid_pos_from_input[arg_best_score].second << endl;
}