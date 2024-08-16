#include <cs50.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 512 // ファイル読み込みサイズ
typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    // 引数が２つではない場合、処理を終了する
    if (argc != 2)
    {
        return 1;
    }

    // ファイルを読み込む
    FILE *f = fopen(argv[1], "r");

    // ファイルを読み込めなかった場合、処理を終了する
    if (f == NULL)
    {
        return 1;
    }

    // ファイル名を格納する変数
    char filename[8];

    // ファイル名の採番に使用
    int count = 0;

    // 書き込み用ファイル　While内で使い回す
    FILE *img = NULL;

    // 全てのデータを読み込むまでループする
    // freadの引数
    // 1.格納先
    // 2.読み込むデータの一つのバイト数
    // 3.読み込むデータの個数
    // 4.ファイルポインタ
    BYTE buffer[BLOCK_SIZE + 1]; // 文字列最後の特殊文字(NULL文字)を含む
    while (fread(buffer, 1, BLOCK_SIZE, f) == BLOCK_SIZE)
    {
        // JPEGファイルのヘッダは固定
        // 0xff 0xd8 0xff 0xe0~0xef(16パターン)
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)
        {
            // 最初はcountで分岐していたがimgの方が可読性が高い
            if (img != NULL)
            {
                // ファイルのポインタ(ハンドラ)を閉じる
                fclose(img);
            }

            // fopenを行いファイルポインタを上書きする
            sprintf(filename, "%03i.jpg", count++);
            img = fopen(filename, "w");
        }
        // データ読み込みの際に、最初からJPEGのヘッダが来るわけではないことを考慮
        // ブライアンの動画(2:44)をきちんと見ないと気づけないし、
        // ここのIf文だけでCheck50の複数のテストに失敗するようになる。鬼のようなテストデータ
        if (img != NULL)
        {
            // JPEGファイルの書き込み
            fwrite(buffer, 1, BLOCK_SIZE, img);
        }
    }
    fclose(f);
    fclose(img);
}
