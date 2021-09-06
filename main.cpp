#include <bits/stdc++.h>
#define REP(i, n) for (int i = 0; (i) < (int)(n); ++ (i))
#define REP3(i, m, n) for (int i = (m); (i) < (int)(n); ++ (i))
#define REP_R(i, n) for (int i = (int)(n) - 1; (i) >= 0; -- (i))
#define REP3R(i, m, n) for (int i = (int)(n) - 1; (i) >= (int)(m); -- (i))
#define ALL(x) std::begin(x), std::end(x)
using namespace std;

class xor_shift_128 {
public:
    typedef uint32_t result_type;
    xor_shift_128(uint32_t seed = 42) {
        set_seed(seed);
    }
    void set_seed(uint32_t seed) {
        a = seed = 1812433253u * (seed ^ (seed >> 30));
        b = seed = 1812433253u * (seed ^ (seed >> 30)) + 1;
        c = seed = 1812433253u * (seed ^ (seed >> 30)) + 2;
        d = seed = 1812433253u * (seed ^ (seed >> 30)) + 3;
    }
    uint32_t operator() () {
        uint32_t t = (a ^ (a << 11));
        a = b; b = c; c = d;
        return d = (d ^ (d >> 19)) ^ (t ^ (t >> 8));
    }
    static constexpr uint32_t max() { return numeric_limits<result_type>::max(); }
    static constexpr uint32_t min() { return numeric_limits<result_type>::min(); }
private:
    uint32_t a, b, c, d;
};

constexpr int N = 16;
constexpr int M = 5000;
constexpr int T = 1000;

struct vegetable_t {
    int y;
    int x;
    int start;
    int end;  // including
    int value;
};

using command_t = array<int, 4>;

const command_t command_pass = {{-1, -1, -1, -1}};

command_t command_buy(int y, int x) {
    return {{y, x, -1, -1}};
}

command_t command_move(int y, int x, int ny, int nx) {
    return {{y, x, ny, nx}};
}

constexpr array<int, 4> DIRS = {{0, 1, 2, 3}};
constexpr array<int, 4> DIR_Y = {-1, 1, 0, 0};
constexpr array<int, 4> DIR_X = {0, 0, 1, -1};
inline bool is_on_field(int y, int x) {
    return 0 <= y and y < N and 0 <= x and x < N;
}

inline uint16_t pack_point(int y, int x) {
    return (y << 8) + x;
}

inline pair<int, int> unpack_point(int packed) {
    return {packed >> 8, packed & ((1 << 8) - 1)};
}

template <class RandomEngine>
array<command_t, T> solve(const array<vegetable_t, M>& vegetables, RandomEngine& gen, chrono::high_resolution_clock::time_point clock_end) {
    chrono::high_resolution_clock::time_point clock_begin = chrono::high_resolution_clock::now();
    array<command_t, T> ans;
    REP (turn, T) {
        ans[turn] = command_pass;
    }
    return ans;
}

int main() {
    constexpr auto TIME_LIMIT = chrono::milliseconds(2000);
    chrono::high_resolution_clock::time_point clock_begin = chrono::high_resolution_clock::now();
    xor_shift_128 gen(20210425);

    int n, m, t; cin >> n >> m >> t;
    assert (n == N);
    assert (m == M);
    assert (t == T);
    array<vegetable_t, M> vegetables;
    REP (i, M) {
        cin >> vegetables[i].y;
        cin >> vegetables[i].x;
        cin >> vegetables[i].start;
        cin >> vegetables[i].end;
        cin >> vegetables[i].value;
    }
    auto ans = solve(vegetables, gen, clock_begin + chrono::duration_cast<chrono::milliseconds>(TIME_LIMIT * 0.95));
    for (auto cmd : ans) {
        cout << cmd[0];
        REP3 (i, 1, 4) {
            if (cmd[i] != -1) {
                cout << ' ' << cmd[i];
            }
        }
        cout << '\n';
    }
    return 0;
}
