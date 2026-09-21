/*
 * じゃんけんRPG
 * 追加・変更した内容：
 *   - Player クラス（HP・レベル・経験値の管理）
 *   - Enemy クラス（名前・HP・手の選択をカプセル化）
 *   - Boss クラス（Enemy を継承、ポリモーフィズムで特殊行動を実装）
 *   - vector<Enemy*> で敵リストを管理（STL使用）
 *   - じゃんけん判定・表示を独立した関数に分離
 *   - 複数戦・レベルアップ・ボス戦のゲームループを追加
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>   

using namespace std;

char getRandomHand();
string handName(char hand);
string judgeResult(char player, char computer);
void printBattle(char player, char enemy, const string& result);

// Player クラス

class Player {
private:
    int hp;
    int maxHp;
    int level;
    int exp;
    int expToNext;

public:
    Player() : hp(20), maxHp(20), level(1), exp(0), expToNext(3) {}

    int getHp()    const { return hp; }
    int getMaxHp() const { return maxHp; }
    int getLevel() const { return level; }
    bool isAlive() const { return hp > 0; }

    // ダメージを受ける
    void takeDamage(int dmg) {
        hp -= dmg;
        if (hp < 0) hp = 0;
    }

    // 回復機能
    void heal(int amount) {
        hp += amount;
        if (hp > maxHp) hp = maxHp;
    }

    // レベルアップ
    void gainExp(int amount) {
        exp += amount;
        if (exp >= expToNext) {
            exp -= expToNext;
            level++;
            maxHp += 5;
            heal(5);            
            expToNext += 2;     
            cout << "\n★ レベルアップ！ レベル " << level
                 << " になった！最大HP+" << 5 << "，HP回復！" << endl;
        }
    }

    // ステータス表示
    void printStatus() const {
        cout << "【プレイヤー】 Lv." << level
             << "  HP:" << hp << "/" << maxHp
             << "  EXP:" << exp << "/" << expToNext << endl;
    }
};

// Enemy クラス

class Enemy {
protected:
    string name;
    int hp;
    int atk;    
    int expDrop;

public:
    Enemy(const string& n, int h, int a, int e)
        : name(n), hp(h), atk(a), expDrop(e) {}

    virtual ~Enemy() {}

    string getName()  const { return name; }
    int    getHp()    const { return hp; }
    int    getAtk()   const { return atk; }
    int    getExpDrop() const { return expDrop; }
    bool   isAlive()  const { return hp > 0; }

    void takeDamage(int dmg) {
        hp -= dmg;
        if (hp < 0) hp = 0;
    }

    // 敵の手の決定
    virtual char chooseHand() {
        return getRandomHand();
    }

    void printStatus() const {
        cout << "【" << name << "】 HP:" << hp << endl;
    }
};

// Boss クラス（Enemy を継承）
// 特殊行動（60%の確率でパー固定）を実装

class Boss : public Enemy {
private:
    int phase;   // フェーズ管理（HPによる行動変化）

public:
    Boss(const string& n, int h, int a, int e)
        : Enemy(n, h, a, e), phase(1) {}

    // HPが半分以下になるとパーを多用する
    char chooseHand() override {
        if (hp <= 10 && phase == 1) {
            phase = 2;
            cout << "  ！！ " << name << " は激怒した！行動パターンが変わった！" << endl;
        }

        if (phase == 2) {
            // フェーズ2：60%の確率でパー
            int r = rand() % 10;
            if (r < 6) return 'P';
        }
        return getRandomHand();
    }
};


// ランダムな手（G/C/P）を返す
char getRandomHand() {
    int r = rand() % 3;
    if (r == 0) return 'G';
    if (r == 1) return 'C';
    return 'P';
}

// 手の文字コードを日本語名に変換
string handName(char hand) {
    if (hand == 'G') return "グー";
    if (hand == 'C') return "チョキ";
    return "パー";
}

// じゃんけんの勝敗を判定して返す（"W"/"L"/"T"）
string judgeResult(char player, char enemy) {
    if (player == enemy) return "T";
    if ((player == 'G' && enemy == 'C') ||
        (player == 'C' && enemy == 'P') ||
        (player == 'P' && enemy == 'G'))
        return "W";
    return "L";
}

// 対戦結果を表示
void printBattle(char player, char enemy, const string& result) {
    cout << "  あなた：" << handName(player)
         << "  敵：" << handName(enemy) << "  →  ";
    if (result == "W")      cout << "勝ち！" << endl;
    else if (result == "L") cout << "負け…" << endl;
    else                    cout << "あいこ" << endl;
}

// 対戦
void battle(Player& player, Enemy* enemy) {
    cout << "\n--- " << enemy->getName() << " との対戦 ---" << endl;
    enemy->printStatus();

    while (player.isAlive() && enemy->isAlive()) {
        player.printStatus();
        cout << "グー：G，チョキ：C，パー：P を入力（Qで中断）：";

        char input;
        cin >> input;

        if (input == 'Q' || input == 'q') {
            cout << "逃げ出した！" << endl;
            return;
        }

        input = toupper(input);
        if (input != 'G' && input != 'C' && input != 'P') {
            cout << "無効な入力です。G / C / P を入力してください。" << endl;
            continue;
        }

        char enemyHand = enemy->chooseHand();
        string result = judgeResult(input, enemyHand);
        printBattle(input, enemyHand, result);

        if (result == "W") {
            int dmg = 5 + player.getLevel();   
            enemy->takeDamage(dmg);
            cout << "  " << enemy->getName() << " に " << dmg << " ダメージ！" << endl;
        } else if (result == "L") {
            int dmg = enemy->getAtk();
            player.takeDamage(dmg);
            cout << "  プレイヤーが " << dmg << " ダメージを受けた！" << endl;
        } else {
            cout << "  お互いにダメージなし。" << endl;
        }
    }

    if (enemy->isAlive()) {
        cout << "\nやられてしまった…" << endl;
    } else {
        cout << "\n" << enemy->getName() << " を倒した！" << endl;
        player.gainExp(enemy->getExpDrop());
    }
}


int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    cout << "==============================" << endl;
    cout << "      じゃんけんRPG           " << endl;
    cout << "==============================" << endl;
    cout << "じゃんけんで敵を倒しながら" << endl;
    cout << "ボスを目指せ！" << endl << endl;

    Player player;

    // 敵リスト
    vector<Enemy*> enemies;
    enemies.push_back(new Enemy("スライム",    10, 2, 1));
    enemies.push_back(new Enemy("ゴブリン",    15, 3, 2));
    enemies.push_back(new Enemy("オーク",      20, 4, 3));
    enemies.push_back(new Boss ("魔王ジャンケロス", 25, 5, 5));   // Boss（継承）

    // 敵の順番
    for (Enemy* enemy : enemies) {
        if (!player.isAlive()) break;

        cout << "\n\n==============================" << endl;
        battle(player, enemy);

        if (!player.isAlive()) {
            cout << "\nゲームオーバー…旅はここで終わった。" << endl;
            break;
        }
    }

    // ゲームクリア判定
    Enemy* boss = enemies.back();
    if (player.isAlive() && !boss->isAlive()) {
        cout << "\n\n==============================" << endl;
        cout << "★★★ おめでとう！ ★★★" << endl;
        cout << "魔王を倒し，平和を取り戻した！" << endl;
        cout << "最終レベル：" << player.getLevel() << endl;
        cout << "==============================" << endl;
    }

    // メモリ解放
    for (Enemy* e : enemies) {
        delete e;
    }

    return 0;
}
