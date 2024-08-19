// Implements a dictionary's functionality

#include <cs50.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "dictionary.h"

int wordCount = 0;

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

// TODO: Choose number of buckets in hash table
const unsigned int N = 26;

// Hash table
node *table[N];

// 引数の単語がハッシュテーブルに存在する場合、True 存在しない場合はFalseを返す
bool check(const char *word)
{
    // hashメソッドを実行し、引数の単語のハッシュ値を取得
    int hashNum = hash(word);

    // リンクリストの先頭をカーソルとする
    // このカーソルのnextがNULLになるまで単語が存在するかチェックし続ける
    node *cursor = table[hashNum];

    // リンクリストの終点までループする
    while (cursor != NULL)
    {
        // strcasecmp を使用する　大文字小文字を区別せず比較ができる
        if (strcasecmp(word, cursor->word) == 0)
        {
            return true;
        }

        cursor = cursor->next;
    }

    return false;
}

// 単語の先頭からハッシュ化して、数値を返す
unsigned int hash(const char *word)
{
    // Aは65 大文字に変換して1文字目を65で引くことで、0〜26までが返るようになっている
    // メモ：最終的には2文字、3文字までハッシュ化する必要がありそう→速度を上げるため
    return toupper(word[0]) - 'A';
}

// ディクショナリデータをロードする。ロードできなかった場合、Falseを返す。
bool load(const char *dictionary)
{
    // dicrionaryファイルを開く
    FILE *dic = fopen(dictionary, "r");

    // dictionaryファイルを開けなかった場合、Falseを返す。
    if (dic == NULL)
    {
        return false;
    }

    // dicから単語を読み取る→wordに配列として格納される
    // nodeのデータ構造の仕様を拝借する　何故LENGTH+1なのかは余裕があるときに調査する　多分他のファイルでLENGTHの値が都度更新されている？
    // → dictionary.hでLENGTHが45として宣言されていた
    char readWord[LENGTH + 1];

    while (fscanf(dic, "%s", readWord) != EOF)
    {
        // ノードのメモリを割り当てる
        node *n = malloc(sizeof(node));

        // メモリ割り当てがうまくいかなかった場合、Falseを返す。
        // うまくいっていない場合は、mallocの戻り値がNULLになる。
        if (n == NULL)
        {
            return false;
        }

        // 単語をノードにコピーする
        strcpy(n->word, readWord);

        // ハッシュ関数
        int hashNum = hash(readWord);

        if (table[hashNum] == NULL)
        {
            // リンクリストが新規の場合は、新規ノードの向き先をNULL(末尾)にする
            n->next = NULL;
        }
        else
        {
            // 該当ハッシュに既にリンクリストが作成されていた場合
            // 新規ノードの向き先を既存リンクリストの先頭にする
            n->next = table[hashNum];
        }
        // 該当のリンクリストが新規、既存に関係なく新規ノードは必ず先頭に来るため、If文内で行う必要がない
        // 既存リンクリストのヘッダの向き先を新規ノードにする
        table[hashNum] = n;

        // sizeメソッドにて返す数値をインクリメントする
        wordCount++;
    }

    // ファイルハンドルを閉じる ※NULLチェックは宣言時に行なっているため不要
    fclose(dic);

    return true;
}

// dictionary内に含まれる単語数を返却する
unsigned int size(void)
{
    // loadメソッドでdicrionaryの単語数を管理している
    return wordCount;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    // 前提
    // 先頭から消していく　先頭をいきなり消してしまうと先頭以降のノードに移動できなくなってしまうため(nextが見れなくなる)
    // cursorとは別にtempを設ける
    node *cursor = NULL;
    node *temp = NULL;
    for (int i = 0; i < N; i++)
    {
        cursor = table[i];

        if (cursor == NULL)
        {
            continue;
        }

        while (cursor != NULL)
        {
            temp = cursor;

            cursor = cursor->next;
            free(temp);
        }
    }
    return true;
}
