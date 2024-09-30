#include <iostream>
#include <array>
#include <cctype>

/**
 * @brief マップの幅。
 */
size_t constexpr Width = 6;

/**
 * @brief マップの高さ。
 */
size_t constexpr Height = 3;

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

    constexpr Vec2D operator -() const
    {
        return *this * -1;
    }

    constexpr Vec2D operator +(Vec2D const other) const
    {
        return Vec2D{ x + other.x, y + other.y };
    }

    constexpr Vec2D operator -(Vec2D const other) const
    {
        return *this + -other;
    }

    constexpr Vec2D operator *(int const factor) const
    {
        return factor * *this;
    }

    static friend constexpr Vec2D operator *(int const factor, Vec2D const self)
    {
        return Vec2D{ factor * self.x, factor * self.y };
    }

    void operator +=(Vec2D const other)
    {
        *this = *this + other;
    }

    void operator -=(Vec2D const other)
    {
        *this += -other;
    }

    void operator *=(int const factor)
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

constexpr Flag operator ~(Flag const self)
{
    return static_cast<Flag>(~static_cast<int>(self));
}

constexpr Flag operator &(Flag const left, Flag const right)
{
    return static_cast<Flag>(static_cast<int>(left) & static_cast<int>(right));
}

constexpr Flag operator |(Flag const left, Flag const right)
{
    return static_cast<Flag>(static_cast<int>(left) | static_cast<int>(right));
}

static Flag &operator &=(Flag &self, Flag const other)
{
    self = self & other;
    return self;
}

static Flag &operator |=(Flag &self, Flag const other)
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
            throw "Invalid direction";
    }
}

/**
 * @brief マップについての情報をすべて保持するクラス。
 */
class Map
{
public:
    /**
     * @brief マスのフラグへの参照を返す。
     * @param pos 位置。
     * @return フラグへの参照。
     */
    Flag &at(Vec2D const pos)
    {
        return _map[pos.y * Width + pos.x];
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
        moveOnlyPerson(dir);
        // 荷物を n 個動かした場合は、人の荷物フラグを降ろし、n 個先に荷物フラグを立てることと等価
        if (num != 0) {
            at(_person_pos + dirToVec(dir) * num) |= Object;
        }
    }

private:
    /**
     * @brief 人がいる場所。
     */
    Vec2D _person_pos = Vec2D{ 4, 0 };

    /**
     * @brief 各マスのフラグ。
     */
    std::array<Flag, Width *Height> _map = {
        None, Goal,   Goal,   None, Person, None,
        None, Object, Object, None, None,   None,
        None, None,   None,   None, None,   None,
    };

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
        if (next.x < 0 || Width <= next.x || next.y < 0 || Height <= next.y) {
            return -1;
        }

        // 動かす先に荷物がある場合は、そこから更に同じ方向に荷物を動かす必要があるので、
        // 更に動かすとして動かせるかどうか、何個動かすかを再帰的に計算する
        if (at(next) & Object) {
            auto prev = calculateObjectLength(next, dir);
            if (prev < 0) {
                return prev;
            }
            return prev + 1;
        // 荷物が置かれてない場合は人を動かすだけ
        } else {
            return 0;
        }
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

/**
 * @brief ユーザーからの入力を受け取る。
 */
void getInput()
{
    std::cin >> input;
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
    for (auto col = 0; col < Width + 2; col++) {
        cout << "#";
    }
    cout << "\n";

    for (auto row = 0; row < Height; row++) {
        // 左の壁
        cout << "#";
        for (auto col = 0; col < Width; col++) {
            // なにもないところにはスペースを描画
            char c = ' ';
            auto flag = map.at(col, row);
            // 荷物があるところには o を描画
            if (flag & Object) {
                c = 'o';
            // 人がいるところには p を描画
            } else if (flag & Person) {
                c = 'p';
            }
            if (flag & Goal) {
                // 人、ものがゴールにいる場合は大文字で描画
                if (c != ' ') {
                    c = toupper(c);
                // 何も無いゴールには . を描画
                } else {
                    c = '.';
                }
            }
            cout << c;
        }
        // 右の壁
        cout << "#\n";
    }

    // 下の壁
    for (auto col = 0; col < Width + 2; col++) {
        cout << "#";
    }
    cout << endl;
}

int main(int argc, char **argv)
{
    draw();
    while (true) {
        getInput();
        updateGame();
        draw();
    }
}
