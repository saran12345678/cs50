#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max voters and candidates
#define MAX_VOTERS 100   // 選挙の候補者の最大数
#define MAX_CANDIDATES 9 // 選挙の有権者の最大数

// preferences[i][j] is jth preference for voter i
int preferences[MAX_VOTERS][MAX_CANDIDATES]; // 投票者iのj番目の投票先を格納 0~candidate_count

// Candidates have name, vote count, eliminated status
typedef struct
{
    string name;     // 選挙者名
    int votes;       // 得票数
    bool eliminated; // 除外フラグ
} candidate;

// Array of candidates
candidate candidates[MAX_CANDIDATES];

// Numbers of voters and candidates
int voter_count;
int candidate_count;

// Function prototypes
bool vote(int voter, int rank, string name);
void tabulate(void);
bool print_winner(void);
int find_min(void);
bool is_tie(int min);
void eliminate(int min);

int main(int argc, string argv[])
{
    // 候補者が１名以下の場合、処理を終了する
    if (argc < 2)
    {
        printf("Usage: runoff [candidate ...]\n");
        return 1;
    }

    // 候補者の最大数を超えている場合、処理を終了する
    candidate_count = argc - 1;
    if (candidate_count > MAX_CANDIDATES)
    {
        printf("Maximum number of candidates is %i\n", MAX_CANDIDATES);
        return 2;
    }

    // 候補者の初期値を設定する
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
        candidates[i].eliminated = false;
    }

    // 投票者数を取得
    voter_count = get_int("Number of voters: ");
    // 投票者数の最大値を超える場合、処理を終了する
    if (voter_count > MAX_VOTERS)
    {
        printf("Maximum number of voters is %i\n", MAX_VOTERS);
        return 3;
    }

    // 投票結果をpreferencesに格納する
    for (int i = 0; i < voter_count; i++)
    {

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            // Record vote, unless it's invalid
            if (!vote(i, j, name))
            {
                printf("Invalid vote.\n");
                return 4;
            }
        }

        printf("\n");
    }

    // Keep holding runoffs until winner exists
    while (true)
    {
        // candidateの得票数を算出する
        tabulate();

        // 過半数の得票がある場合、処理を終了する
        bool won = print_winner();
        if (won)
        {
            break;
        }

        // 最も得票数の少ない候補者を取得する
        int min = find_min();
        // 最小得票数以上の得票数が存在するかチェック
        bool tie = is_tie(min);

        // 全候補者の得票数が同数の場合、当選者として出力する
        if (tie)
        {
            for (int i = 0; i < candidate_count; i++)
            {
                if (!candidates[i].eliminated)
                {
                    printf("%s\n", candidates[i].name);
                }
            }
            break;
        }

        // Eliminate anyone with minimum number of votes
        eliminate(min);

        // 最小得票者の除外フラグをTrueにする
        for (int i = 0; i < candidate_count; i++)
        {
            candidates[i].votes = 0;
        }
    }
    return 0;
}

// preferencesに投票結果を代入する
// 引数：voter 投票者のIndex
// rank 投票の順位
// name 候補者氏名
// 戻り値：投票に不備があった場合、False
bool vote(int voter, int rank, string name)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i].name) == 0)
        {
            preferences[voter][rank] = i;
            return true;
        }
    }
    return false;
}

// candidate.votesを算出する
void tabulate(void)
{
    for (int i = 0; i < voter_count; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            // 除外フラグがTrueではない場合、得票数を+1する
            if (!candidates[preferences[i][j]].eliminated)
            {
                candidates[preferences[i][j]].votes++;
                // インデントしたら次の投票者に移る
                break;
            }
        }
    }
    return;
}

// 過半数の得票があるかチェック
bool print_winner(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        // 割り算は切り捨てのため>=ではなく>で対応 ※9/2の場合4となる
        if (candidates[i].votes > voter_count / 2)
        {
            printf("%s\n", candidates[i].name);
            return true;
        }
    }
    return false;
}

// 最小の得票数を取得する
// 得票数は、candidates[n].votes
int find_min(void)
{
    int min = MAX_VOTERS;
    for (int i = 0; i < candidate_count; i++)
    {
        if (min > candidates[i].votes && !candidates[i].eliminated)
        {
            min = candidates[i].votes;
        }
    }
    return min;
}

// 最小得票数以上の得票数が存在しないかチェック
bool is_tie(int min)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes > min)
        {
            return false;
        }
    }
    return true;
}

// 最小得票数のcandidates[i].eliminatedをtrueにする
void eliminate(int min)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes == min)
        {
            candidates[i].eliminated = true;
        }
    }
    return;
}
