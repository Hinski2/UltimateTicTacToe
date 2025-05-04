#include <bits/stdc++.h>
#include <immintrin.h>
using namespace std;

/*
    game: ultimate tic tac toe
    algoright: mcts
    performacne: 278 gold league

*/

#pragma GCC optimize("Ofast,inline,tracer")
#pragma GCC optimize("unroll-loops,vpt,split-loops,unswitch-loops") 

//! consts
constexpr bool debug = false;
constexpr bool test = true;
constexpr bool turbo_debug = true;
constexpr int_fast32_t no_sim = 5;
constexpr int_fast32_t inf = 1e9 + 7;
constexpr int_fast32_t max_nodes = 100'000;

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
    uint32_t random(){
        static random_device rd;
        static mt19937 gen(rd());
        uniform_int_distribution<uint32_t> dist(0, 81);
        return dist(gen);

        // return (randomRaw() * uint64_t(82)) >> 32;
    }

    bool all_on_red(double prob){
        return (double) random() / UINT32_MAX < prob;
    }
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

//! node
struct Node{
    static int_fast32_t idx;
    static int_fast32_t free_idx;

    // node info
    int_fast32_t wins, vis;
    int_fast32_t player;
    int_fast32_t bit, ultimate_row;     // new bit on ultimate_board[ultimate_column]
    int_fast32_t main_bit;              // new bit on a main_baord

    // moving
    int_fast32_t priev;                 // prievious state
    int_fast32_t offset;                // where in nodes[] my children starts
    int_fast32_t size;                  // how many sons node has

} nodes[max_nodes];

int_fast32_t Node::idx = 0;
int_fast32_t Node::free_idx = 0;

//! board
struct Board{
    uint_fast32_t ultimate_board[3];
    uint_fast16_t main_board;

    uint_fast32_t copy_ultimate_board[3];
    uint_fast16_t copy_main_board;

    static constexpr bool winning_board[] = {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    static constexpr uint_fast32_t board_mask[] = {511, 261632, 133955584, 134217727}; // fi ultra_col, se ultra_col, th ultra_coll, all ultra_coll
    static constexpr uint_fast32_t not_board_mask[] = {134217216, 133956095, 262143};

    inline bool check_for_win(int_fast32_t ultimate_row, int_fast32_t ultimate_col){
        return winning_board[(ultimate_board[ultimate_row] & board_mask[ultimate_col]) >> (9 * ultimate_col)];
    }

    inline bool check_for_copy_win(int_fast32_t ultimate_row, int_fast32_t ultimate_col){
        return winning_board[(copy_ultimate_board[ultimate_row] & board_mask[ultimate_col]) >> (9 * ultimate_col)];
    }

} board[2];

//! global functions
// input
static inline void initialize();
static inline void get_input();
static inline void add_son(uint_fast32_t &moves, int_fast32_t ultimate_row, int_fast32_t player);

// moves utils
static inline void generate_all_moves(uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols, array<uint_fast32_t, 3> &moves);
static inline void generate_all_moves_copy(uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols, array<uint_fast32_t, 3> &moves);

// making moves
static inline void make_move();
static inline pair<uint_fast32_t, uint_fast32_t> get_next_board_copy(uint_fast32_t bit);
static inline pair<uint_fast32_t, uint_fast32_t> get_next_board(uint_fast32_t bit);

// bots
static inline int_fast32_t flat_mc(int player, uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols);
static inline int_fast32_t simulate_till_end(int player, uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols);

// debug
static void print_ultimate_board();
static void print_main_baord();

static inline double uct(int_fast32_t idx, double c);
static inline void select();
static inline pair<int_fast32_t, int_fast32_t> expand(); // [win, vis];
static inline void backtrack(int_fast32_t win, int_fast32_t vis); 


//! main
int main(){
    initialize();
    while(true){
        get_input();
        utils::T0 = utils::get_time();
        make_move();
    }
}

static inline double uct(int_fast32_t idx, double c){
    //! DO NOT INVOKE THIS FUNCTION ON A LEAF
    double wins = nodes[idx].player == me ? nodes[idx].wins : nodes[idx].vis - nodes[idx].wins;
    return (double) wins / nodes[idx].vis + c / sqrt(nodes[idx].vis);
}

static inline void initialize(){
};

static inline void get_input(){
    // setup root
    Node::idx = 0;
    Node::free_idx = 1;

    nodes[0].priev = 0;
    nodes[0].wins = nodes[0].vis = 0;
    nodes[0].bit = nodes[0].ultimate_row = nodes[0].main_bit = -1;
    nodes[0].player = foe;
    nodes[0].offset = 1; // first childre in 1

    int_fast32_t row, col;
    cin >> row >> col;
    if(debug){
        cerr << row << ' ' << col << endl;
    }

    if(row != -1){
        auto [bit, ultimate_row] = pos_of_coord[row][col];
        board[foe].ultimate_board[ultimate_row] |= (1 << bit);

        if(board[foe].check_for_win(ultimate_row, bit / 9)){
            board[foe].main_board |= (1 << (3 * ultimate_row + bit / 9));
        }
    }

    cin >> nodes[0].size;
    if(debug){
        cerr << nodes[0].size << endl;
    }

    for(uint_fast32_t i = 0; i < nodes[0].size; i++){
        cin >> row >> col;
        auto [bit, ultimate_row] = pos_of_coord[row][col];
        bit = 1 << bit;
        add_son(bit , ultimate_row, me); 
        if(debug){
            cerr << row << ' ' << col << endl;
        }
    }
}

static inline void generate_all_moves_copy(uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols, array<uint_fast32_t, 3> &moves){
    moves = {0, 0, 0};
    uint_fast32_t start = (next_ultimate_rows == all ? 0 : next_ultimate_rows), end = (next_ultimate_rows == all ? 2 : next_ultimate_rows);
    for(; start <= end; start++){
        moves[start] = ~(board[me].copy_ultimate_board[start] | board[foe].copy_ultimate_board[start]);

        if((board[me].copy_main_board | board[foe].copy_main_board) & (1 << (start * 3))) moves[start] &= Board::not_board_mask[0];
        if((board[me].copy_main_board | board[foe].copy_main_board) & (1 << (start * 3 + 1))) moves[start] &= Board::not_board_mask[1];
        if((board[me].copy_main_board | board[foe].copy_main_board) & (1 << (start * 3 + 2))) moves[start] &= Board::not_board_mask[2];

        moves[start] &= Board::board_mask[next_ultimate_cols];
    }
}

static inline void generate_all_moves(uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols, array<uint_fast32_t, 3> &moves){
    moves = {0, 0, 0};
    uint_fast32_t start = (next_ultimate_rows == all ? 0 : next_ultimate_rows), end = (next_ultimate_rows == all ? 2 : next_ultimate_rows);
    for(; start <= end; start++){
        moves[start] = ~(board[me].ultimate_board[start] | board[foe].ultimate_board[start]);

        if((board[me].main_board | board[foe].main_board) & (1 << (start * 3))) moves[start] &= Board::not_board_mask[0];
        if((board[me].main_board | board[foe].main_board) & (1 << (start * 3 + 1))) moves[start] &= Board::not_board_mask[1];
        if((board[me].main_board | board[foe].main_board) & (1 << (start * 3 + 2))) moves[start] &= Board::not_board_mask[2];

        moves[start] &= Board::board_mask[next_ultimate_cols];
    }
}

__attribute__((target("bmi2")))
static inline int_fast32_t simulate_till_end(int player, uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols){
    int ok_moves_cnt;
    array<uint_fast32_t, 3> ok_moves;
    array<uint_fast32_t, 3> moves;
    generate_all_moves_copy(next_ultimate_rows, next_ultimate_cols, moves);

    while(moves[0] | moves[1] | moves[2]){
        // get ok moves
        ok_moves_cnt = 0;
        if(moves[0]) ok_moves[ok_moves_cnt++] = 0;
        if(moves[1]) ok_moves[ok_moves_cnt++] = 1;
        if(moves[2]) ok_moves[ok_moves_cnt++] = 2;

        // get random move
        const int_fast32_t board_idx = ok_moves[utils::random() % ok_moves_cnt];
        const int_fast32_t move = _pdep_u32(1u << (utils::random() % __builtin_popcountll(moves[board_idx])), moves[board_idx]);
        const int_fast32_t log2move = __builtin_ctz(move);
        
        board[player].copy_ultimate_board[board_idx] |= (1 << log2move);
        if(board[player].check_for_copy_win(board_idx, log2move / 9)){
            board[player].copy_main_board |= (1 << (board_idx * 3 + log2move / 9));
        }

        tie(next_ultimate_rows, next_ultimate_cols) = get_next_board_copy(log2move);
        generate_all_moves_copy(next_ultimate_rows, next_ultimate_cols, moves);
        player ^= 1;
    }

    if(Board::winning_board[board[me].copy_main_board]) return 1;
    if(Board::winning_board[board[foe].copy_main_board]) return -1;
    return __builtin_popcountll(board[me].copy_main_board) > __builtin_popcountll(board[foe].copy_main_board);
}

static int_fast32_t flat_mc(int player, uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols){
    int_fast32_t wins = 0;
    
    for(uint_fast32_t i = 0; i < no_sim; i++){
        // copy
        for(uint_fast32_t j = 0; j < 3; j++){
            board[foe].copy_ultimate_board[j] = board[foe].ultimate_board[j];
            board[me].copy_ultimate_board[j] = board[me].ultimate_board[j];
        }

        board[me].copy_main_board = board[me].main_board;
        board[foe].copy_main_board = board[foe].main_board;

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
    if(board[foe].main_board & (1 << (next_ultiamte_row * 3 + next_ultimate_col)) or board[me].main_board & (1 << (next_ultiamte_row * 3 + next_ultimate_col)))
        return {all, all};

    // if there is still palce
    if((~(board[foe].ultimate_board[next_ultiamte_row] | board[me].ultimate_board[next_ultiamte_row])) & Board::board_mask[next_ultimate_col]) 
        return {next_ultiamte_row, next_ultimate_col};
    
    return {all, all};
}

static inline pair<uint_fast32_t, uint_fast32_t> get_next_board_copy(uint_fast32_t bit){
    uint_fast32_t next_ultiamte_row = (bit % 9) / 3;
    uint_fast32_t next_ultimate_col = bit % 3;

    // if subgame is finished
    if(board[foe].copy_main_board & (1 << (next_ultiamte_row * 3 + next_ultimate_col)) or board[me].copy_main_board & (1 << (next_ultiamte_row * 3 + next_ultimate_col)))
        return {all, all};

    // if there is still palce
    if((~(board[foe].copy_ultimate_board[next_ultiamte_row] | board[me].copy_ultimate_board[next_ultiamte_row])) & Board::board_mask[next_ultimate_col]) 
        return {next_ultiamte_row, next_ultimate_col};
    
    return {all, all};
}

static inline void select(){
    while(nodes[Node::idx].size != 0){
        double best_uct = -inf;
        int_fast32_t best_arg = 0;

        const double c = 1 * sqrt(log(nodes[Node::idx].vis));
        for(int_fast32_t i = nodes[Node::idx].offset; i < nodes[Node::idx].offset + nodes[Node::idx].size; i++){
            double UCT = uct(i, c);

            if(UCT > best_uct){
                best_uct = UCT;
                best_arg = i;
            }
        }

        Node::idx = best_arg;
        if(nodes[Node::idx].main_bit != -1) board[nodes[best_arg].player].main_board |= (1 << nodes[Node::idx].main_bit);
        board[nodes[best_arg].player].ultimate_board[nodes[Node::idx].ultimate_row] |= (1 << nodes[Node::idx].bit);
    }
}

static inline void backtrack(int_fast32_t win, int_fast32_t vis){ 
    while(Node::idx){
        auto &v = nodes[Node::idx];

        nodes[v.priev].wins += win;
        nodes[v.priev].vis += vis;

        board[v.player].ultimate_board[v.ultimate_row] ^= (1 << v.bit);
        if(v.main_bit != -1) board[v.player].main_board ^= (1 << v.main_bit); 

        Node::idx = v.priev;
    }
}

static inline void add_son(uint_fast32_t &moves, int_fast32_t ultimate_row, int_fast32_t player){
    int_fast32_t bit = __builtin_ctz(moves);
    nodes[Node::free_idx].priev = Node::idx;
    nodes[Node::free_idx].bit = bit;
    nodes[Node::free_idx].ultimate_row = ultimate_row;
    nodes[Node::free_idx].size = 0;
    nodes[Node::free_idx].player = player;
    nodes[Node::free_idx].wins = nodes[Node::free_idx].vis = 0;

    board[player].ultimate_board[ultimate_row] |= (1 << bit);

    nodes[Node::free_idx].main_bit = board[player].check_for_win(ultimate_row, bit / 9) ? (3 * ultimate_row) + bit / 9 : -1;
    if(nodes[Node::free_idx].main_bit != -1) board[player].main_board |= (1 << nodes[Node::free_idx].main_bit);

    auto [next_ultimate_rows, next_ultimate_cols] = get_next_board(bit);
    nodes[Node::free_idx].vis = no_sim;
    nodes[Node::free_idx].wins = flat_mc(player^1, next_ultimate_rows, next_ultimate_cols);


    board[player].ultimate_board[ultimate_row] ^= (1 << bit);
    if(nodes[Node::free_idx].main_bit != -1) board[player].main_board ^= (1 << nodes[Node::free_idx].main_bit);

    nodes[Node::idx].wins += nodes[Node::free_idx].wins;
    nodes[Node::idx].vis += nodes[Node::free_idx].vis;

    moves ^= (1 << bit); 
    Node::free_idx++; 
}

static inline pair<int_fast32_t, int_fast32_t> expand(){
    int_fast32_t prev_w = nodes[Node::idx].wins;
    int_fast32_t prev_v = nodes[Node::idx].vis;

    array<uint_fast32_t, 3> moves;
    auto [next_ultimate_rows, next_ultimate_cols] = get_next_board(nodes[Node::idx].bit);
    generate_all_moves(next_ultimate_rows, next_ultimate_cols, moves);

    int_fast32_t player = nodes[Node::idx].player ^ 1;
    nodes[Node::idx].offset = Node::free_idx;
    nodes[Node::idx].size = __builtin_popcountll(moves[0]) + __builtin_popcountll(moves[1]) + __builtin_popcountll(moves[2]);

    while(moves[0]){
        add_son(moves[0], 0, player);
    }

    while(moves[1]){
        add_son(moves[1], 1, player);
    }

    while(moves[2]){
        add_son(moves[2], 2, player);
    }

    if(nodes[Node::idx].size == 0){
        if(Board::winning_board[board[me].main_board]) nodes[Node::idx].wins += no_sim;
        else if(Board::winning_board[board[foe].main_board]) nodes[Node::idx].wins -= no_sim;
        else nodes[Node::idx].wins += no_sim * (__builtin_popcountll(board[me].main_board) > __builtin_popcountll(board[foe].main_board) ? 1 : -1);

        nodes[Node::idx].vis += no_sim;
    }

    return {nodes[Node::idx].wins - prev_w, nodes[Node::idx].vis - prev_v};
}

static void make_move(){ 
    while(utils::elapsed_time() < 95'000){ 
        for(int i = 0; i < 20; i++){
            select();
            auto [win, vis] = expand();
            backtrack(win, vis);
        }  
    }

    // for(int ddddddd = 0; ddddddd < 100; ddddddd++){
    //     for(int i = 0; i < 81; i++){
    //         select();
    //         auto [win, vis] = expand();
    //         backtrack(win, vis);
    //     }  
    // }

    // choose best move
    int arg_best_score = 0;
    double best_score = -inf;
    for(uint_fast32_t i = nodes[Node::idx].offset; i < nodes[Node::idx].offset + nodes[Node::idx].size; i++){
        double score = (double) nodes[i].wins / nodes[i].vis;
        if(score > best_score){
            best_score = score;
            arg_best_score = i;
        }
    }

    // make move
    board[me].ultimate_board[nodes[arg_best_score].ultimate_row] |= (1 << nodes[arg_best_score].bit);
    if(nodes[arg_best_score].main_bit != -1) board[me].main_board |= (1 << nodes[arg_best_score].main_bit);

    auto [row, col] = coord_of_pos[nodes[arg_best_score].ultimate_row][nodes[arg_best_score].bit];
    cout << row << ' ' << col << endl;

    if(turbo_debug){
        print_ultimate_board();
        print_main_baord();
    }
}

static void print_ultimate_board(){
    int matrix_ultimate_board[9][9];
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            auto [bit, ultimate_row] = pos_of_coord[i][j];
            if(board[me].ultimate_board[ultimate_row] & (1 << bit) and board[foe].ultimate_board[ultimate_row] & (1 << bit)){
                cout << "PANIC4" << endl;
                exit(1);
            } else if(board[me].ultimate_board[ultimate_row] & (1 << bit)){
                matrix_ultimate_board[i][j] = me;
            } else if(board[foe].ultimate_board[ultimate_row] & (1 << bit)){
                matrix_ultimate_board[i][j] = foe;
            } else {
                matrix_ultimate_board[i][j] = 2;
            }
        }
    }

    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            cout << matrix_ultimate_board[i][j] << ' ';
        }
        cout << endl;
    }
}

static void print_main_baord(){
    int matrix_main_board[3][3];
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            auto [bit, ultimate_row] = pos_of_coord[i][j];
            if(board[me].main_board & (1 << bit) and board[foe].main_board & 1 << bit){
                cout << "PANIC5" << endl;
                exit(1);
            } else if(board[me].main_board & (1 << bit)){
                matrix_main_board[i][j] = me;
            } else if(board[foe].main_board & (1 << bit)){
                matrix_main_board[i][j] = foe;
            } else {
                matrix_main_board[i][j] = 2;
            }
        }
    }

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            cout << matrix_main_board[i][j] << ' ';
        }
        cout << endl;
    }
}