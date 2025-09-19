#include <bits/stdc++.h>
#include <immintrin.h>
using namespace std;

/*
    game: ultimate tic tac toe
    algoright: mcts
    performacne: 241

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
#pragma GCC optimize("Ofast,inline,tracer")
#pragma GCC optimize("unroll-loops,vpt,split-loops,unswitch-loops")
#pragma GCC optimize("Ofast,inline,unroll-loops")
#pragma GCC target("aes,abm,align-stringops,avx,avx2,bmi,bmi2,crc32,cx16,f16c,fma,fsgsbase,fxsr,hle,ieee-fp,lzcnt,mmx,movbe,mwait,pclmul,popcnt,rdrnd,sahf,sse,sse2,sse3,sse4,sse4.1,sse4.2,ssse3,xsave,xsaveopt") 

typedef int32_t i32;
typedef int64_t i64;
typedef uint32_t u32;
typedef uint64_t u64;

//! consts
constexpr bool debug = false;
constexpr bool turbo_debug = false;
constexpr bool test = true;
constexpr bool test_localy = true;
constexpr i32 no_sim = 1;
constexpr i32 inf = 1e9 + 7;
constexpr i32 max_nodes = 3'000'000;
i32 m_time = 990'000;

i32 itr_cnt = 0;
constexpr i32 no_itr[] = {10984, 1144, 1156, 1260, 1348, 1398, 1462, 1484, 1404, 1532, 1652, 1724, 1684, 1684, 1744, 1752, 2031, 2502, 2878, 6820, 12990, 21280, 42280, 64660, 100000, 100000,
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
    u32 fi, se, th;
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

    double inverse_sqrt(u32 x) {
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
    u32 bit;
    u32 ultra_row;
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
namespace Node{
    static u32 idx = 0;
    static u32 free_idx = 0;
    static u32 root = 0;

    /*
    int64_t ultimate_row_size_main_bit_bit_win
        [0, 1]      2 bit   uint     ultimate_row: {fi, se, th, all}
        [2, 8]      7 bit   uint    size: max son no is 81
        [9, 17]     9 bit   uint    main_bit: mask with one or all on bits
        [18, 44]    27 bit  uint    bit: mask with one set bit
        [45, 63]    19 bit  int     win: 524'288 is enough
    */

    static u64 ultimateRow_size_mainBit_bit_win[max_nodes];
    constexpr u64 MASK_ULTIMATE_ROW =   ((1ULL << 2) - 1ULL) << 0;
    constexpr u64 MASK_SIZE =           ((1ULL << 7) - 1ULL) << 2;
    constexpr u64 MASK_MAIN_BIT =       ((1ULL << 9) - 1ULL) << 9;
    constexpr u64 MASK_BIT =            ((1ULL << 27) - 1ULL) << 18;
    constexpr u64 MASK_WIN =            ((1ULL << 19) - 1ULL) << 45;

    /*
    int64_t player_vis_priev_offset
        [0, 0]      1 bit   uint player: {me, foe}
        [1, 19]     19 bit  uint vis: 524'288 is enough
        [20, 41]    22 bit  uint priev: 4'194'304 is enough
        [42, 63]    22 bit  uint offset: 4'194'304 is enogh
    */

    static u64 player_vis_priev_offset[max_nodes];
    constexpr u64 MASK_PLAYER =     ((1ULL << 1) - 1ULL) << 0;
    constexpr u64 MASK_VIS =        ((1ULL << 19) - 1ULL) << 1;
    constexpr u64 MASK_PRIEV =      ((1ULL << 22) - 1ULL) << 20; 
    constexpr u64 MASK_OFFSET =     ((1ULL << 22) - 1ULL) << 42;  

    // some const for templates
    constexpr u64 WIDTH_ULTIMATE_ROW = 2,  SHIFT_ULTIMATE_ROW = 0,     FM_ULTIMATE_ROW = (1ULL << 2) - 1ULL;
    constexpr u64 WIDTH_SIZE = 7,          SHIFT_SIZE = 2,             FM_SIZE = (1ULL << 7) - 1ULL;
    constexpr u64 WIDTH_MAIN_BIT = 9,      SHIFT_MAIN_BIT = 9,         FM_MAIN_BIT = (1ULL << 9) - 1ULL;
    constexpr u64 WIDTH_BIT = 27,          SHIFT_BIT = 18,             FM_BIT = (1ULL << 27) - 1ULL;
    constexpr u64 WIDTH_WIN = 19,          SHIFT_WIN = 45,             FM_WIN = (1ULL << 19) - 1ULL;

    constexpr u64 WIDTH_PLAYER = 1,        SHIFT_PLAYER = 0,           FM_PLAYER = (1ULL << 1) - 1ULL;
    constexpr u64 WIDTH_VIS = 19,          SHIFT_VIS = 1,              FM_VIS = (1ULL << 19) - 1ULL;
    constexpr u64 WIDTH_PRIEV = 22,        SHIFT_PRIEV = 20,           FM_PRIEV = (1ULL << 22) - 1ULL;
    constexpr u64 WIDTH_OFFSET = 22,       SHIFT_OFFSET = 42,          FM_OFFSET = (1ULL << 22) - 1ULL;

    // making custom getters and setters
    enum class FieldSign { 
        Unsigned,
        Signed
    };

    template<u64 WIDTH, u64 SHIFT, u64 MASK, u64 FM, FieldSign Sign>
    struct FieldRef;

    // unsigned
    template<u64 WIDTH, u64 SHIFT, u64 MASK, u64 FM>
    struct FieldRef<WIDTH, SHIFT, MASK, FM, FieldSign::Unsigned> {
        u64 &x;

        inline operator u64() const noexcept {
            return (x & MASK) >> SHIFT;
        }

        inline void operator=(const u64 v) noexcept {
            x = (x & ~MASK) | (v << SHIFT);
        }

        inline void operator+=(const u64 d) noexcept {
            const u64 v = ((x & MASK) >> SHIFT) + d;
            x = (x & ~MASK) | (v << SHIFT);
        } 
    };

    // signed
    template<u64 WIDTH, u64 SHIFT, u64 MASK, u64 FM>
    struct FieldRef<WIDTH, SHIFT, MASK, FM, FieldSign::Signed> {
        u64 &x;

        inline operator i64() const noexcept {
            constexpr u64 SIGN = (1ULL << (WIDTH - 1));
            const u64 v = (x & MASK) >> SHIFT;
            return static_cast<i64>((v ^ SIGN) - SIGN);
        }

        inline void operator=(const u64 v) noexcept {
            x = (x & ~MASK) | ((static_cast<u64>(v) & FM) << SHIFT);
        }

        inline void operator+=(const u64 d) noexcept {
            const u64 v = (((x & MASK) >> SHIFT) + static_cast<u64>(d)) & FM;
            x = (x & ~MASK) | (v << SHIFT);
        }
    };

    template<u64 WIDTH, u64 SHIFT, u64 MASK, u64 FM, FieldSign Sign>
    struct Field {
        u64 *arr;

        inline FieldRef<WIDTH, SHIFT, MASK, FM, Sign>
        operator[](size_t i) const noexcept {
            return {arr[i]};
        }
    };

    static constexpr Field<WIDTH_ULTIMATE_ROW, SHIFT_ULTIMATE_ROW, MASK_ULTIMATE_ROW, FM_ULTIMATE_ROW, FieldSign::Unsigned> ultimate_row{ultimateRow_size_mainBit_bit_win};
    static constexpr Field<WIDTH_SIZE,         SHIFT_SIZE,         MASK_SIZE,         FM_SIZE,         FieldSign::Unsigned> size{ultimateRow_size_mainBit_bit_win};
    static constexpr Field<WIDTH_MAIN_BIT,     SHIFT_MAIN_BIT,     MASK_MAIN_BIT,     FM_MAIN_BIT,     FieldSign::Unsigned> main_bit{ultimateRow_size_mainBit_bit_win};
    static constexpr Field<WIDTH_BIT,          SHIFT_BIT,          MASK_BIT,          FM_BIT,          FieldSign::Unsigned> bit{ultimateRow_size_mainBit_bit_win};
    static constexpr Field<WIDTH_WIN,          SHIFT_WIN,          MASK_WIN,          FM_WIN,          FieldSign::Signed>   wins{ultimateRow_size_mainBit_bit_win};

    static constexpr Field<WIDTH_PLAYER,       SHIFT_PLAYER,       MASK_PLAYER,       FM_PLAYER,       FieldSign::Unsigned> player{player_vis_priev_offset};
    static constexpr Field<WIDTH_VIS,          SHIFT_VIS,          MASK_VIS,          FM_VIS,          FieldSign::Unsigned> vis{player_vis_priev_offset};
    static constexpr Field<WIDTH_PRIEV,        SHIFT_PRIEV,        MASK_PRIEV,        FM_PRIEV,        FieldSign::Unsigned> priev{player_vis_priev_offset};
    static constexpr Field<WIDTH_OFFSET,       SHIFT_OFFSET,       MASK_OFFSET,       FM_OFFSET,       FieldSign::Unsigned> offset{player_vis_priev_offset};
};


//! board
struct Board{
    u32 ultimate_board[3];
    uint_fast16_t main_board;

    u32 copy_ultimate_board[3];
    uint_fast16_t copy_main_board;

    static constexpr bool winning_board[] = {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    static constexpr u32 board_mask[] = {511, 261632, 133955584, 134217727}; // fi ultra_col, se ultra_col, th ultra_coll, all ultra_coll
    static constexpr u32 not_board_mask[] = {134217216, 133956095, 262143};

    inline bool check_for_win(i32 ultimate_row, i32 ultimate_col){
        return winning_board[(ultimate_board[ultimate_row] & board_mask[ultimate_col]) >> (9 * ultimate_col)];
    }

    inline bool check_for_copy_win(i32 ultimate_row, i32 ultimate_col){
        return winning_board[(copy_ultimate_board[ultimate_row] & board_mask[ultimate_col]) >> (9 * ultimate_col)];
    }
} board[2];

//! global functions
// input
static inline void initialize();
static inline void get_input();
static inline void get_first_input();

// moves utils
static inline void generate_all_moves(const u32 next_ultimate_rows, const u32 next_ultimate_cols);
static inline void generate_all_moves_copy(const u32 next_ultimate_rows, const u32 next_ultimate_cols);

// making moves
static inline void make_move();
static inline pair<u32, u32> get_next_board_copy(u32 bit);
static inline pair<u32, u32> get_next_board(u32 bit);

// bots
static inline i32 flat_mc(int player, u32 next_ultimate_rows, u32 next_ultimate_cols);
static inline i32 simulate_till_end(int player, u32 next_ultimate_rows, u32 next_ultimate_cols);

// debug
static void print_ultimate_board();
static void print_main_baord();

static inline double uct(i32 idx, double c);
static inline void select();
static inline pair<i32, i32> expand(); // [win, vis];
static inline void backtrack(i32 win, i32 vis); 

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

template<i32 PLAYER, i32 ULTIMATE_ROW>
static inline void add_son_impl(u32 &moves){
    i32 bit = __builtin_ctz(moves);
    Node::priev[Node::free_idx] = Node::idx;
    Node::bit[Node::free_idx] = bit;
    Node::ultimate_row[Node::free_idx] = ULTIMATE_ROW;
    Node::size[Node::free_idx] = 0;
    Node::player[Node::free_idx] = PLAYER;
    Node::wins[Node::free_idx] = 0;
    Node::vis[Node::free_idx] = 0;

    board[PLAYER].ultimate_board[ULTIMATE_ROW] |= (1 << bit);

    Node::main_bit[Node::free_idx] = board[PLAYER].check_for_win(ULTIMATE_ROW, bit / 9) ? (3 * ULTIMATE_ROW) + bit / 9 : -1;
    if(Node::main_bit[Node::free_idx] != -1) board[PLAYER].main_board |= (1 << Node::main_bit[Node::free_idx]);

    auto [next_ultimate_rows, next_ultimate_cols] = get_next_board(bit);
    Node::vis[Node::free_idx] = no_sim;
    Node::wins[Node::free_idx] = flat_mc(PLAYER^1, next_ultimate_rows, next_ultimate_cols);

    board[PLAYER].ultimate_board[ULTIMATE_ROW] ^= (1 << bit);
    if(Node::main_bit[Node::free_idx] != -1) board[PLAYER].main_board ^= (1 << Node::main_bit[Node::free_idx]);

    Node::wins[Node::idx] += Node::wins[Node::free_idx];
    Node::vis[Node::idx] += Node::vis[Node::free_idx];

    moves ^= (1 << bit); 
    Node::free_idx++; 
    if(test and Node::free_idx + 10'000 > max_nodes){
        cout << "PANIC7" << endl;
    }
}

static inline void add_son(u32& moves, i32 ultimate_row, i32 player) {
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

static inline double uct(i32 idx, double c){
    double wins = Node::player[idx] == me ? Node::wins[idx] : Node::vis[idx] - Node::wins[idx];
    return (double) wins / Node::vis[idx] + c * utils::inverse_sqrt(Node::vis[idx]);
}

static inline void initialize(){
    Node::root = 0;
    Node::idx = 0;
    Node::free_idx = 1;

    Node::priev[Node::root] = 0;
    Node::wins[Node::root] = 0;
    Node::vis[Node::root] = 0;

    Node::bit[Node::root] = -1;
    Node::ultimate_row[Node::root] = -1;
    Node::main_bit[Node::root] = -1;

    Node::player[Node::root] = foe;
    Node::offset[Node::root] = 1; // first childre in 1

    get_first_input();
    utils::T0 = utils::get_time();
    make_move();
};

static inline void move_root(u32 bit, u32 ultimate_row){
    for(int i = Node::offset[Node::root]; i < Node::offset[Node::root] + Node::size[Node::root]; i++){
        if(Node::ultimate_row[i] == ultimate_row and Node::bit[i] == bit){
            board[foe].ultimate_board[ultimate_row] |= (1 << bit);
            if(Node::main_bit[i] != -1) board[foe].main_board |= (1 << Node::main_bit[i]);
            Node::root = i;
            Node::idx = i;
            return;
        }
    }

    if(test) cout << "PANIC1" << endl;
}

void debug_check_if_contains_son(u32 bit, u32 ultimate_row){
    for(int i = Node::offset[Node::root]; i < Node::offset[Node::root] + Node::size[Node::root]; i++){
        if(Node::ultimate_row[i] == ultimate_row and Node::bit[i] == bit){
            return;
        }
    }

    cout << "PANIC3" << endl;
}

static inline void get_input(){
    i32 row, col;
    cin >> row >> col;
    if(debug){
        cerr << row << ' ' << col << endl;
    }

    if(row != -1){
        auto [bit, ultimate_row] = pos_of_coord[row][col];
        move_root(bit, ultimate_row);
    }

    i32 s;
    cin >> s;
    if(debug){
        cerr << s << endl;
    }


    bool add_sons = Node::size[Node::root] == 0;
    if(add_sons){
        Node::size[Node::root] = s;
        Node::offset[Node::root] = Node::free_idx;
    }

    for(u32 i = 0; i < s; i++){
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

    if(test and Node::size[Node::root] != s){
        cout << "PANIC2 " << s << ' ' << Node::size[Node::root] << endl;
    }
}

static inline void get_first_input(){
    i32 row, col;
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
    
    uint size; cin >> size;
    Node::size[Node::root] = size; // I'm lazy it's faster to write this than custom >> operator

    if(debug){
        cerr << Node::size[Node::root] << endl;
    }

    for(u32 i = 0; i < Node::size[0]; i++){
        cin >> row >> col;
        auto [bit, ultimate_row] = pos_of_coord[row][col];
        bit = 1 << bit;
        add_son(bit, ultimate_row, me); 
        if(debug){
            cerr << row << ' ' << col << endl;
        }
    }
}



static inline void generate_all_moves_copy(const u32 next_ultimate_rows, const u32 next_ultimate_cols){
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
static inline void generate_all_moves(const u32 next_ultimate_rows, const u32 next_ultimate_cols){
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

static inline pair<u32, u32> pick_random_move() {
    u32 cnt = (Moves::fi != 0) + (Moves::se != 0) + (Moves::th != 0);
    u32 k = utils::random_with_boud(cnt);

    if(Moves::fi) {
        if(!k) return {fi, Moves::fi};
        k--;
    } 
    if(Moves::se) {
        if(!k) return {se, Moves::se};
    }

    return {th, Moves::th};
}

static inline i32 simulate_till_end(int player, u32 next_ultimate_rows, u32 next_ultimate_cols){
    generate_all_moves_copy(next_ultimate_rows, next_ultimate_cols);

    while(Moves::fi | Moves::se | Moves::th){

        // get random move
        auto [board_idx, chosed_move] = pick_random_move();
        const i32 move = pick_random_bit(chosed_move, __builtin_popcountll(chosed_move));
        const i32 log2move = __builtin_ctz(move);
        
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

static i32 flat_mc(int player, u32 next_ultimate_rows, u32 next_ultimate_cols){
    i32 wins = 0;
    
    for(u32 i = 0; i < no_sim; i++){
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
static inline pair<u32, u32> get_next_board(u32 bit){
    u32 next_ultiamte_row = (bit % 9) / 3;
    u32 next_ultimate_col = bit % 3;

    // if subgame is finished
    if(board[foe].main_board & (1 << (next_ultiamte_row * 3 + next_ultimate_col)) or board[me].main_board & (1 << (next_ultiamte_row * 3 + next_ultimate_col)))
        return {all, all};

    // if there is still palce
    if((~(board[foe].ultimate_board[next_ultiamte_row] | board[me].ultimate_board[next_ultiamte_row])) & Board::board_mask[next_ultimate_col]) 
        return {next_ultiamte_row, next_ultimate_col};
    
    return {all, all};
}

static inline pair<u32, u32> get_next_board_copy(u32 bit){
    u32 next_ultiamte_row = (bit % 9) / 3;
    u32 next_ultimate_col = bit % 3;

    // if subgame is finished
    if(board[foe].copy_main_board & (1 << (next_ultiamte_row * 3 + next_ultimate_col)) or board[me].copy_main_board & (1 << (next_ultiamte_row * 3 + next_ultimate_col)))
        return {all, all};

    // if there is still palce
    if((~(board[foe].copy_ultimate_board[next_ultiamte_row] | board[me].copy_ultimate_board[next_ultiamte_row])) & Board::board_mask[next_ultimate_col]) 
        return {next_ultiamte_row, next_ultimate_col};
    
    return {all, all};
}

static inline void select(){
    while(Node::size[Node::idx] != 0){
        double best_uct = -inf;
        i32 best_arg = 0;

        const double c = 0.75 * sqrt(log(static_cast<double>(Node::vis[Node::idx])));
        for(i32 i = Node::offset[Node::idx]; i < Node::offset[Node::idx] + Node::size[Node::idx]; i++){
            double UCT = uct(i, c);

            if(UCT > best_uct){
                best_uct = UCT;
                best_arg = i;
            }
        }

        Node::idx = best_arg;
        if(Node::main_bit[Node::idx] != -1) board[Node::player[best_arg]].main_board |= (1 << Node::main_bit[Node::idx]);
        board[Node::player[best_arg]].ultimate_board[Node::ultimate_row[Node::idx]] |= (1 << Node::bit[Node::idx]);
    }
}

static inline void backtrack(i32 win, i32 vis){ 
    while(Node::idx != Node::root){

        Node::wins[Node::priev[Node::idx]] += win;
        Node::vis[Node::priev[Node::idx]] += vis;

        board[Node::player[Node::idx]].ultimate_board[Node::ultimate_row[Node::idx]] ^= (1 << Node::bit[Node::idx]);
        if(Node::main_bit[Node::idx] != -1) board[Node::player[Node::idx]].main_board ^= (1 << Node::main_bit[Node::idx]); 

        Node::idx = Node::priev[Node::idx];
    }
}

static inline pair<i32, i32> expand(){
    i32 prev_w = Node::wins[Node::idx];
    i32 prev_v = Node::vis[Node::idx];

    auto [next_ultimate_rows, next_ultimate_cols] = get_next_board(Node::bit[Node::idx]);
    generate_all_moves(next_ultimate_rows, next_ultimate_cols);
    u32 moves_fi = Moves::fi, moves_se = Moves::se, moves_th = Moves::th;

    i32 player = Node::player[Node::idx] ^ 1;
    Node::offset[Node::idx] = Node::free_idx;
    Node::size[Node::idx] = __builtin_popcountll(moves_fi) + __builtin_popcountll(moves_se) + __builtin_popcountll(moves_th);

    while(moves_fi){
        add_son(moves_fi, 0, player);
    }

    while(moves_se){
        add_son(moves_se, 1, player);
    }

    while(moves_th){
        add_son(moves_th, 2, player);
    }

    if(Node::size[Node::idx] == 0){
        if(Board::winning_board[board[me].main_board]) Node::wins[Node::idx] += no_sim;
        else if(Board::winning_board[board[foe].main_board]) Node::wins[Node::idx] += -no_sim;
        else Node::wins[Node::idx] += __builtin_popcountll(board[me].main_board) > __builtin_popcountll(board[foe].main_board) ?  no_sim : -no_sim;

        Node::vis[Node::idx] += no_sim;
    }

    return {Node::wins[Node::idx] - prev_w, Node::vis[Node::idx] - prev_v};
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
    for(u32 i = Node::offset[Node::idx]; i < Node::offset[Node::idx] + Node::size[Node::idx]; i++){
        double score = Node::wins[i];
        if(score > best_score){
            best_score = score;
            arg_best_score = i;
        }
    }

    // make move
    if(test and Node::player[Node::idx] != foe){
        cout << "PANIC6" << endl;
    }

    board[me].ultimate_board[Node::ultimate_row[arg_best_score]] |= (1 << Node::bit[arg_best_score]);
    if(Node::main_bit[arg_best_score] != -1) board[me].main_board |= (1 << Node::main_bit[arg_best_score]);

    Node::idx = arg_best_score;
    Node::root = arg_best_score;

    auto [row, col] = coord_of_pos[Node::ultimate_row[arg_best_score]][Node::bit[arg_best_score]];
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