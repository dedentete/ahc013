#include <bits/stdc++.h>
using namespace std;
using P = pair<int, int>;
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define ALL(v) (v).begin(), (v).end()

constexpr int dx[] = {0, 1, 0, -1}, dy[] = {1, 0, -1, 0};
constexpr double TIMELIMIT = 2.9;
constexpr int M = 100;

int N, K;
vector<vector<int>> c;

struct XorShift {
    unsigned int x, y, z, w, t;

    XorShift(int seed) {
        mt19937 rnd(seed);
        x = rnd();
        y = rnd();
        z = rnd();
        w = rnd();
        t = 1;
    }

    int rand() {
        t = x ^ (x << 11);
        x = y;
        y = z;
        z = w;
        w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
        return w & 0x7fffffff;
    }
} rnd(rand());

struct Timer {
    chrono::system_clock::time_point start, now;

    Timer() {
        start = chrono::system_clock::now();
    }

    double getTime() {
        now = chrono::system_clock::now();
        return chrono::duration<double>(now - start).count();
    }
};

struct UnionFind {
    vector<int> par, siz;

    UnionFind(int n) : par(n), siz(n, 1) {
        iota(par.begin(), par.end(), 0);
    }

    int root(int x) {
        if (par[x] == x)
            return x;
        else
            return par[x] = root(par[x]);
    }

    void unite(int x, int y) {
        x = root(x);
        y = root(y);
        if (x == y) return;
        if (siz[x] < siz[y]) swap(x, y);
        siz[x] += siz[y];
        par[y] = x;
    }

    bool same(int x, int y) {
        return root(x) == root(y);
    }

    int size(int x) {
        return siz[root(x)];
    }
};

struct Point {
    int x, y;
    Point(int x, int y) : x(x), y(y) {}
};

struct Move {
    int x, y, dir;
    Move(int x, int y, int dir) : x(x), y(y), dir(dir) {}

    bool operator<(const Move& move) const {
        return (x != move.x ? x < move.x
                            : (y != move.y ? y < move.y
                                           : (dir != move.dir ? dir < move.dir
                                                              : true)));
    }
};

struct Connect {
    int e, f, g, h;
    Connect(int e, int f, int g, int h) : e(e), f(f), g(g), h(h) {}
};

struct State {
    vector<vector<int>> grid;
    double score;

    State() {}

    P find(int x, int y, int dir) {
        int nx = x + dx[dir], ny = y + dy[dir], dist = 1;
        while (0 <= nx && nx < N && 0 <= ny && ny < N) {
            if (0 < grid[nx][ny]) return P(grid[nx][ny], dist);
            nx += dx[dir];
            ny += dy[dir];
            dist++;
        }
        return P(-1, -1);
    }

    Point connectable(int x, int y, int dir) {
        int nx = x + dx[dir], ny = y + dy[dir];
        while (0 <= nx && nx < N && 0 <= ny && ny < N) {
            if (grid[nx][ny] == grid[x][y])
                return Point(nx, ny);
            else if (grid[nx][ny] != 0)
                return Point(-1, -1);
            nx += dx[dir];
            ny += dy[dir];
        }
        return Point(-1, -1);
    }

    void connect(int x, int y, int dir) {
        int nx = x + dx[dir], ny = y + dy[dir];
        while (0 <= nx && nx < N && 0 <= ny && ny < N) {
            if (grid[nx][ny] == grid[x][y]) return;
            grid[nx][ny] = K + 1;
            nx += dx[dir];
            ny += dy[dir];
        }
        return;
    }

    void init() {
        grid = c;
        score = 0;
        rep(x, N) {
            rep(y, N) {
                rep(dir, 2) {
                    auto a = find(x, y, dir);
                    if (grid[x][y] == a.first)
                        score += 2 - (double)a.second / N;
                }
            }
        }
    }

    bool move(int x, int y, int dir) {
        if (x < 0 || N <= x || y < 0 || N <= y) return false;
        if (grid[x][y] == 0) return false;
        int nx = x + dx[dir], ny = y + dy[dir];
        if (nx < 0 || N <= nx || ny < 0 || N <= ny) return false;
        if (grid[nx][ny] != 0) return false;
        if (dir % 2) {
            auto a = find(x, y, 0), b = find(x, y, 2);
            if (grid[x][y] == a.first) score -= 2 - (double)a.second / N;
            if (grid[x][y] == b.first) score -= 2 - (double)b.second / N;
            if (a.first == b.first)
                score += 2 - (double)(a.second + b.second) / N;
        } else {
            auto a = find(x, y, 1), b = find(x, y, 3);
            if (grid[x][y] == a.first) score -= 2 - (double)a.second / N;
            if (grid[x][y] == b.first) score -= 2 - (double)b.second / N;
            if (a.first == b.first)
                score += 2 - (double)(a.second + b.second) / N;
        }
        swap(grid[x][y], grid[nx][ny]);
        if (dir % 2) {
            auto a = find(nx, ny, 0), b = find(nx, ny, 2);
            if (grid[nx][ny] == a.first) score += 2 - (double)a.second / N;
            if (grid[nx][ny] == b.first) score += 2 - (double)b.second / N;
            if (a.first == b.first)
                score -= 2 - (double)(a.second + b.second) / N;
        } else {
            auto a = find(nx, ny, 1), b = find(nx, ny, 3);
            if (grid[nx][ny] == a.first) score += 2 - (double)a.second / N;
            if (grid[nx][ny] == b.first) score += 2 - (double)b.second / N;
            if (a.first == b.first)
                score -= 2 - (double)(a.second + b.second) / N;
        }
        return true;
    }

    pair<int, vector<Connect>> connects(int possible_count) {
        vector<Connect> res;
        UnionFind UF(N * N);
        while (true) {
            bool fin = true;
            UnionFind uf(N * N);
            rep(x, N) {
                rep(y, N) {
                    if (grid[x][y] == 0 || grid[x][y] > K) continue;
                    if (UF.size(x + N * y) > 1) continue;
                    rep(dir, 2) {
                        Point p = connectable(x, y, dir);
                        if (p.x == -1) continue;
                        uf.unite(x + N * y, p.x + N * p.y);
                        fin = false;
                    }
                }
            }
            if (fin) break;
            vector<pair<int, int>> v;
            rep(x, N) {
                rep(y, N) {
                    if (grid[x][y] == 0 || grid[x][y] > K) continue;
                    v.emplace_back(uf.size(x + N * y), uf.root(x + N * y));
                }
            }
            sort(ALL(v));
            reverse(ALL(v));
            int target = v.front().second;
            rep(x, N) {
                rep(y, N) {
                    if (grid[x][y] == 0 || grid[x][y] > K) continue;
                    if (uf.root(x + N * y) != target) continue;
                    rep(dir, 2) {
                        Point p = connectable(x, y, dir);
                        if (p.x == -1) continue;
                        if (UF.same(x + N * y, p.x + N * p.y)) continue;
                        if (possible_count == 0) continue;
                        connect(x, y, dir);
                        UF.unite(x + N * y, p.x + N * p.y);
                        res.emplace_back(x, y, p.x, p.y);
                        possible_count--;
                    }
                }
            }
            if (possible_count == 0) break;
        }
        set<int> S;
        int score = 0;
        rep(x, N) {
            rep(y, N) {
                if (K < grid[x][y]) grid[x][y] = 0;
                if (grid[x][y] == 0) continue;
                if (S.count(UF.root(x + N * y))) continue;
                int n = UF.size(x + N * y);
                score += n * (n - 1) / 2;
                S.emplace(UF.root(x + N * y));
            }
        }
        return pair<int, vector<Connect>>(score, res);
    }
};

void output(vector<Move> moves, vector<Connect> connects) {
    cout << moves.size() << endl;
    rep(i, moves.size()) {
        cout << moves[i].x << " " << moves[i].y << " "
             << moves[i].x + dx[moves[i].dir] << " "
             << moves[i].y + dy[moves[i].dir] << endl;
    }
    cout << connects.size() << endl;
    rep(i, connects.size()) {
        cout << connects[i].e << " " << connects[i].f << " " << connects[i].g
             << " " << connects[i].h << endl;
    }
}

double dfs(int depth, int x, int y, State& state) {
    if (depth == 1) return 0;
    double res = DBL_MIN / 2;
    for (int nx = max(x - 1, 0); nx <= min(x + 1, N); nx++) {
        for (int ny = max(y - 1, 0); ny <= min(y + 1, N); ny++) {
            rep(dir, 4) {
                if (state.move(nx, ny, dir)) {
                    res = max(res + dfs(depth + 1, nx, ny, state) / 2,
                              state.score);
                    state.move(nx + dx[dir], ny + dy[dir], (dir + 2) % 4);
                }
            }
        }
    }
    return res;
}

void solve() {
    Timer tmr;
    double start_time = tmr.getTime();
    int highest = INT_MIN;
    vector<Move> highest_moves;
    vector<Connect> highest_connects;
    vector<Move> moves;
    State state;
    state.init();
    rep(number_of_moves, M * K) {
        if (tmr.getTime() - start_time > TIMELIMIT) break;
        auto p = state.connects(M * K - number_of_moves);
        // output(moves, p.second);
        if (p.first > highest) {
            highest_moves = moves;
            highest_connects = p.second;
            highest = p.first;
        }
        vector<pair<double, Move>> candidate_moves;
        rep(x, N) {
            rep(y, N) {
                rep(dir, 4) {
                    if (state.move(x, y, dir)) {
                        candidate_moves.emplace_back(pair<double, Move>(
                            state.score + dfs(0, x, y, state) / 2,
                            Move(x, y, dir)));
                        state.move(x + dx[dir], y + dy[dir], (dir + 2) % 4);
                    }
                }
            }
        }
        sort(ALL(candidate_moves));
        reverse(ALL(candidate_moves));
        state.move(candidate_moves.front().second.x,
                   candidate_moves.front().second.y,
                   candidate_moves.front().second.dir);
        moves.emplace_back(candidate_moves.front().second);
    }
    output(highest_moves, highest_connects);
}

void input() {
    cin >> N >> K;
    c.resize(N, vector<int>(N));
    rep(x, N) {
        string s;
        cin >> s;
        rep(y, N) {
            c[x][y] = s[y] - '0';
        }
    }
}

signed main() {
    input();
    solve();
    return 0;
}