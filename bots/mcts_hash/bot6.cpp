#include <bits/stdc++.h>
#include <immintrin.h>
using namespace std;

/*
    game: ultimate tic tac toe
    algoright: mcts + tree reuse + hash
    performance: ? 

*/

// TODO w następnej wersji zrób tak, że dla danego node spamiętujesz Hashe dzieci (podobnie jak w bot5)

// #pragma GCC optimize("Ofast,inline,tracer")
// #pragma GCC optimize("unroll-loops,vpt,split-loops,unswitch-loops")
// #pragma GCC optimize("Ofast,inline,unroll-loops")
// #pragma GCC target("aes,abm,align-stringops,avx,avx2,bmi,bmi2,crc32,cx16,f16c,fma,fsgsbase,fxsr,hle,ieee-fp,lzcnt,mmx,movbe,mwait,pclmul,popcnt,rdrnd,sahf,sse,sse2,sse3,sse4,sse4.1,sse4.2,ssse3,xsave,xsaveopt") 

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! consts !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// remember about random()
constexpr bool debug = false;
constexpr bool turbo_debug = false;
constexpr bool test = true;
constexpr int_fast32_t no_sim = 1;
constexpr int_fast32_t inf = 1e9 + 7;
constexpr int_fast32_t max_nodes = 3'000'000;
int_fast32_t m_time = 990'000;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! enums !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! Hash !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
struct Hash {
    //? static
    static const int64_t random_ultimate_board[2][32][3];   // <player, bit, ultimate_row> 
    static const int64_t random_main_board[10];             // next board bit, (9 -> all)
    static const int64_t Hash::random_player[2];

    //? data
    int64_t hash;                                           // our hash
    int_fast32_t player;
    int_fast32_t next_main_bit;                             // (9 -> all)

    //? methods
    Hash();
    void add_bit(uint_fast32_t bit, uint_fast32_t ultimate_row);                // add bit + change_next_sub_board + change_player
    void change_next_sub_board(uint_fast32_t bit, uint_fast32_t ultimate_row);  // coord of newly added point
    void change_player();

    pair<uint_fast32_t, uint_fast32_t> get_next_board(); // <next_ultimate_rows, next_ultimate_cols>
};

//???????????????????????????????????????????????????????????????????????????????????????????????????????????
Hash::Hash() {
    hash = random_player[foe] ^ Hash::random_main_board[9];
    player = foe;
    next_main_bit = 9; 
}
pair<uint_fast32_t, uint_fast32_t> Hash::get_next_board(){
    if(next_main_bit == 9) return {all, all};
    return {next_main_bit / 3, next_main_bit % 3};
}

void Hash::add_bit(uint_fast32_t bit, uint_fast32_t ultimate_row) {
    hash ^= Hash::random_ultimate_board[player][bit][ultimate_row];
    change_next_sub_board(bit, ultimate_row);
    change_player();
}

void Hash::change_next_sub_board(uint_fast32_t bit, uint_fast32_t ultimate_row) {
    hash ^= Hash::random_main_board[next_main_bit];
    next_main_bit = bit % 9;
    hash ^= Hash::random_main_board[next_main_bit];
}

void Hash::change_player() {
    hash ^= Hash::random_player[player];
    player ^= 1;
    hash ^= Hash::random_player[player];
}

const int64_t Hash::random_ultimate_board[2][32][3] = 
{
    {
        {  1525586134125041852LL, -2996996415091763326LL,  -979829467213699442LL },
        { -8683089949414890730LL,  6848543915769680425LL,  6929564676940999313LL },
        {  4279774972623744952LL,  7059464309643367482LL,  1802108136696289791LL },
        {  2189391563943646172LL,  3902466378601054505LL,  6927353344346594637LL },
        {  7589026338500961097LL, -7149463025259751594LL, -8231603103848041106LL },
        {  1232631860679261933LL, -1835994744989905282LL, -4248741214993825845LL },
        {   491115612596666202LL,  4204604356819848618LL, -3946209088287693971LL },
        { -4025105284048892587LL,  7334410543387760095LL,  8490919278908345346LL },
        { -8859371083095173017LL,   565021364231369026LL,  2043604862867729540LL },
        {  3654354943119154882LL,  2250057750074301954LL, -7271561711925928413LL },
        {  2141021655093900911LL,  2510840976987267865LL,  -572525009403162198LL },
        {  1597398493305152208LL, -7631794432753166629LL, -4332828165175310143LL },
        {  8168112016668205526LL, -1108635376067932495LL,  3247806473023858115LL },
        { -4989930066528442760LL, -8717845421126775046LL, -8640568384735922273LL },
        { -7317436895897967084LL,  5529585069469201669LL,   491115612596666202LL },
        { -3823001545720812455LL, -6122308641148451213LL, -4349646755933315115LL },
        {  7143504661004821516LL,  3693968746535758396LL,  9139172282886104902LL },
        {  9111032783606829686LL, -6084286624765123878LL,  5535450984904480353LL },
        { -8412951496007291467LL,  2917858366388876689LL,  4256474004226588761LL },
        { -8367505404376038616LL, -3390841157644134797LL, -2498968948019684679LL },
        { -1532906506229234971LL, -3654788529659101238LL,  7941864316487735853LL },
        { -4777785434840136719LL,   265248671708662268LL, -2075007120512710684LL },
        { -2800197660868450506LL, -1840823387155315766LL,  7408909406447983361LL },
        {  5315697808037779021LL, -1210595486691513678LL, -2399684950876306048LL },
        {   841262509853839011LL, -6016115341218193328LL, -6462784335881614781LL },
        { -2968302375707197917LL, -6960051996943961648LL,   338399123150065483LL },
        { -1401456220705341757LL, -8358924293051593805LL, -6638817207143911620LL },
        {   911969335800906202LL,  3106499270972060145LL,  4385305978742582864LL },
        {  6164900025502970743LL,  6798926373317911390LL, -9120313283047543702LL },
        { -5743452281028039078LL,  7287568675318636299LL,  3405860852640245725LL },
        { -7399085765727204593LL,  2762006560500114584LL, -3257086802083350213LL },
        { -1521038219350078033LL, -8877633106208031406LL, -3302053651131363582LL },
    },
    {
        {  2395131010639850201LL, -5567258283158543665LL, -1585214809993369455LL },
        {  2076233676637274353LL,  4752077445567739268LL,  2299715032594632876LL },
        {  9010375214153600266LL, -5166831694144346437LL, -4539422331997384970LL },
        {  5482509382288419935LL,  4714835012359515848LL,   214049122845331423LL },
        {   122746792345002809LL, -5184535214473596725LL,  5595491718182608898LL },
        {  8028924201384455331LL,  3148755776010370579LL, -4969976039177745488LL },
        { -6466704187121380686LL, -5625867613272298124LL,  6908864588277868328LL },
        {  7225577756371973587LL,  1284384804036849262LL, -8966373626155266243LL },
        {  8298843172873074843LL,  1116403388819474274LL,  9018110866111889144LL },
        {  5053571565008427064LL, -2980052566411501744LL, -4080260186286156765LL },
        {  6590714775810526463LL,  7152451156445150519LL,  3959765247313387749LL },
        { -4793999030040702999LL,  2281754211109857219LL, -1979179884319405321LL },
        {   485963237363254626LL, -6050925205063689567LL,  3689181000242790961LL },
        { -6204609557165591570LL,  2435733285065675138LL,  1913312665749077743LL },
        { -5365308617567779126LL,  4039423275164219329LL,  4479763545126942383LL },
        {  8394298313919942766LL,   746611680166735471LL, -6917264947059979059LL },
        {  6825059835516420408LL, -9193251468749404659LL, -6973103593667570990LL },
        {  3818941147972151119LL,  1582974860944026365LL, -8762805389860178845LL },
        { -6791678875026975034LL,  4136270214360348949LL,  1127180934608408403LL },
        { -8998579662066626687LL,  6409806033777926822LL,  6271803377194382751LL },
        { -2783321298839442148LL,  2686846867736523614LL, -2516188590466654679LL },
        {  3885487322054812942LL, -4199477500414798550LL,  2851062045886888638LL },
        { -8604946920721389430LL,  6844182210895152287LL,  6737970910286582713LL },
        { -2623433003249252207LL,  3108296315991235919LL,  2779250059236258667LL },
        { -6930081986984038633LL,  7392067266912085904LL, -4601401531327700626LL },
        {  1924844282656287198LL,  7847597087546695379LL, -7920042978399293153LL },
        { -1416550391497114776LL, -8469121082736070402LL, -829283436753308184LL  },
        {  1904239717270290506LL,  4966808584547767621LL,  229753900015507712LL  },
        {  3474275354723036054LL, -4299227859015595838LL, -1166320482828057619LL },
        { -6403575433833861379LL, -8175829979797988963LL,  7988004716207246706LL },
        { -4158753766641090128LL, -8084164855525904783LL, -6154545236168979118LL },
        {  3656301825044434998LL,  5107744845088875370LL, -6915234946198183415LL },
    }
};

// <player, bit>
const int64_t Hash::random_main_board[10] = 
{
    2395131010639850201LL, -5567258283158543665LL, -1585214809993369455LL,
    2076233676637274353LL,  4752077445567739268LL,  2299715032594632876LL,
    9010375214153600266LL, -5166831694144346437LL, -4539422331997384970LL,
    5482509382288419935LL 
};

const int64_t Hash::random_player[2] = { 1525586134125041852LL, -2996996415091763326LL };

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! Node !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
struct Node {
    // node info
    int_fast32_t wins, vis;

    // moving
    uint_fast32_t to_add[3];    
    uint_fast32_t moves[3];     // moves that node can make 
    int_fast32_t size;          // how many sons node has now
    int_fast32_t max_size;      // popcount of moves

    // methods
    Node();
    void Node::generate_moves(uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols);
    double uct(double c, uint_fast32_t player);
    int_fast32_t add_son();
};
//???????????????????????????????????????????????????????????????????????????????????????????????????????????
Node::Node() {
    wins = vis = 0;
    size = 0;
    max_size = inf;
    // garbage in moves[], to_add and max_size
};

int_fast32_t Node::add_son() {
    // generate one son and put it in sons_hash[0]
    graph.generate_one_son_hash(*this);
    Move& move = graph.sons_hash[0];

    // teporarily add bit do board
    int_fast32_t main_bit = board[move.hash.player].add_bit(move.bit, move.ultimate_row);
    auto [next_ultimate_rows, next_ultimate_cols] = graph.curr_hash.get_next_board();

    // make sim
    int_fast32_t son_wins = flat_mc(graph.curr_hash.player, next_ultimate_rows, next_ultimate_cols);

    // remove added bit
    board[move.hash.player].remove_bit(move.bit, move.ultimate_row, main_bit);
    wins += son_wins; 
    vis += no_sim;

    return son_wins;
}

double Node::uct(double c, uint_fast32_t player){
    if(vis == 0) return inf;

    double real_wins = player == me ? wins : vis - wins;
    return (double) real_wins / vis + c / sqrt(vis);
}

void Node::generate_moves(uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols) {
    moves[0] = moves[1] = moves[2] = 0;
    uint_fast32_t start = (next_ultimate_rows == all ? 0 : next_ultimate_rows), end = (next_ultimate_rows == all ? 2 : next_ultimate_rows);
    for(; start <= end; start++){
        moves[start] = ~(board[me].ultimate_board[start] | board[foe].ultimate_board[start]);

        if((board[me].main_board | board[foe].main_board) & (1 << (start * 3))) moves[start] &= Board::not_board_mask[0];
        if((board[me].main_board | board[foe].main_board) & (1 << (start * 3 + 1))) moves[start] &= Board::not_board_mask[1];
        if((board[me].main_board | board[foe].main_board) & (1 << (start * 3 + 2))) moves[start] &= Board::not_board_mask[2];

        moves[start] &= Board::board_mask[next_ultimate_cols];
    }

    to_add[0] = moves[0];
    to_add[1] = moves[1];
    to_add[2] = moves[2];
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! Move !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct Move {
    Hash hash;
    uint_fast32_t bit, ultimate_row;
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! Graph !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
class Graph {
private:
    unordered_map<int64_t, Node> nodes;     // <hash, node>
    void add_sons_hash(uint_fast32_t bit, uint_fast32_t ultimate_row);

public:
    // data
    Move sons_hash[81];
    int_fast32_t sons_hash_size;
    Hash curr_hash, root_hash;

    // methods
    void try_add_hash(Hash h);
    Node& add_hash(Hash h);              // use only when you are certain that we lack this hash
    Node& get_node(Hash h);

    void generate_all_sons_hash(Node& node); // this must be a curr node (from curr hash)
    void generate_one_son_hash(Node& node);
    
} graph;

//???????????????????????????????????????????????????????????????????????????????????????????????????????????
void Graph::add_sons_hash(uint_fast32_t bit, uint_fast32_t ultiamte_row) {
    // create new hash
    Hash new_hash = curr_hash;
    new_hash.add_bit(bit, ultiamte_row);

    // add this hash
    sons_hash[sons_hash_size++] = {new_hash, bit, ultiamte_row};
}

void Graph::generate_all_sons_hash(Node& node) {
    sons_hash_size = 0;
    node.to_add[0] = node.moves[0];
    node.to_add[1] = node.moves[1];
    node.to_add[2] = node.moves[2];

    while(node.to_add[0]) {
        uint32_t bit = __builtin_ctz(node.to_add[0]);
        add_sons_hash(bit, 0);
        node.to_add[0] ^= 1 << bit;
    }

    while(node.to_add[1]) {
        uint32_t bit = __builtin_ctz(node.to_add[1]);
        add_sons_hash(bit, 1);
        node.to_add[1] ^= 1 << bit;
    }

    while(node.to_add[2]) {
        uint32_t bit = __builtin_ctz(node.to_add[2]);
        add_sons_hash(bit, 2);
        node.to_add[2] ^= 1 << bit;
    }
}

void Graph::generate_one_son_hash(Node& node) {
    sons_hash_size = 0;
    if(node.to_add[0]) {
        uint_fast32_t bit = __builtin_ctz(node.to_add[0]);
        add_sons_hash(bit, 0);
        node.to_add[0] ^= 1 << bit;

    }  else if(node.to_add[1]) {
        uint_fast32_t bit = __builtin_ctz(node.to_add[1]);
        add_sons_hash(bit, 1);
        node.to_add[1] ^= 1 << bit;

    } else if(node.to_add[2]) {
        uint_fast32_t bit = __builtin_ctz(node.to_add[2]);
        add_sons_hash(bit, 2);
        node.to_add[2] ^= 1 << bit;

    } else {
        cout << "PANIC15" << endl;
    }
}

Node& Graph::get_node(Hash h) {
    if(test && !nodes.contains(h.hash)) {
        cout << "PANIC13" << endl;
    }

    return nodes[h.hash];
}

void Graph::try_add_hash(Hash h) {
    nodes.emplace(h.hash, Node());
}

Node& Graph::add_hash(Hash h) {
    auto [itr, inserted] = nodes.emplace(h.hash, Node());
    if(test and !inserted) {
        cout << "Panic14" << endl;
    }

    return itr -> second;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! utils !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! Pos !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
struct Pos {
    uint_fast32_t bit;
    uint_fast32_t ultra_row;
};

constexpr Pos pos_of_coord[9][9] = {
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

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! Board !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
struct Board {
    uint_fast32_t ultimate_board[3];
    uint_fast16_t main_board;

    uint_fast32_t copy_ultimate_board[3];
    uint_fast16_t copy_main_board;

    //? static
    static const bool winning_board[];
    static const uint_fast32_t board_mask[]; // fi ultra_col, se ultra_col, th ultra_coll, all ultra_coll
    static const uint_fast32_t not_board_mask[];

    // moves_stack
    static uint_fast32_t moves_stack_size;
    static pair<Move, int_fast32_t> moves_stack[81];

    //? methods
    inline bool check_for_win(int_fast32_t ultimate_row, int_fast32_t ultimate_col);
    inline bool check_for_copy_win(int_fast32_t ultimate_row, int_fast32_t ultimate_col);

    // adds bit to ultimate_board and maybe main_board, returns lit bit in main_board : -1 
    inline int_fast32_t add_bit(uint_fast32_t bit, uint_fast32_t ultimate_row);
    inline void remove_bit(uint_fast32_t bit, uint_fast32_t ultimate_row, int_fast32_t main_bit); // main_bit = -1 -> don't remove any main_board bit

    inline int_fast32_t add_bit_copy(uint_fast32_t bit, uint_fast32_t ultimate_row);

    // adds node to the board
    // inline void add_node(int_fast32_t idx);

    // remove node from the board
    // inline void remove_node(int_fast32_t idx);

} board[2];

//???????????????????????????????????????????????????????????????????????????????????????????????????????????
const bool Board::winning_board[] = {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
const uint_fast32_t Board::board_mask[] = {511, 261632, 133955584, 134217727}; // fi ultra_col, se ultra_col, th ultra_coll, all ultra_coll
const uint_fast32_t Board::not_board_mask[] = {134217216, 133956095, 262143};


inline void Board::remove_bit(uint_fast32_t bit, uint_fast32_t ultimate_row, int_fast32_t main_bit){
    if(test and ultimate_board[ultimate_row] & (1 << bit) == 0) {
        cout << "PANIC16" << endl;
    }
    if(test and main_bit != -1 and main_board & (1 << main_bit) == 0) {
        cout << "PANIC17" << endl;
    }

    ultimate_board[ultimate_row] ^= (1 << bit);
    if(main_bit != -1) main_board ^= (1 << main_bit);
}

inline bool Board::check_for_win(int_fast32_t ultimate_row, int_fast32_t ultimate_col){
    return winning_board[(ultimate_board[ultimate_row] & board_mask[ultimate_col]) >> (9 * ultimate_col)];
}

inline bool Board::check_for_copy_win(int_fast32_t ultimate_row, int_fast32_t ultimate_col){
    return winning_board[(copy_ultimate_board[ultimate_row] & board_mask[ultimate_col]) >> (9 * ultimate_col)];
}

inline int_fast32_t Board::add_bit(uint_fast32_t bit, uint_fast32_t ultimate_row) {
    if(test and check_for_win(ultimate_row, bit / 9)) {
        cout << "PANIC8" << endl;
    }

    // add bit on the board
    ultimate_board[ultimate_row] |= (1 << bit);
    int_fast32_t main_bit = check_for_win(ultimate_row, bit / 9) ? (3 * ultimate_row) + bit / 9 : -1;
    if(main_bit != -1) main_board |= (1 << main_bit);

    // add bit to the curr hash
    graph.curr_hash.add_bit(bit, ultimate_row);

    return main_bit;
}

inline int_fast32_t Board::add_bit_copy(uint_fast32_t bit, uint_fast32_t ultimate_row) {
    if(test and check_for_copy_win(ultimate_row, bit / 9)) {
        cout << "PANIC9" << endl;
    }

    copy_ultimate_board[ultimate_row] |= (1 << bit);
    int_fast32_t main_bit = check_for_copy_win(ultimate_row, bit / 9) ? (3 * ultimate_row) + bit / 9 : -1;
    if(main_bit != -1) copy_main_board |= (1 << main_bit);

    return  main_bit;
}

// inline void Board::add_node(int_fast32_t idx) {
//     const Node &node = nodes[idx];
//     if(test and check_for_win(node.ultimate_row, node.bit / 9)) {
//         cout << "PANIC10" << endl;
//     }

//     ultimate_board[node.ultimate_row] |= (1 << node.bit);
//     if(node.main_bit != -1) main_board |= (1 << node.main_bit);
// }

// inline void Board::remove_node(int_fast32_t idx) { 
//     const Node &node = nodes[idx];
//     if(test and ultimate_board[node.ultimate_row] & (1 << (node.bit)) == 0) {
//         cout << "PANIC11" << endl;
//     }
//     if(test and node.main_bit != -1 and main_board & (1 << (node.main_bit)) == 0) {
//         cout << "PANIC12" << endl;
//     }

//     ultimate_board[node.ultimate_row] ^= (1 << node.bit);
//     if(node.main_bit != -1) main_board ^= (1 << node.main_bit);
// }


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! Global functions !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// input
static inline void initialize();
static inline void get_input();
static inline void get_first_input();
// static inline int_fast32_t add_son(uint_fast32_t &moves, int_fast32_t ultimate_row, int_fast32_t player);

// moves utils
// static inline void generate_all_moves(uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols);   // moved from global to Node
static inline void generate_all_moves_copy(uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols, array<uint_fast32_t, 3> &moves);

// making moves
static inline void move_root(uint_fast32_t bit, uint_fast32_t ultimate_row);
static inline void make_move();
static inline pair<uint_fast32_t, uint_fast32_t> get_next_board_copy(uint_fast32_t bit);
static inline pair<uint_fast32_t, uint_fast32_t> get_next_board(uint_fast32_t bit);

// bots
static inline int_fast32_t flat_mc(int player, uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols);
static inline int_fast32_t simulate_till_end(int player, uint_fast32_t next_ultimate_rows, uint_fast32_t next_ultimate_cols);

// debug
static void print_ultimate_board();
static void print_main_baord();
void debug_check_if_contains_son(uint_fast32_t bit, uint_fast32_t ultimate_row);

// mcts
// static inline double uct(int_fast32_t idx, double c);   // moved from global function to Node
static inline void select();
static inline pair<int_fast32_t, int_fast32_t> expand(); // [win, vis];
static inline void backtrack(int_fast32_t win, int_fast32_t vis); 

//???????????????????????????????????????????????????????????????????????????????????????????????????????????

static inline void initialize(){
    // set hash
    graph.curr_hash = Hash();
    graph.root_hash = Hash();

    get_first_input();
    utils::T0 = utils::get_time();
    make_move();
};

static inline void move_root(uint_fast32_t bit, uint_fast32_t ultimate_row){
    board[foe].add_bit(bit, ultimate_row);
    graph.try_add_hash(graph.curr_hash);
    graph.root_hash = graph.curr_hash;
}

// void debug_check_if_contains_son(uint_fast32_t bit, uint_fast32_t ultimate_row){
//     for(int i = nodes[Node::root].offset; i < nodes[Node::root].offset + nodes[Node::root].size; i++){
//         if(nodes[i].ultimate_row == ultimate_row and nodes[i].bit == bit){
//             return;
//         }
//     }

//     cout << "PANIC3" << endl;
// }

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


    for(uint_fast32_t i = 0; i < s; i++){
        cin >> row >> col;
        if(debug){
            cerr << row << ' ' << col << endl;
        }
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

        board[foe].add_bit(bit, ultimate_row);
        graph.curr_hash.change_player();    // player still foe
        graph.root_hash = graph.curr_hash;
    }

    // add hash with the current board
    Node& root_node = graph.add_hash(graph.curr_hash);

    // modify root_node
    cin >> root_node.max_size;
    auto [next_ultimate_rows, next_ultimate_cols] = graph.root_hash.get_next_board();

    if(debug){
        cerr << root_node.max_size << endl;
    }

    root_node.generate_moves(next_ultimate_rows, next_ultimate_cols);   // add moves
    graph.generate_all_sons_hash(root_node); 

    for(uint_fast32_t i = 0; i < root_node.size; i++){
        cin >> row >> col;
        auto [bit, ultimate_row] = pos_of_coord[row][col];
        graph.try_add_hash(graph.sons_hash[i].hash);
        root_node.size++;
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
        const int_fast32_t ultimate_row = ok_moves[utils::random_with_boud(ok_moves_cnt)];
        const uint_fast32_t move = _pdep_u32(1u << (utils::random() % __builtin_popcountll(moves[ultimate_row])), moves[ultimate_row]);
        const int_fast32_t bit = __builtin_ctz(move);
        
        // make move
        board[player].add_bit_copy(bit, ultimate_row);

        tie(next_ultimate_rows, next_ultimate_cols) = get_next_board_copy(bit);
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
    // get node_root
    Node curr_node = graph.get_node(graph.curr_hash); // here curr_hash = root_hash

    while(curr_node.size == curr_node.max_size and curr_node.max_size) {
        graph.generate_all_sons_hash(curr_node);

        double best_uct = -inf;
        int_fast32_t arg_best = 0;
        const double c = 1.41 * sqrt(log(curr_node.vis + 1));

        for(int i = 0; i < graph.sons_hash_size; i++) {
            const Move& move = graph.sons_hash[i];
            Node& son = graph.get_node(move.hash);

            double sons_uct = son.uct(c, move.hash.player);

            if(sons_uct > best_uct) {
                best_uct = sons_uct;
                arg_best = i;
            }
        }

        // make best move
        const Move& best_move = graph.sons_hash[arg_best];
        auto main_bit = board[graph.curr_hash.player ^ 1].add_bit(best_move.bit, best_move.ultimate_row);
        curr_node = graph.get_node(graph.curr_hash);

        // remember best move
        Board::moves_stack[Board::moves_stack_size++] = {best_move, main_bit};
    }
}

static inline void backtrack(int_fast32_t win, int_fast32_t vis){ 
    while(Board::moves_stack_size){
        auto [move, main_bit] = Board::moves_stack[Board::moves_stack_size--];
        Node& node = graph.get_node(move.hash); // move.hast == curr.hash

        node.wins += win;
        node.vis += vis;

        board[move.hash.player].remove_bit(move.bit, move.ultimate_row, main_bit);
    }

    if(test and graph.curr_hash.hash != graph.root_hash.hash) {
        cout << "PANIC18" << endl;
    }
}

// static inline int_fast32_t add_son(uint_fast32_t &moves, int_fast32_t ultimate_row, int_fast32_t player){
//     int_fast32_t &pos = nodes[Node::idx].curr_free_idx;

//     int_fast32_t bit = __builtin_ctz(moves);
//     nodes[pos].priev = Node::idx;
//     nodes[pos].bit = bit;
//     nodes[pos].ultimate_row = ultimate_row;
//     nodes[pos].player = player;
    

//     // add bit
//     nodes[pos].main_bit = board[player].add_bit(bit, ultimate_row);

//     // test new node
//     auto [next_ultimate_rows, next_ultimate_cols] = get_next_board(bit);
//     nodes[pos].vis = no_sim;
//     nodes[pos].wins = flat_mc(player^1, next_ultimate_rows, next_ultimate_cols);

//     // remove node from board
//     board[player].remove_node(pos);

//     nodes[Node::idx].wins += nodes[pos].wins;
//     nodes[Node::idx].vis += nodes[pos].vis;

//     moves ^= (1 << bit); 
//     if(test and pos + 10'000 > max_nodes){
//         cout << "PANIC7" << endl;
//     }

//     return nodes[pos++].wins;
// }

static inline pair<int_fast32_t, int_fast32_t> expand(){
    Node& curr_node = graph.get_node(graph.curr_hash);
    if(curr_node.max_size == inf) {
        auto [next_ultimate_rows, next_ultimate_cols] = graph.curr_hash.get_next_board();
        curr_node.generate_moves(next_ultimate_rows, next_ultimate_cols);
    }

    int_fast32_t wins = curr_node.add_son();
    return {wins, no_sim};
}

static void make_move(){ 
    while(utils::elapsed_time() < m_time){ 
        for(int i = 0; i < 20; i++){
            select();
            auto [win, vis] = expand();
            backtrack(win, vis);
        }  
    }

    // for(int ddddddd = 0; ddddddd < 5; ddddddd++){
    //     for(int i = 0; i < 81; i++){
    //         select();
    //         auto [win, vis] = expand();
    //         backtrack(win, vis);
    //     }  
    // }

    // choose best move
    Node curr_node = graph.get_node(graph.curr_hash);
    graph.generate_all_sons_hash(curr_node);

    int arg_best_score = 0, best_score = -inf;
    for(int i = 0; i < graph.sons_hash_size; i++) {
        const Move& move = graph.sons_hash[i];
        Node& son = graph.get_node(move.hash);

        if(son.wins > best_score) {
            best_score = son.wins;
            arg_best_score = i;
        }
    }

    if(test and graph.curr_hash.player != foe){
        cout << "PANIC6" << endl;
    }

    // make move
    const Move& best_move = graph.sons_hash[arg_best_score];
    board[me].add_bit(best_move.bit, best_move.ultimate_row);

    // move root
    graph.root_hash = graph.curr_hash;

    // print ans
    auto [row, col] = coord_of_pos[best_move.ultimate_row][best_move.bit];
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

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! main !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int main(){
    initialize();
    m_time = 95'000;

    while(true){
        get_input();
        utils::T0 = utils::get_time();
        make_move();
        
        // if(test) cerr << Node::free_idx << endl;
    }
}