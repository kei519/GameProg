#include <cstring>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

#ifdef _DEBUG
#include <cassert>
#define DEBUG_ASSERT(expression) assert(expression)
#else
#define DEBUG_ASSERT(expression) (void)(expression)
#endif

#define UNREACHEABLE(msg)                                                                                              \
    std::cout << (msg) << std::endl;                                                                                   \
    exit(1)

/**
 * @brief 2次元ベクトルを表す。
 */
struct Vec2D
{
    /**
     * @brief x 成分。
     */
    int x;
    /**
     * @brief y 成分。
     */
    int y;

    constexpr Vec2D operator-() const
    {
        return *this * -1;
    }

    constexpr Vec2D operator+(Vec2D const other) const
    {
        return Vec2D{ x + other.x, y + other.y };
    }

    constexpr Vec2D operator-(Vec2D const other) const
    {
        return *this + -other;
    }

    constexpr Vec2D operator*(int const factor) const
    {
        return factor * *this;
    }

    friend constexpr Vec2D operator*(int const factor, Vec2D const self)
    {
        return Vec2D{ factor * self.x, factor * self.y };
    }

    void operator+=(Vec2D const other)
    {
        *this = *this + other;
    }

    void operator-=(Vec2D const other)
    {
        *this += -other;
    }

    void operator*=(int const factor)
    {
        *this = factor * *this;
    }
};

/**
 * @brief 各マスの情報をフラグとして表す。
 */
enum Flag
{
    /**
     * @brief なにもないことを表す。
     */
    None = 0,
    /**
     * @brief 荷物があることを表す。
     */
    Object = 1 << 0,
    /**
     * @brief 人がいることを表す。
     */
    Person = 1 << 1,
    /**
     * @brief ゴールであることを表す。
     */
    Goal = 1 << 2,
};

constexpr Flag operator~(Flag const self)
{
    return static_cast<Flag>(~static_cast<int>(self));
}

constexpr Flag operator&(Flag const left, Flag const right)
{
    return static_cast<Flag>(static_cast<int>(left) & static_cast<int>(right));
}

constexpr Flag operator|(Flag const left, Flag const right)
{
    return static_cast<Flag>(static_cast<int>(left) | static_cast<int>(right));
}

static Flag &operator&=(Flag &self, Flag const other)
{
    self = self & other;
    return self;
}

static Flag &operator|=(Flag &self, Flag const other)
{
    self = self | other;
    return self;
}

/**
 * @brief 方向を表す。
 */
enum Direction
{
    /**
     * @brief 上方向を表す。
     */
    Up,
    /**
     * @brief 下方向を表す。
     */
    Down,
    /**
     * @brief 左方向を表す。
     */
    Left,
    /**
     * @brief 右方向を表す。
     */
    Right,
};

/**
 * @brief `Direction` をそちら方向に1マス分の `Vec2D` に変換する。
 * @param dir 変換される方向。
 * @return 変換されたベクトル。
 */
constexpr Vec2D dirToVec(Direction const dir)
{
    switch (dir) {
        case Up:
            return Vec2D{ 0, -1 };
        case Down:
            return Vec2D{ 0, 1 };
        case Left:
            return Vec2D{ -1, 0 };
        case Right:
            return Vec2D{ 1, 0 };
        default:
            // ここには来ないはず
            UNREACHEABLE("Invalid direction");
    }
}

/**
 * @brief マップについての情報をすべて保持するクラス。
 */
class Map
{
public:
    Map()
    {
    }

    /**
     * @brief `mapString` から生成。
     *
     * @param mapString マップ文字列。
     */
    Map(char const *mapString)
    {
        this->initFromMapString(mapString);
    }

    /**
     * @brief `mapString` を用いて初期化。
     *
     * @param mapString マップ文字列。
     */
    void initFromMapString(char const *mapString)
    {
        auto [width, height] = validateAndGetMapSize(mapString);
        if (isErrorOccurred()) {
            return;
        }
        _width = width;
        _height = height;

        vector<Flag> tmp(width * height);
        _map.reserve(width * height);

        int pos = 0;
        for (; *mapString; mapString++) {
            Flag flag = Flag::None;
            // ここまで来た場合 mapString には改行文字かオブジェクトの文字しか含まれていない
            switch (*mapString) {
                case '\r':
                case '\n':
                    continue;
                case '.':
                    flag |= Flag::Goal;
                    break;

                case 'P':
                    flag |= Flag::Goal;
                    [[fallthrough]];
                case 'p':
                    flag |= Flag::Person;
                    _person_pos.x = pos % width;
                    _person_pos.y = pos / width;
                    break;

                case 'O':
                    flag |= Flag::Goal;
                    [[fallthrough]];
                case 'o':
                    flag |= Flag::Object;
                    break;
            }
            _map.push_back(flag);
            pos++;
        }
    }

    /**
     * @brief マップの幅。
     */
    size_t width()
    {
        return _width;
    }

    /**
     * @brief マップの高さ。
     */
    size_t height()
    {
        return _height;
    }

    /**
     * @brief マスのフラグへの参照を返す。
     * @param pos 位置。
     * @return フラグへの参照。
     */
    Flag &at(Vec2D const pos)
    {
        return _map[pos.y * width() + pos.x];
    }

    /**
     * @brief マスのフラグへの参照を返す。
     * @param x x 座標。
     * @param y y 座標。
     * @return フラグへの参照。
     */
    Flag &at(int const x, int const y)
    {
        return at(Vec2D{ x, y });
    }

    /**
     * @brief 動かせる場合は人を `pos` 方向に動かす。
     * @param dir 方向。
     */
    void move(Direction const dir)
    {
        auto num = calculateObjectLength(_person_pos, dir);
        // 動かせない場合
        if (num < 0) {
            return;
        }
        // 荷物は 0（動かさない）個か1個しか動かせない
        // それ以外の場合はなにも動かない
        switch (num) {
            case 0:
                moveOnlyPerson(dir);
                break;
            case 1:
                moveOnlyPerson(dir);
                at(_person_pos + dirToVec(dir)) |= Object;
                break;
        }
    }

    /**
     * @brief 処理の途中でエラーが起きたかどうか。
     */
    bool isErrorOccurred()
    {
        return _isErrorOccurred;
    }

    /**
     * @brief エラー内容。起きていない場合は空文字列を返す。
     */
    char const *errorMessage()
    {
        if (isErrorOccurred()) {
            DEBUG_ASSERT(_errorMessage);
            return _errorMessage;
        } else {
            return "";
        }
    }

private:
    /**
     * @brief 人がいる場所。
     */
    Vec2D _person_pos = { 0, 0 };

    size_t _width = 0;
    size_t _height = 0;

    bool _isErrorOccurred = false;
    char const *_errorMessage = nullptr;

    /**
     * @brief 各マスのフラグ。
     */
    vector<Flag> _map;

    /**
     * @brief 人を `dir` 方向に1マスだけ動かす。このとき動かす先に元々荷物が置かれていた場合、
     * その荷物フラグを降ろす。
     * @param dir 方向。
     */
    void moveOnlyPerson(Direction const dir)
    {
        at(_person_pos) &= ~Person;
        _person_pos += dirToVec(dir);
        at(_person_pos) &= ~Object;
        at(_person_pos) |= Person;
    }

    /**
     * @brief `pos` から `dir` 方向に移動するときにいくつの荷物を動かすことになるかを返す。
     * ただし、動かせない場合は負の値を返す。
     * @param pos 初期位置。
     * @param dir 方向。
     * @return 移動させる荷物の量。
     */
    int calculateObjectLength(Vec2D const pos, Direction dir)
    {

        auto next = pos + dirToVec(dir);
        // 動かす先が範囲外の場合は動かせない
        if (next.x < 0 || width() <= next.x || next.y < 0 || height() <= next.y) {
            return -1;
        }

        if (at(next) & Object) {
            // 動かす先に荷物がある場合は、そこから更に同じ方向に荷物を動かす必要があるので、
            // 更に動かすとして動かせるかどうか、何個動かすかを再帰的に計算する
            auto prev = calculateObjectLength(next, dir);
            if (prev < 0) {
                return prev;
            }
            return prev + 1;
        } else {
            // 荷物が置かれてない場合は人を動かすだけ
            return 0;
        }
    }

    /**
     * @brief `mapString` が正当か確認し、マップの大きさを返す。
     *
     * マップが正当とは
     *
     *   - 長方形である
     *
     *   - 不正な文字列が含まれていない
     *
     * @param mapString マップの文字列表現。
     * @return マップの大きさ。
     */
    Vec2D validateAndGetMapSize(const char *mapString)
    {
        int width = 0;
        int height = 0;
        int count = 0;
        for (; *mapString; mapString++) {
            // CRLF の場合
            if (*mapString == '\r' && mapString[1] == '\n') {
                // CR を読み飛ばす
                mapString++;
            }

            switch (*mapString) {
                case '\n':
                    if (height == 0) {
                        width = count;
                    } else if (count != width) {
                        _isErrorOccurred = true;
                        _errorMessage = "マップが長方形ではありません";
                        return { 0, 0 };
                    }
                    count = 0;
                    height++;
                    break;
                case ' ':
                case '.':
                case 'p':
                case 'P':
                case 'o':
                case 'O':
                    count++;
                    break;
                default:
                    _isErrorOccurred = true;
                    _errorMessage = "不正な文字が使われています";
                    return { 0, 0 };
            }
        }
        return { width, height };
    }
};

/**
 * @brief プレイヤーからの入力を保持する。
 */
char input;

/**
 * @brief 現在のマップの状況を保持する。
 */
Map map;

bool checkClear()
{
    for (auto x = 0; x < map.width(); x++) {
        for (auto y = 0; y < map.height(); y++) {
            if (map.at(Vec2D{ x, y }) == Flag::Object) {
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief ユーザーからの入力を受け取る。
 */
void getInput()
{
    cin >> input;
}

/**
 * @brief ユーザーからの入力を受けて、現在のゲームの状況を更新する。
 */
void updateGame()
{
    // WASD で人を左右上下に動かす
    Direction dir;
    switch (input) {
        case 'w':
            dir = Up;
            break;
        case 'a':
            dir = Left;
            break;
        case 's':
            dir = Down;
            break;
        case 'd':
            dir = Right;
            break;
        default:
            return;
    }
    map.move(dir);
}

/**
 * @brief 現在のゲームの状況を描画する。
 */
void draw()
{
    using namespace std;
    // 上の壁を描画
    for (auto col = 0; col < map.width() + 2; col++) {
        cout << "#";
    }
    cout << "\n";

    for (auto row = 0; row < map.height(); row++) {
        // 左の壁
        cout << "#";
        for (auto col = 0; col < map.width(); col++) {
            // なにもないところにはスペースを描画
            char c = ' ';
            auto flag = map.at(col, row);
            if (flag & Object) {
                // 荷物があるところには o を描画
                c = 'o';
            } else if (flag & Person) {
                // 人がいるところには p を描画
                c = 'p';
            }
            if (flag & Goal) {
                if (c != ' ') {
                    // 人、ものがゴールにいる場合は大文字で描画
                    c = (char)toupper(c);
                } else {
                    // 何も無いゴールには . を描画
                    c = '.';
                }
            }
            cout << c;
        }
        // 右の壁
        cout << "#\n";
    }

    // 下の壁
    for (auto col = 0; col < map.width() + 2; col++) {
        cout << "#";
    }
    cout << endl;
}

int main()
{
    // ファイルからのマップ読み込み
    char stagePath[] = "assets/stageData.txt";
    ifstream inputFile(stagePath, ifstream::binary);

    inputFile.seekg(0, ifstream::end);
    auto fileSize = static_cast<int>(inputFile.tellg());
    if (fileSize == -1) {
        char msg[256];
        strerror_s(msg, errno);
        cout << stagePath << " を読み込めませんでした (" << msg << ")" << endl;
        return 1;
    }
    char *mapString = new char[fileSize + 1];

    inputFile.seekg(0, ifstream::beg);
    inputFile.read(mapString, fileSize);

    mapString[fileSize] = '\0';
    map.initFromMapString(mapString);

    delete[] mapString;

    if (map.isErrorOccurred()) {
        cout << map.errorMessage() << endl;
        return 1;
    }
    cout << map.errorMessage() << endl;

    draw();
    while (true) {
        if (checkClear()) {
            cout << "clear!" << endl;
            break;
        }

        getInput();
        updateGame();
        draw();
    }
    while (true) {
        this_thread::sleep_for(chrono::hours(100));
    }
}
