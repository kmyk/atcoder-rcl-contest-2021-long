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

int64_t squared(int64_t x) {
    return x * x;
}

int64_t cubed(int64_t x) {
    return x * x * x;
}

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

struct game_state {
    const array<vegetable_t, M> vegetables;

    array<command_t, T> ans;
    int turn = 0;
    int64_t money = 1;
    array<array<bool, N>, N> machine = {};
    int machine_count = 0;
    array<array<int, N>, N> field;

    explicit game_state(const array<vegetable_t, M> &vegetables_)
            : vegetables(vegetables_) {
        REP (y, N) {
            fill(ALL(field[y]), -1);
        }
    }

    array<array<int, N>, N> connected_index = {};
    vector<int> connected_size = {};
    int count_connected_machines(int y, int x) {
        if (connected_index[y][x] != -1) {
            return connected_size[connected_index[y][x]];
        }
        int index = connected_size.size();
        connected_size.push_back(0);
        auto go = [&](auto &&go, int y, int x) -> void {
            connected_index[y][x] = index;
            ++ connected_size[index];
            for (int dir : DIRS) {
                int ny = y + DIR_Y[dir];
                int nx = x + DIR_X[dir];
                if (is_on_field(ny, nx) and machine[ny][nx]) {
                    if (connected_index[ny][nx] == -1) {
                        go(go, ny, nx);
                    }
                }
            }
        };
        go(go, y, x);
        return connected_size[connected_index[y][x]];
    };

    int next_vegetable = 0;
    void update_add_vegetables() {
        while (next_vegetable < M and vegetables[next_vegetable].start == turn) {
            auto &vegetable = vegetables[next_vegetable];
            field[vegetable.y][vegetable.x] = next_vegetable;
            ++ next_vegetable;
        }
    };

    void update_remove_vegetables() {
        REP (y, N) {
            fill(ALL(connected_index[y]), -1);
        }
        connected_size.clear();

        REP (y, N) {
            REP (x, N) {
                if (field[y][x] != -1 and machine[y][x]) {
                    money += vegetables[field[y][x]].value * count_connected_machines(y, x);
                    field[y][x] = -1;
                }
                if (field[y][x] != -1 and vegetables[field[y][x]].end == turn) {
                    field[y][x] = -1;
                }
            }
        }
    };

    void update() {
        // update_add_vegetables();
        update_remove_vegetables();
        ++ turn;
    };

    void pass() {
        ans[turn] = command_pass;
        update();
    };

    void buy(int y, int x) {
        int64_t cost = cubed(machine_count + 1ll);
        assert (money >= cost);
        assert (not machine[y][x]);
        ans[turn] = command_buy(y, x);
        money -= cost;
        machine[y][x] = true;
        ++ machine_count;
        update();
    };

    void move(int y, int x, int ny, int nx) {
        assert (machine[y][x]);
        assert (not machine[ny][nx]);
        ans[turn] = command_move(y, x, ny, nx);
        machine[y][x] = false;
        machine[ny][nx] = true;
        update();
    };

};

template <class RandomEngine>
array<command_t, T> solve(const array<vegetable_t, M>& vegetables, RandomEngine& gen, chrono::high_resolution_clock::time_point clock_end) {
    chrono::high_resolution_clock::time_point clock_begin = chrono::high_resolution_clock::now();

    // NOTE: the left and right is flipped.
    const array<int, N> plan = {{
        0b0000000000000000,
        0b0000000000000000,
        0b1111111111111111,
        0b0000000000000001,
        0b0000000000000001,
        0b0000000000000001,
        0b0000000000000001,
        0b1111111111111111,
        0b0000000000000001,
        0b0000000000000001,
        0b0000000000000001,
        0b0000000000000001,
        0b1111111111111111,
        0b0000000000000000,
        0b0000000000000000,
        0b0000000000000000,
    }};
    vector<pair<int, int>> movable;
    array<array<bool, N>, N> fixed = {};

    game_state a(vegetables);
    int target = -1;
    auto go = [&]() {
        a.update_add_vegetables();

        vector<int> order;
        REP (y, N) {
            REP (x, N) {
                if (a.field[y][x] != -1) {
                    order.push_back(a.field[y][x]);
                }
            }
        }
        sort(ALL(order), [&](int i, int j) -> bool {
            return vegetables[i].value > vegetables[j].value;
        });

        if (cubed(a.machine_count + 1ll) <= a.money) {
            if (a.machine_count < 3) {
                for (int i : order) {
                    int y = vegetables[i].y;
                    int x = vegetables[i].x;
                    if (a.machine[y][x]) continue;
                    a.buy(y, x);
                    movable.emplace_back(y, x);
                    return;
                }
            } else {
                REP (y, N) {
                    REP (x, N) {
                        if ((plan[y] & (1 << x)) and not a.machine[y][x]) {
                            a.buy(y, x);
                            fixed[y][x] = true;
                            return;
                        }
                    }
                }
            }
        }

        array<array<vector<pair<int, int>>, N>, N> path = {};
        queue<pair<int, int>> que;
        REP (y, N) {
            REP (x, N) {
                if (fixed[y][x]) {
                    path[y][x].emplace_back(y, x);
                    que.emplace(y, x);
                }
            }
        }
        while (not que.empty()) {
            auto [y, x] = que.front();
            que.pop();
            for (int dir : DIRS) {
                int ny = y + DIR_Y[dir];
                int nx = x + DIR_X[dir];
                if (is_on_field(ny, nx) and path[ny][nx].empty()) {
                    path[ny][nx] = path[y][x];
                    path[ny][nx].emplace_back(ny, nx);
                    que.emplace(ny, nx);
                }
            }
        }

        if (target != -1 and a.field[vegetables[target].y][vegetables[target].x] == -1) {
            target = -1;
        }
        if (target == -1) {
            for (int i : order) {
                if (a.machine[vegetables[i].y][vegetables[i].x]) continue;
                target = i;
                break;
            }
        }
        if (target == -1) {
            a.pass();
        }

        vegetable_t trg = vegetables[target];
        if (path[trg.y][trg.x].empty()) {
            auto [y, x] = movable[0];
            if (not a.machine[trg.y][trg.x]) {
                a.move(y, x, trg.y, trg.x);
                movable[0] = {trg.y, trg.x};
                return;
            }
        } else {
            auto p = path[trg.y][trg.x];
            while (p.size() > movable.size()) {
                p.erase(p.begin());
            }
            REP (i, movable.size()) {
                auto [y, x] = movable[i];
                if (count(ALL(p), movable[i])) continue;
                REP (j, p.size()) {
                    auto [ny, nx] = p[j];
                    if (a.machine[ny][nx]) continue;
                    a.move(y, x, ny, nx);
                    movable[i] = p[j];
                    return;
                }
            }
        }

        a.pass();
    };
    while (a.turn < T) {
        go();
    }

    return a.ans;
}

int main() {
    constexpr auto TIME_LIMIT = chrono::milliseconds(2000);
    chrono::high_resolution_clock::time_point clock_begin = chrono::high_resolution_clock::now();
    xor_shift_128 gen;

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
