#include <cs50.h>
#include <stdio.h>
#include <string.h>

// 選挙候補者の最大値
#define MAX 9

// 投票者iのj番目がどの候補者か格納する
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// 候補者の勝ち負けを格納
typedef struct
{
    int winner;
    int loser;
} pair;

// 候補者氏名を保持
string candidates[MAX];
// 勝ち負けのペアの配列
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // 候補者が2名未満の場合処理を終了する
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // 候補者数が最大値を超える場合、処理を終了する
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }

    // 候補者の氏名を代入する
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // グラフのロックを初期化する
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // 投票者ごとに投票先のランク付け、
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// 投票先を取得する
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// 投票先の順序
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// 勝ち負けの全てのペアをpairsに代入する
void add_pairs(void)
{
    int p_count = 0;
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            // 同じ投票数の場合、pairsには含めない
            if (preferences[i][j] == preferences[j][i])
            {
                continue;
            }

            if (preferences[i][j] > preferences[j][i])
            {
                pairs[p_count].winner = i;
                pairs[p_count].loser = j;
            }
            else
            {
                pairs[p_count].winner = j;
                pairs[p_count].loser = i;
            }
            p_count++;
        }
    }
    pair_count = p_count;
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // 強さはPREFERENCESで決まる バブルソート
    for (int i = 0; i < pair_count; i++)
    {
        for (int j = pair_count - 1; j >= i; j--)
        {
            int winnerA = pairs[j].winner;
            int loserA = pairs[j].loser;
            int winnerB = pairs[j + 1].winner;
            int loserB = pairs[j + 1].loser;
            if (preferences[winnerA][loserA] < preferences[winnerB][loserB])
            {
                pair tmp = pairs[j];
                pairs[j] = pairs[j + 1];
                pairs[j + 1] = tmp;
            }
        }
    }
    return;
}

// サイクルパターン
// pairsのwinnerとloserを取得
// locked[hoge][winner]としたときに存在しなければ(falseなら)サイクルしていない
// 存在していても(trueでも)最終的にlocked[loser][hoge]とならなければサイクルしていない
// 矢印の先は最終的に起点にたどり着くため、複数の候補者先を考慮する必要はないはず
void lock_pairs(void)
{
    // サイクルしていない場合のみlockedをTrueにする
    for (int i = 0; i < pair_count; i++)
    {
        // サイクルチェック
        bool isCycle = true;
        int winner = pairs[i].winner;
        while (true)
        {
            bool isExist = false;
            for (int j = 0; j < candidate_count; j++)
            {
                if (locked[j][winner] == true)
                {
                    isExist = true;
                    winner = j;
                    break;
                }
            }

            // winnerがloserとなる組み合わせが存在しない場合、矢印の先頭までチェック完了している
            if (!isExist)
            {
                isCycle = false;
                break;
            }

            // Winnerより得票数のある候補者が(最終的に)loserの場合、サイクルしていると言える
            if (winner == pairs[i].loser)
            {
                // isCycleは初期値Trueのため、Falseの代入不要
                break;
            }
        }

        // サイクルしていない場合のみLockedをTrueにする
        if (!isCycle)
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }

    return;
}

// Print the winner of the election
void print_winner(void)
{
    // グラフの起点を探す
    for (int i = 0; i < candidate_count; i++)
    {
        // 起点ではない場合、lockedに必ずTrueが存在する
        // 逆に、起点の場合lockedが全てFalseのはず
        bool isStart = true;
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i] == true)
            {
                isStart = false;
                break;
            }
        }
        if (isStart)
        {
            printf("%s\n", candidates[i]);
            return;
        }
    }
    return;
}
