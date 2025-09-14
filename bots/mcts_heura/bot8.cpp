#include <bits/stdc++.h>
#include <immintrin.h>
using namespace std;

/*
    game: ultimate tic tac toe
    algoright: mcts
    performacne: 267

    data:
        [10940, 1120, 1100, 1160, 1320, 1440, 1480, 1080, 1100, 1260, 1200, 1300, 1480, 1720, 2000, 2580, 3680, 5160, 5640, 23020, 50400]
        [12740, 1320, 1380, 1480, 1560, 1680, 1800, 1960, 1980, 2100, 1680, 1840, 1620, 1760, 1840, 2340, 3720, 5340]
        [10880, 1120, 1200, 1240, 1360, 1440, 1540, 1640, 1740, 1800, 1940, 2080, 2140, 1940, 1940, 1920, 2080, 2160, 2580, 2480, 3300, 6340, 9080]
        [10860, 1100, 1180, 1260, 1320, 1360, 1420, 1400, 1260, 1480, 1640, 1700, 1780, 1980, 1940, 1620, 1820, 2020, 2640, 6940, 13340, 54080, 92020, 178320]
        [10260, 1160, 1000, 1260, 1420, 1220, 1240, 1180, 1340, 1280, 1380, 1460, 1260, 1380]
        [9840, 1060, 980, 1200, 1260, 1340, 1400, 1520, 1780, 1840, 1920, 1760, 2160, 1860, 1560, 1780, 1920, 2400, 3420, 20460, 39540]
        [11640, 1220, 1240, 1300, 1380, 1420, 1600, 1580, 1400, 1500, 1660, 1400, 1360, 1560, 1700, 1900]
        [10740, 1080, 1140, 1240, 1280, 900, 960, 1040, 1160, 1260, 1240, 1180, 1300, 1240, 1440, 1580, 3500, 2440]
        [10900, 1120, 1180, 1220, 1260, 1340, 1440, 1500, 1600, 1700, 1720, 1860, 1360, 1400, 1560, 1580, 1660, 1860, 2280, 2620, 2880, 3520, 3960]
        [11040, 1140, 1160, 1240, 1320, 1440, 1500, 1440, 1580, 1700, 1740, 1960, 1640, 1360, 1600, 1720, 1940, 2060, 2620, 3920, 9520, 16180, 65560, 110800]
    avg + pred: 
        [10984, 1144, 1156, 1260, 1348, 1398, 1462, 1484, 1404, 1532, 1652, 1724, 1684, 1684, 1744, 1752, 2031, 2502, 2878, 6820, 12990, 21280, 42280, 64660, 100000, 100000,
        100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000 100000, 100000, 100000, 100000, 100000, 100000]

*/

// #pragma GCC optimize("Ofast,inline,tracer")
// #pragma GCC optimize("unroll-loops,vpt,split-loops,unswitch-loops")
// #pragma GCC optimize("Ofast,inline,unroll-loops")
// #pragma GCC target("aes,abm,align-stringops,avx,avx2,bmi,bmi2,crc32,cx16,f16c,fma,fsgsbase,fxsr,hle,ieee-fp,lzcnt,mmx,movbe,mwait,pclmul,popcnt,rdrnd,sahf,sse,sse2,sse3,sse4,sse4.1,sse4.2,ssse3,xsave,xsaveopt") 

//! consts
constexpr bool debug = false;
constexpr bool turbo_debug = false;
constexpr bool test = true;
constexpr bool test_localy = true;
constexpr int_fast32_t no_sim = 1;
constexpr int_fast32_t inf = 1e9 + 7;
constexpr int_fast32_t max_nodes = 3'000'000;
int_fast32_t m_time = 990'000;

int_fast32_t itr_cnt = 0;
constexpr int_fast32_t no_itr[] = {10984, 1144, 1156, 1260, 1348, 1398, 1462, 1484, 1404, 1532, 1652, 1724, 1684, 1684, 1744, 1752, 2031, 2502, 2878, 6820, 12990, 21280, 42280, 64660, 100000, 100000,
                                    100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000}; 

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

namespace Moves {
    uint_fast32_t fi, se, th;
};

//! utils
namespace utils{
 //! utils section
    //* time utils section.vis += no_sim;
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
    uint32_t random_with_boud(const uint bound){
        return (randomRaw() * uint64_t(bound)) >> 32;
    }
    uint32_t random(){
        return (randomRaw() * uint64_t(82)) >> 32;
    }

    bool all_on_red(double prob){
        return (double) random() / UINT32_MAX < prob;
    }

    double inverse_sqrt(uint_fast32_t x) {
        double y = static_cast<double>(x);
        uint64_t bits;
        memcpy(&bits, &y, sizeof(bits));
        bits = 0x5fe6ec85e7de30daULL - (bits >> 1);
        memcpy(&y, &bits, sizeof(y));
        y = y * (1.5 - 0.5 * static_cast<double>(x) * y * y);
        return y;
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
    static int_fast32_t root;

    // node info
    int_fast32_t wins;
    uint_fast32_t vis;
    int_fast32_t player;
    int_fast32_t bit, ultimate_row;     // new bit on ultimate_board[ultimate_column]
    int_fast32_t main_bit;              // new bit on a main_baord

    // moving
    int_fast32_t priev;                 // prievious state
    int_fast32_t offset;                // where in nodes[] my children starts
    int_fast32_t size;                  // how many sons node has

} nodes[max_nodes];

int_fast32_t Node::idx = 0;
int_fast32_t Node::root = 0;
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
static inline void get_first_input();

// moves utils
static inline void generate_all_moves(const uint_fast32_t next_ultimate_rows, const uint_fast32_t next_ultimate_cols);
static inline void generate_all_moves_copy(const uint_fast32_t next_ultimate_rows, const uint_fast32_t next_ultimate_cols);

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
int main(int argc, char* argv[]){
    if(test_localy) {
        utils::seed = argc < 2 ? 42 : stoi(argv[1]);
    }

    initialize();
    m_time = 95'000;

    while(true){
        get_input();
        utils::T0 = utils::get_time();
        make_move();
        
        // if(test) cerr << Node::free_idx << endl;
    }
}

template<int_fast32_t PLAYER, int_fast32_t ULTIMATE_ROW>
static inline void add_son_impl(uint_fast32_t &moves){
    int_fast32_t bit = __builtin_ctz(moves);
    nodes[Node::free_idx].priev = Node::idx;
    nodes[Node::free_idx].bit = bit;
    nodes[Node::free_idx].ultimate_row = ULTIMATE_ROW;
    nodes[Node::free_idx].size = 0;
    nodes[Node::free_idx].player = PLAYER;
    nodes[Node::free_idx].wins = nodes[Node::free_idx].vis = 0;

    board[PLAYER].ultimate_board[ULTIMATE_ROW] |= (1 << bit);

    nodes[Node::free_idx].main_bit = board[PLAYER].check_for_win(ULTIMATE_ROW, bit / 9) ? (3 * ULTIMATE_ROW) + bit / 9 : -1;
    if(nodes[Node::free_idx].main_bit != -1) board[PLAYER].main_board |= (1 << nodes[Node::free_idx].main_bit);

    auto [next_ultimate_rows, next_ultimate_cols] = get_next_board(bit);
    nodes[Node::free_idx].vis = no_sim;
    nodes[Node::free_idx].wins = flat_mc(PLAYER^1, next_ultimate_rows, next_ultimate_cols);

    board[PLAYER].ultimate_board[ULTIMATE_ROW] ^= (1 << bit);
    if(nodes[Node::free_idx].main_bit != -1) board[PLAYER].main_board ^= (1 << nodes[Node::free_idx].main_bit);

    nodes[Node::idx].wins += nodes[Node::free_idx].wins;
    nodes[Node::idx].vis += nodes[Node::free_idx].vis;

    moves ^= (1 << bit); 
    Node::free_idx++; 
    if(test and Node::free_idx + 10'000 > max_nodes){
        cout << "PANIC7" << endl;
    }
}

static inline void add_son(uint_fast32_t& moves, int_fast32_t ultimate_row, int_fast32_t player) {
    const int idx = player * 3 + ultimate_row;
    if(debug and (idx < 0 or idx > 5)) {
        cout << "invalid idx in add_son" << endl;
    }

    switch(idx) {
        case 0: return add_son_impl<0, 0>(moves);
        case 1: return add_son_impl<0, 1>(moves);
        case 2: return add_son_impl<0, 2>(moves);
        case 3: return add_son_impl<1, 0>(moves);
        case 4: return add_son_impl<1, 1>(moves);
        case 5: return add_son_impl<1, 2>(moves);
    }
}

static inline double uct(int_fast32_t idx, double c){
    double wins = nodes[idx].player == me ? nodes[idx].wins : nodes[idx].vis - nodes[idx].wins;
    return (double) wins / nodes[idx].vis + c * utils::inverse_sqrt(nodes[idx].vis);
}

static inline void initialize(){
    Node::root = 0;
    Node::idx = 0;
    Node::free_idx = 1;

    nodes[Node::root].priev = 0;
    nodes[Node::root].wins = nodes[Node::root].vis = 0;
    nodes[Node::root].bit = nodes[Node::root].ultimate_row = nodes[Node::root].main_bit = -1;
    nodes[Node::root].player = foe;
    nodes[Node::root].offset = 1; // first childre in 1

    get_first_input();
    utils::T0 = utils::get_time();
    make_move();
};

static inline void move_root(uint_fast32_t bit, uint_fast32_t ultimate_row){
    for(int i = nodes[Node::root].offset; i < nodes[Node::root].offset + nodes[Node::root].size; i++){
        if(nodes[i].ultimate_row == ultimate_row and nodes[i].bit == bit){
            board[foe].ultimate_board[ultimate_row] |= (1 << bit);
            if(nodes[i].main_bit != -1) board[foe].main_board |= (1 << nodes[i].main_bit);
            Node::root = i;
            Node::idx = i;
            return;
        }
    }

    if(test) cout << "PANIC1" << endl;
}

void debug_check_if_contains_son(uint_fast32_t bit, uint_fast32_t ultimate_row){
    for(int i = nodes[Node::root].offset; i < nodes[Node::root].offset + nodes[Node::root].size; i++){
        if(nodes[i].ultimate_row == ultimate_row and nodes[i].bit == bit){
            return;
        }
    }

    cout << "PANIC3" << endl;
}

static inline void get_input(){
    int_fast32_t row, col;
    cin >> row >> col;
    if(debug){
        cerr << row << ' ' << col << endl;
    }

    if(row != -1){
        auto [bit, ultimate_row] = pos_of_coord[row][col];
        move_root(bit, ultimate_row);
    }

    int_fast32_t s;
    cin >> s;
    if(debug){
        cerr << s << endl;
    }


    bool add_sons = nodes[Node::root].size == 0;
    if(add_sons){
        nodes[Node::root].size = s;
        nodes[Node::root].offset = Node::free_idx;
    }

    for(uint_fast32_t i = 0; i < s; i++){
        cin >> row >> col;
        if(debug){
            cerr << row << ' ' << col << endl;
        }
        if(add_sons){
            auto [bit, ultiamte_row] = pos_of_coord[row][col];
            bit = 1 << bit;
            add_son(bit, ultiamte_row, me);
        }

        if(test){
            auto [bit, ultimate_row] = pos_of_coord[row][col];
            debug_check_if_contains_son(bit, ultimate_row);
        }
    }

    if(test and nodes[Node::root].size != s){
        cout << "PANIC2 " << s << ' ' << nodes[Node::root].size << endl;
    }
}

static inline void get_first_input(){
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

    cin >> nodes[Node::root].size;
    if(debug){
        cerr << nodes[Node::root].size << endl;
    }

    for(uint_fast32_t i = 0; i < nodes[0].size; i++){
        cin >> row >> col;
        auto [bit, ultimate_row] = pos_of_coord[row][col];
        bit = 1 << bit;
        add_son(bit, ultimate_row, me); 
        if(debug){
            cerr << row << ' ' << col << endl;
        }
    }
}



static inline void generate_all_moves_copy(const uint_fast32_t next_ultimate_rows, const uint_fast32_t next_ultimate_cols){
    Moves::fi = Moves::se = Moves::th = 0;

    // fi
    if(next_ultimate_rows == all or next_ultimate_rows == fi) {
        Moves::fi = ~(board[me].copy_ultimate_board[fi] | board[foe].copy_ultimate_board[fi]);

        if((board[me].copy_main_board | board[foe].copy_main_board) & (1 << (fi * 3))) Moves::fi &= Board::not_board_mask[0];
        if((board[me].copy_main_board | board[foe].copy_main_board) & (1 << (fi * 3 + 1))) Moves::fi &= Board::not_board_mask[1];
        if((board[me].copy_main_board | board[foe].copy_main_board) & (1 << (fi * 3 + 2))) Moves::fi &= Board::not_board_mask[2];

        Moves::fi &= Board::board_mask[next_ultimate_cols];
    }

    // se 
    if(next_ultimate_rows == all or next_ultimate_rows == se) {
        Moves::se = ~(board[me].copy_ultimate_board[se] | board[foe].copy_ultimate_board[se]);

        if((board[me].copy_main_board | board[foe].copy_main_board) & (1 << (se * 3))) Moves::se &= Board::not_board_mask[0];
        if((board[me].copy_main_board | board[foe].copy_main_board) & (1 << (se * 3 + 1))) Moves::se &= Board::not_board_mask[1];
        if((board[me].copy_main_board | board[foe].copy_main_board) & (1 << (se * 3 + 2))) Moves::se &= Board::not_board_mask[2];

        Moves::se &= Board::board_mask[next_ultimate_cols];
    }

    // th
    if(next_ultimate_rows == all or next_ultimate_rows == th) {
        Moves::th = ~(board[me].copy_ultimate_board[th] | board[foe].copy_ultimate_board[th]);

        if((board[me].copy_main_board | board[foe].copy_main_board) & (1 << (th * 3))) Moves::th &= Board::not_board_mask[0];
        if((board[me].copy_main_board | board[foe].copy_main_board) & (1 << (th * 3 + 1))) Moves::th &= Board::not_board_mask[1];
        if((board[me].copy_main_board | board[foe].copy_main_board) & (1 << (th * 3 + 2))) Moves::th &= Board::not_board_mask[2];

        Moves::th &= Board::board_mask[next_ultimate_cols];
    }
}
static inline void generate_all_moves(const uint_fast32_t next_ultimate_rows, const uint_fast32_t next_ultimate_cols){
    Moves::fi = Moves::se = Moves::th = 0;

    // fi
    if(next_ultimate_rows == all or next_ultimate_rows == fi) {
        Moves::fi = ~(board[me].ultimate_board[fi] | board[foe].ultimate_board[fi]);

        if((board[me].main_board | board[foe].main_board) & (1 << (fi * 3))) Moves::fi &= Board::not_board_mask[0];
        if((board[me].main_board | board[foe].main_board) & (1 << (fi * 3 + 1))) Moves::fi &= Board::not_board_mask[1];
        if((board[me].main_board | board[foe].main_board) & (1 << (fi * 3 + 2))) Moves::fi &= Board::not_board_mask[2];

        Moves::fi &= Board::board_mask[next_ultimate_cols];
    }

    // se 
    if(next_ultimate_rows == all or next_ultimate_rows == se) {
        Moves::se = ~(board[me].ultimate_board[se] | board[foe].ultimate_board[se]);

        if((board[me].main_board | board[foe].main_board) & (1 << (se * 3))) Moves::se &= Board::not_board_mask[0];
        if((board[me].main_board | board[foe].main_board) & (1 << (se * 3 + 1))) Moves::se &= Board::not_board_mask[1];
        if((board[me].main_board | board[foe].main_board) & (1 << (se * 3 + 2))) Moves::se &= Board::not_board_mask[2];

        Moves::se &= Board::board_mask[next_ultimate_cols];
    }

    // th
    if(next_ultimate_rows == all or next_ultimate_rows == th) {
        Moves::th = ~(board[me].ultimate_board[th] | board[foe].ultimate_board[th]);

        if((board[me].main_board | board[foe].main_board) & (1 << (th * 3))) Moves::th &= Board::not_board_mask[0];
        if((board[me].main_board | board[foe].main_board) & (1 << (th * 3 + 1))) Moves::th &= Board::not_board_mask[1];
        if((board[me].main_board | board[foe].main_board) & (1 << (th * 3 + 2))) Moves::th &= Board::not_board_mask[2];

        Moves::th &= Board::board_mask[next_ultimate_cols];
    }
}

static inline uint32_t pick_random_bit(uint32_t mask, int cnt) {
    int r = utils::random_with_boud(cnt);
    while(r--) mask &= mask - 1;
    return mask & -mask;
}

static inline pair<uint_fast32_t, uint_fast32_t> pick_random_move() {
    uint_fast32_t cnt = (Moves::fi != 0) + (Moves::se != 0) + (Moves::th != 0);
    uint_fast32_t k = utils::random_with_boud(cnt);

    if(Moves::fi) {
        if(!k) return {fi, Moves::fi};
        k--;
    } 
    if(Moves::se) {
        if(!k) return {se, Moves::se};
    }

    return {th, Moves::th};
}

static inline int_fast32_t simulate_till_end(int player, uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols){
    generate_all_moves_copy(next_ultimate_rows, next_ultimate_cols);

    while(Moves::fi | Moves::se | Moves::th){

        // get random move
        auto [board_idx, chosed_move] = pick_random_move();
        const int_fast32_t move = pick_random_bit(chosed_move, __builtin_popcountll(chosed_move));
        const int_fast32_t log2move = __builtin_ctz(move);
        
        board[player].copy_ultimate_board[board_idx] |= (1 << log2move);
        if(board[player].check_for_copy_win(board_idx, log2move / 9)){
            board[player].copy_main_board |= (1 << (board_idx * 3 + log2move / 9));
        }

        tie(next_ultimate_rows, next_ultimate_cols) = get_next_board_copy(log2move);
        generate_all_moves_copy(next_ultimate_rows, next_ultimate_cols);
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
        memcpy(board[foe].copy_ultimate_board, board[foe].ultimate_board, 3 * sizeof(board[0].ultimate_board[0]));
        memcpy(board[me].copy_ultimate_board, board[me].ultimate_board, 3 * sizeof(board[0].ultimate_board[0]));
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

        const double c = 0.75 * sqrt(log(nodes[Node::idx].vis));
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
    while(Node::idx != Node::root){
        auto &v = nodes[Node::idx];

        nodes[v.priev].wins += win;
        nodes[v.priev].vis += vis;

        board[v.player].ultimate_board[v.ultimate_row] ^= (1 << v.bit);
        if(v.main_bit != -1) board[v.player].main_board ^= (1 << v.main_bit); 

        Node::idx = v.priev;
    }
}



static inline pair<int_fast32_t, int_fast32_t> expand(){
    int_fast32_t prev_w = nodes[Node::idx].wins;
    int_fast32_t prev_v = nodes[Node::idx].vis;

    auto [next_ultimate_rows, next_ultimate_cols] = get_next_board(nodes[Node::idx].bit);
    generate_all_moves(next_ultimate_rows, next_ultimate_cols);
    uint_fast32_t moves_fi = Moves::fi, moves_se = Moves::se, moves_th = Moves::th;

    int_fast32_t player = nodes[Node::idx].player ^ 1;
    nodes[Node::idx].offset = Node::free_idx;
    nodes[Node::idx].size = __builtin_popcountll(moves_fi) + __builtin_popcountll(moves_se) + __builtin_popcountll(moves_th);

    while(moves_fi){
        add_son(moves_fi, 0, player);
    }

    while(moves_se){
        add_son(moves_se, 1, player);
    }

    while(moves_th){
        add_son(moves_th, 2, player);
    }

    if(nodes[Node::idx].size == 0){
        if(Board::winning_board[board[me].main_board]) nodes[Node::idx].wins += no_sim;
        else if(Board::winning_board[board[foe].main_board]) nodes[Node::idx].wins -= no_sim;
        else nodes[Node::idx].wins += __builtin_popcountll(board[me].main_board) > __builtin_popcountll(board[foe].main_board) ?  no_sim : -no_sim;

        nodes[Node::idx].vis += no_sim;
    }

    return {nodes[Node::idx].wins - prev_w, nodes[Node::idx].vis - prev_v};
}

static void make_move(){ 
    if(!test_localy) {
        while(utils::elapsed_time() < m_time) { 
            for(int i = 0; i < 50; i++){
                select();
                auto [win, vis] = expand();
                backtrack(win, vis);
            }  
        }
    } else {
        for(int i = 0; i < no_itr[itr_cnt]; i++) {
            select();
            auto [win, vis] = expand();
            backtrack(win, vis);
        }
        itr_cnt++;
    }

    // choose best move
    int arg_best_score = 0, best_score = -inf;
    for(uint_fast32_t i = nodes[Node::idx].offset; i < nodes[Node::idx].offset + nodes[Node::idx].size; i++){
        double score = nodes[i].wins;
        if(score > best_score){
            best_score = score;
            arg_best_score = i;
        }
    }

    // make move
    if(test and nodes[Node::idx].player != foe){
        cout << "PANIC6" << endl;
    }

    board[me].ultimate_board[nodes[arg_best_score].ultimate_row] |= (1 << nodes[arg_best_score].bit);
    if(nodes[arg_best_score].main_bit != -1) board[me].main_board |= (1 << nodes[arg_best_score].main_bit);

    Node::idx = arg_best_score;
    Node::root = arg_best_score;

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