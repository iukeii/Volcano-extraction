#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cctype>

using namespace std;

const int MAP_SIZE = 10;

enum class CellType {
    Road,
    House,
    SafeZone,
    Volcano,
    Lava,
    Barricade
};

struct Position {
    int x;
    int y;
};

struct Citizen {
    Position pos;
    bool alive = true;
    bool escaped = false;
};

struct Cell {
    CellType type = CellType::Road;
    int citizenIndex = -1;
};

class Game {
private:
    Cell map[MAP_SIZE][MAP_SIZE];
    vector<Citizen> citizens;

    Position volcanoPos;
    int difficulty;
    vector<Position> lavaFront;

public:
    Game()
        : volcanoPos({ MAP_SIZE / 2, MAP_SIZE / 2 }),
        difficulty(2)
    {
        srand((unsigned)time(nullptr));
        resetMap();
    }

    void run() {
        chooseDifficulty();

        bool running = true;
        while (running) {
            draw();

            if (isGameOver()) {
                cout << "\n=== ZAIDIMAS BAIGESI ===\n";
                cout << "Isgelbeta pilieciu: " << countEscaped() << "\n";
                cout << "Zuvo pilieciu:     " << (int)citizens.size() - countEscaped() << "\n";
                cout << "\nPaspausk Enter, kad uzdarytum...\n";
                string tmp;
                getline(cin, tmp);
                break;
            }

            running = handleTurn();
            if (!running) break;

            updateWorld();
        }
    }

private:
    void resetMap() {
        for (int y = 0; y < MAP_SIZE; ++y) {
            for (int x = 0; x < MAP_SIZE; ++x) {
                map[y][x].type = CellType::Road;
                map[y][x].citizenIndex = -1;
            }
        }

        map[volcanoPos.y][volcanoPos.x].type = CellType::Volcano;

        map[0][0].type = CellType::SafeZone;
        map[0][MAP_SIZE - 1].type = CellType::SafeZone;
        map[MAP_SIZE - 1][0].type = CellType::SafeZone;
        map[MAP_SIZE - 1][MAP_SIZE - 1].type = CellType::SafeZone;

        citizens.clear();

        lavaFront.clear();
        lavaFront.push_back(volcanoPos);

        spawnHousesAndCitizens();
    }

    void spawnHousesAndCitizens() {
        int numHouses = 4 + (rand() % 3);
        int created = 0;

        while (created < numHouses) {
            int x = rand() % MAP_SIZE;
            int y = rand() % MAP_SIZE;

            if (map[y][x].type == CellType::Road) {
                map[y][x].type = CellType::House;

                Citizen c;
                c.pos = { x, y };
                citizens.push_back(c);
                map[y][x].citizenIndex = (int)citizens.size() - 1;
                ++created;
            }
        }
    }

    void chooseDifficulty() {
        while (true) {
            system("cls");

            cout << "Pasirink sunkuma:\n";
            cout << "1 - Lengvas (lete lava)\n";
            cout << "2 - Normalus\n";
            cout << "3 - Sunkus (greita lava)\n";
            cout << "Ivesk 1, 2 arba 3 ir spausk Enter: ";

            string line;
            getline(cin, line);

            if (line == "1") { difficulty = 1; break; }
            if (line == "2") { difficulty = 2; break; }
            if (line == "3") { difficulty = 3; break; }
        }
    }

    void draw() {
        system("cls");

        for (int y = 0; y < MAP_SIZE; ++y) {
            for (int x = 0; x < MAP_SIZE; ++x) {
                cout << cellChar(x, y) << ' ';
            }
            cout << " | y=" << y + 1 << "\n";
        }
        cout << "\nValdymas:\n";
        cout << "  B x y  - pastatyti / nuimti barikada (#) \n";
        cout << "  Q      - baigti zaidima\n";
        cout << "  Tuscias Enter - baigti ejima (pasaulis pajuda)\n";

        cout << "\nStatistika:\n";
        cout << "  Pilieciu viso:   " << citizens.size() << "\n";
        cout << "  Isgelbeta:       " << countEscaped() << "\n";
        cout << "  Gyvu (neisgelbeta):" << countAliveNotEscaped() << "\n";
    }

    char cellChar(int x, int y) {
        int idx = map[y][x].citizenIndex;
        if (idx >= 0 && idx < (int)citizens.size()) {
            if (citizens[idx].alive && !citizens[idx].escaped) {
                return 'C';
            }
        }

        switch (map[y][x].type) {
        case CellType::Road:     return '.';
        case CellType::House:    return 'H';
        case CellType::SafeZone: return 'S';
        case CellType::Volcano:  return 'V';
        case CellType::Lava:     return '~';
        case CellType::Barricade:return '#';
        }
        return '?';
    }

    int countEscaped() const {
        int c = 0;
        for (const auto& z : citizens) {
            if (z.escaped) ++c;
        }
        return c;
    }

    int countAliveNotEscaped() const {
        int c = 0;
        for (const auto& z : citizens) {
            if (z.alive && !z.escaped) ++c;
        }
        return c;
    }

    bool handleTurn() {
        int barricadesLeft = 5;

        while (true) {
            cout << "\n\nKomanda siame EJIME (B x y / Q / ENTER):  "
                << "(liko barikadu veiksmu: " << barricadesLeft << ")\n> ";

            string line;
            getline(cin, line);

            if (line.empty()) {
                return true;
            }

            stringstream ss(line);
            char cmd;
            ss >> cmd;
            cmd = (char)tolower((unsigned char)cmd);

            if (cmd == 'q') {
                return false;
            }

            if (cmd == 'b') {
                if (barricadesLeft <= 0) {
                    cout << "Sio ejimo barikadu veiksmu limitas pasiektas (5). "
                        << "Baik ejima su tusciu Enter.\n";
                    continue;
                }
                int x, y;
                if (!(ss >> x >> y)) {
                    cout << "Blogas formatas. Naudok: B 3 4\n";
                    continue;
                }
                toggleBarricadeAt(x, y);
                barricadesLeft--;
                continue;
            }

            cout << "Nezinoma komanda. Naudok B x y, Q arba tuscia Enter.\n";
        }
    }

    void toggleBarricadeAt(int x, int y) {
        if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE) {
            cout << "Koordinates uz ribu! (turi buti 0.." << MAP_SIZE - 1 << ")\n";
            return;
        }

        Cell& c = map[y][x];

        if (c.type == CellType::Lava ||
            c.type == CellType::SafeZone ||
            c.type == CellType::Volcano) {
            cout << "Sioje vietoje barikados deti negalima.\n";
            return;
        }

        if (c.type == CellType::Barricade) {
            c.type = CellType::Road;
            cout << "Barikada nuimta (" << x << "," << y << ").\n";
        }
        else if (c.type == CellType::Road || c.type == CellType::House) {
            c.type = CellType::Barricade;
            cout << "Barikada pastatyta (" << x << "," << y << ").\n";
        }
    }

    void updateWorld() {
        spreadLava();
        moveCitizens();
        checkDeaths();
        checkEscapes();
    }

    bool isGameOver() const {
        for (const auto& z : citizens) {
            if (z.alive && !z.escaped) return false;
        }
        return true;
    }

    void spreadLavaOnce() {
        vector<Position> newFront;

        for (const auto& p : lavaFront) {
            static const int dx[4] = { 1, -1, 0, 0 };
            static const int dy[4] = { 0, 0, 1, -1 };

            for (int i = 0; i < 4; ++i) {
                int nx = p.x + dx[i];
                int ny = p.y + dy[i];

                if (nx < 0 || nx >= MAP_SIZE || ny < 0 || ny >= MAP_SIZE)
                    continue;

                Cell& c = map[ny][nx];

                if (c.type == CellType::Lava ||
                    c.type == CellType::Volcano ||
                    c.type == CellType::Barricade) {
                    continue;
                }

                c.type = CellType::Lava;
                newFront.push_back({ nx, ny });
            }
        }

        for (auto& p : newFront) {
            map[p.y][p.x].type = CellType::Lava;
        }
        lavaFront.insert(lavaFront.end(), newFront.begin(), newFront.end());
    }

    void spreadLava() {
        int steps = difficulty;
        for (int i = 0; i < steps; ++i) {
            spreadLavaOnce();
        }
    }

    bool isWalkableForCitizen(CellType t) const {
        if (t == CellType::Lava || t == CellType::Volcano || t == CellType::Barricade)
            return false;
        return true;
    }

    void moveCitizens() {
        const int INF = 1e9;
        int dist[MAP_SIZE][MAP_SIZE];

        for (int y = 0; y < MAP_SIZE; ++y)
            for (int x = 0; x < MAP_SIZE; ++x)
                dist[y][x] = INF;

        queue<Position> q;

        for (int y = 0; y < MAP_SIZE; ++y) {
            for (int x = 0; x < MAP_SIZE; ++x) {
                if (map[y][x].type == CellType::SafeZone) {
                    dist[y][x] = 0;
                    q.push({ x, y });
                }
            }
        }

        static const int dx[4] = { 1, -1, 0, 0 };
        static const int dy[4] = { 0, 0, 1, -1 };

        while (!q.empty()) {
            Position p = q.front();
            q.pop();

            for (int i = 0; i < 4; ++i) {
                int nx = p.x + dx[i];
                int ny = p.y + dy[i];

                if (nx < 0 || nx >= MAP_SIZE || ny < 0 || ny >= MAP_SIZE)
                    continue;

                if (!isWalkableForCitizen(map[ny][nx].type))
                    continue;

                if (dist[ny][nx] > dist[p.y][p.x] + 1) {
                    dist[ny][nx] = dist[p.y][p.x] + 1;
                    q.push({ nx, ny });
                }
            }
        }

        for (int i = 0; i < (int)citizens.size(); ++i) {
            Citizen& c = citizens[i];
            if (!c.alive || c.escaped) continue;

            int x = c.pos.x;
            int y = c.pos.y;

            int bestDist = dist[y][x];
            int bestX = x;
            int bestY = y;

            for (int k = 0; k < 4; ++k) {
                int nx = x + dx[k];
                int ny = y + dy[k];

                if (nx < 0 || nx >= MAP_SIZE || ny < 0 || ny >= MAP_SIZE)
                    continue;

                if (!isWalkableForCitizen(map[ny][nx].type))
                    continue;

                if (dist[ny][nx] < bestDist) {
                    bestDist = dist[ny][nx];
                    bestX = nx;
                    bestY = ny;
                }
            }

            if (bestX != x || bestY != y) {
                if (map[y][x].citizenIndex == i) {
                    map[y][x].citizenIndex = -1;
                }
                c.pos.x = bestX;
                c.pos.y = bestY;
                map[bestY][bestX].citizenIndex = i;
            }
        }
    }

    void checkDeaths() {
        for (int i = 0; i < (int)citizens.size(); ++i) {
            Citizen& c = citizens[i];
            if (!c.alive || c.escaped) continue;

            CellType t = map[c.pos.y][c.pos.x].type;
            if (t == CellType::Lava || t == CellType::Volcano) {
                c.alive = false;
                map[c.pos.y][c.pos.x].citizenIndex = -1;
            }
        }
    }

    void checkEscapes() {
        for (int i = 0; i < (int)citizens.size(); ++i) {
            Citizen& c = citizens[i];
            if (!c.alive || c.escaped) continue;

            CellType t = map[c.pos.y][c.pos.x].type;
            if (t == CellType::SafeZone) {
                c.escaped = true;
                map[c.pos.y][c.pos.x].citizenIndex = -1;
            }
        }
    }
};

int main() {
    while (true) {
        Game game;
        game.run();

        cout << "\nRestart game? (y/n): ";
        string ans;
        getline(cin, ans);

        if (ans != "y" && ans != "Y")
            break;
    }
}
