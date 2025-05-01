#include <bits/stdc++.h>
#include <immintrin.h>
using namespace std;

/*
    game: ultimate tic tac toe
    algoright: mcts
    performacne: ?

*/

#pragma GCC optimize("Ofast,inline,tracer")
#pragma GCC optimize("unroll-loops,vpt,split-loops,unswitch-loops") 

//! consts
constexpr bool debug = true;
constexpr bool test = false;
constexpr int_fast32_t no_sim = 5;
constexpr int_fast32_t inf = 1e9 + 7;
constexpr int_fast32_t max_nodes = 50'000;

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

//! node
struct my_vector{
    int_fast32_t size;
    int_fast32_t arr[81];
};

int_fast32_t nodes_size;
struct struct_node{
    int_fast32_t priev;                 // prievious state
    int_fast32_t wins, vis;
    int_fast32_t bit, ultimate_row;     // new bit on ultimate_board[ultimate_column]
    int_fast32_t main_bit;              // new bit on a main_baord
    my_vector next;                     // next states indexes
    int_fast32_t player;
} nodes[max_nodes];

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

constexpr pair<int, int> coord_of_pos[3][27] = { // {ultimate_row, bit} -> {row, col}
    {
        {0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2},
        {0, 3}, {0, 4}, {0, 5}, {1, 3}, {1, 4}, {1, 5}, {2, 3}, {2, 4}, {2, 5},
        {0, 6}, {0, 7}, {0, 8}, {1, 6}, {1, 7}, {1, 8}, {2, 6}, {2, 7}, {2, 8}
    },
    {  
        {3, 0}, {3, 1}, {3, 2}, {4, 0}, {4, 1}, {4, 2}, {5, 0}, {5, 1}, {5, 2},
        {3, 3}, {3, 4}, {3, 5}, {4, 3}, {4, 4}, {4, 5}, {5, 3}, {5, 4}, {5, 5},
        {3, 6}, {3, 7}, {3, 8}, {4, 6}, {4, 7}, {4, 8}, {5, 6}, {5, 7}, {5, 8}
    },
    {   
        {6, 0}, {6, 1}, {6, 2}, {7, 0}, {7, 1}, {7, 2}, {8, 0}, {8, 1}, {8, 2},
        {6, 3}, {6, 4}, {6, 5}, {7, 3}, {7, 4}, {7, 5}, {8, 3}, {8, 4}, {8, 5},
        {6, 6}, {6, 7}, {6, 8}, {7, 6}, {7, 7}, {7, 8}, {8, 6}, {8, 7}, {8, 8}
    }
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

// TODO
static inline double uct(int_fast32_t idx);
static inline int_fast32_t select();
static inline void expand(int_fast32_t idx);
static inline void backtrack(int_fast32_t idx); 

//! main
int main(){
    initialize();
    while(true){
        get_input();
        utils::T0 = utils::get_time();

        make_move();
    }
}

static inline double uct(int_fast32_t idx){
    //! DO NOT INVOKE THIS FUNCTION ON A LEAF
    return (double) nodes[idx].wins / nodes[idx].vis + 1.41 * sqrt(log(nodes[nodes[idx].priev].vis) / nodes[idx].vis);
}

static inline void initialize(){
};

static inline void get_input(){
    // setup root
    nodes_size = 1;
    nodes[0].priev = 0;
    nodes[0].wins = nodes[0].vis = 0;
    nodes[0].bit = nodes[0].ultimate_row = nodes[0].main_bit = 0;
    nodes[0].player = foe;

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

    cin >> nodes[0].next.size;
    if(debug){
        cerr << nodes[0].next.size << endl;
    }

    for(uint_fast32_t i = 0; i < nodes[0].next.size; i++){
        cin >> row >> col;
        auto [bit, ultimate_row] = pos_of_coord[row][col];

        nodes[0].next.arr[i] = nodes_size;
        nodes[nodes_size].player = me;
        nodes[nodes_size].priev = 0;
        nodes[nodes_size].next.size = 0;
        nodes[nodes_size].wins = nodes[nodes_size].vis = 0;
        nodes[nodes_size].bit = bit;
        nodes[nodes_size].ultimate_row = ultimate_row;

        board::ultimate_board_me[ultimate_row] |= (1 << bit);
        nodes[nodes_size].main_bit = board::check_for_win(me, ultimate_row, bit / 9) ? 1 << (3 * ultimate_row + bit / 9) : 0;
        board::ultimate_board_me[ultimate_row] ^= (1 << bit);

        nodes_size++;

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

__attribute__((target("bmi2")))
static inline int_fast32_t simulate_till_end(int player, uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols){
    int ok_moves_cnt;
    array<uint_fast32_t, 3> ok_moves;
    array<uint_fast32_t, 3> moves;
    generate_all_moves(next_ultimate_rows, next_ultimate_cols, moves);

    while(moves[0] | moves[1] | moves[2]){
        // get ok moves
        ok_moves_cnt = 0;
        if(moves[0]) ok_moves[ok_moves_cnt++] = 0;
        if(moves[1]) ok_moves[ok_moves_cnt++] = 1;
        if(moves[2]) ok_moves[ok_moves_cnt++] = 2;

        // get random move
        const int_fast32_t board_idx = ok_moves[utils::random() % ok_moves_cnt];
        const int_fast32_t move = _pdep_u32(1u << (utils::random() % popcount(moves[board_idx])), moves[board_idx]);
        const int_fast32_t log2move = __builtin_ctz(move);
        

        // update board
        auto &ultimate_board = (player == me ? board::copy_ultimate_board_me[board_idx] : board::copy_ultimate_board_foe[board_idx]);
        auto &main_board = (player == me ? board::copy_main_board_me : board::copy_main_board_foe);

        ultimate_board |= (1 << log2move);
        if(board::check_for_copy_win(player, board_idx, log2move / 9)){
            main_board |= (1 << (board_idx * 3 + log2move / 9));
        }

        tie(next_ultimate_rows, next_ultimate_cols) = get_next_board_copy(log2move);
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

static inline int_fast32_t select(){ // TODO adjust oryginal board and main board going deeper
    int_fast32_t idx = 0;
    while(nodes[idx].next.size != 0){
        double best_uct = -1;
        int_fast32_t best_arg = 0;
        
        for(int_fast32_t i = 0; i < nodes[idx].next.size; i++){
            double UCT = uct(nodes[idx].next.arr[i]);
            if(UCT > best_uct){
                best_uct = UCT;
                best_arg = i;
            }
        }

        idx = nodes[idx].next.arr[best_arg];
    }

    return idx;
}

static inline void backtrack(int_fast32_t idx){ 

}

static void expand(int_fast32_t idx){
    array<uint_fast32_t, 3> moves;
    auto [next_ultimate_rows, next_ultimate_cols] = get_next_board(nodes[idx].bit);
    generate_all_moves(next_ultimate_rows, next_ultimate_cols, moves);

    int_fast32_t player = nodes[idx].player ^ 1;

    while(moves[0]){
        nodes[idx].next.arr[nodes[idx].next.size++] = nodes_size;

        int_fast32_t bit = __builtin_ctz(moves[0]);
        nodes[nodes_size].priev = idx;
        nodes[nodes_size].bit = bit;
        nodes[nodes_size].ultimate_row = 0; //!
        nodes[nodes_size].next.size = 0;
        nodes[nodes_size].player = player;

        if(player == me) board::ultimate_board_me[0] |= (1 << bit); //!
        else board::ultimate_board_foe[0] |= (1 << bit);  //!

        nodes[nodes_size].main_bit = board::check_for_win(player, 0, bit / 9) ? 1 << (bit / 9) : 0; //!
        if(nodes[nodes_size].main_bit and player == me) board::main_baord_me |= nodes[nodes_size].main_bit; 
        else if(nodes[nodes_size].main_bit and player == foe) board::main_baord_foe |= nodes[nodes_size].main_bit;

        tie(next_ultimate_rows, next_ultimate_cols) = get_next_board(bit);
        nodes[nodes_size].vis = no_sim;
        nodes[nodes_size].wins = flat_mc(player, next_ultimate_rows, next_ultimate_cols, no_sim);

        if(player == me){
            board::ultimate_board_me[0] ^= (1 << bit); //!
            if(nodes[nodes_size].main_bit) board::main_baord_me ^= nodes[nodes_size].main_bit; //!
        } else {
            board::ultimate_board_foe[0] ^= (1 << bit); //!
            if(nodes[nodes_size].main_bit) board::main_baord_foe ^= nodes[nodes_size].main_bit; //!
        }

        nodes[idx].wins += nodes[nodes_size].wins;
        nodes[idx].vis += nodes[nodes_size].vis;
        moves[0] ^= bit; //!
        nodes_size++;
    }

    while(moves[1]){
        nodes[idx].next.arr[nodes[idx].next.size++] = nodes_size;

        int_fast32_t bit = __builtin_ctz(moves[1]);
        nodes[nodes_size].priev = idx;
        nodes[nodes_size].bit = bit;
        nodes[nodes_size].ultimate_row = 1;
        nodes[nodes_size].next.size = 0;

        if(player == me) board::ultimate_board_me[1] |= (1 << bit);
        else board::ultimate_board_foe[1] |= (1 << bit);  //!

        nodes[nodes_size].main_bit = board::check_for_win(player, 1, bit / 9) ? 1 << (3 + bit / 9) : 0;
        if(nodes[nodes_size].main_bit and player == me) board::main_baord_me |= nodes[nodes_size].main_bit; 
        else if(nodes[nodes_size].main_bit and player == foe) board::main_baord_foe |= nodes[nodes_size].main_bit;

        tie(next_ultimate_rows, next_ultimate_cols) = get_next_board(bit);
        nodes[nodes_size].vis = no_sim;
        nodes[nodes_size].wins = flat_mc(player, next_ultimate_rows, next_ultimate_cols, no_sim);

        if(player == me){
            board::ultimate_board_me[1] ^= (1 << bit);
            if(nodes[nodes_size].main_bit) board::main_baord_me ^= nodes[nodes_size].main_bit; 
        } else {
            board::ultimate_board_foe[1] ^= (1 << bit); 
            if(nodes[nodes_size].main_bit) board::main_baord_foe ^= nodes[nodes_size].main_bit; 
        }

        nodes[idx].wins += nodes[nodes_size].wins;
        nodes[idx].vis += nodes[nodes_size].vis;
        moves[1] ^= bit;
        nodes_size++;
    }

    while(moves[2]){
        nodes[idx].next.arr[nodes[idx].next.size++] = nodes_size;

        int_fast32_t bit = __builtin_ctz(moves[2]);
        nodes[nodes_size].priev = idx;
        nodes[nodes_size].bit = bit;
        nodes[nodes_size].ultimate_row = 2;
        nodes[nodes_size].next.size = 0;

        if(player == me) board::ultimate_board_me[2] |= (1 << bit);
        else board::ultimate_board_foe[2] |= (1 << bit);  

        nodes[nodes_size].main_bit = board::check_for_win(player, 2, bit / 9) ? 1 << (6 + bit / 9) : 0;
        if(nodes[nodes_size].main_bit and player == me) board::main_baord_me |= nodes[nodes_size].main_bit; 
        else if(nodes[nodes_size].main_bit and player == foe) board::main_baord_foe |= nodes[nodes_size].main_bit;

        tie(next_ultimate_rows, next_ultimate_cols) = get_next_board(bit);
        nodes[nodes_size].vis = no_sim;
        nodes[nodes_size].wins = flat_mc(player, next_ultimate_rows, next_ultimate_cols, no_sim);

        if(player == me){
            board::ultimate_board_me[2] ^= (1 << bit);
            if(nodes[nodes_size].main_bit) board::main_baord_me ^= nodes[nodes_size].main_bit; 
        } else {
            board::ultimate_board_foe[2] ^= (1 << bit); 
            if(nodes[nodes_size].main_bit) board::main_baord_foe ^= nodes[nodes_size].main_bit; 
        }

        nodes[idx].wins += nodes[nodes_size].wins;
        nodes[idx].vis += nodes[nodes_size].vis;
        moves[2] ^= bit; 
        nodes_size++;
    }
}

static void make_move(){
    while(utils::elapsed_time() < 95'000){
        for(int i = 0; i < 50; i++){
            int_fast32_t selected = select();
            expand(selected);
            backtrack(selected);
        }  
    }

    // choose best move
    int best_score = -inf, arg_best_score = 0;
    for(uint_fast32_t i = 0; i < nodes[0].next.size; i++){
        if(nodes[nodes[0].next.arr[i]].wins > best_score){
            best_score = nodes[nodes[0].next.arr[i]].wins;
            arg_best_score = i;
        }
    }

    // make move
    board::ultimate_board_me[nodes[nodes[0].next.arr[arg_best_score]].ultimate_row] |= nodes[nodes[0].next.arr[arg_best_score]].bit;
    board::main_baord_me |= nodes[nodes[0].next.arr[arg_best_score]].main_bit;

    auto [row, col] = coord_of_pos[nodes[nodes[0].next.arr[arg_best_score]].ultimate_row][nodes[nodes[0].next.arr[arg_best_score]].bit];
    cout << row << ' ' << col << endl;
}