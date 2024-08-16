#include "helpers.h"
#include <math.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // 変数sumがBYTE型だとcheck50が通らない
            // BYTEの加算がダメなのか、BYTEをdoubleで除算するのがダメなのか時間ある時に調べる
            int sum = 0;
            sum += image[i][j].rgbtBlue;
            sum += image[i][j].rgbtGreen;
            sum += image[i][j].rgbtRed;
            // 3.0で割ることで小数点以下まで計算される
            // 整数同士の場合、正確な平均値が算出できないため小数点以下まで計算必須
            // 最終的に0.5を足すことで四捨五入を行う →
            // check50が通らない原因がわからなかったのでround関数に変更した(関係なかった)
            // debugが手軽にできないのが辛い(慣れていないだけの可能性が高い)
            int avg = round(sum / 3.0);
            image[i][j].rgbtBlue = (int) avg;
            image[i][j].rgbtGreen = (int) avg;
            image[i][j].rgbtRed = (int) avg;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            RGBTRIPLE tmp = image[i][j];
            image[i][j] = image[i][width - 1 - j];
            image[i][width - 1 - j] = tmp;
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE copy[height][width];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int sumRed = 0;
            int sumGrn = 0;
            int sumBlu = 0;
            double count = 0.0;
            for (int k = -1; k < 2; k++)
            {
                for (int l = -1; l < 2; l++)
                {
                    // 指定するindexがマイナスになっていないか
                    // かつ、height,widthを超えていないか確認している
                    if (i + k >= 0 && j + l >= 0 && i + k < height && j + l < width)
                    {
                        sumRed += image[i + k][j + l].rgbtRed;
                        sumGrn += image[i + k][j + l].rgbtGreen;
                        sumBlu += image[i + k][j + l].rgbtBlue;
                        count++;
                    }
                }
            }
            copy[i][j].rgbtRed = round(sumRed / count);
            copy[i][j].rgbtGreen = round(sumGrn / count);
            copy[i][j].rgbtBlue = round(sumBlu / count);
        }
    }

    // image = copyでは配列は上書きできない
    // 関数も存在するが(memcpy)使用する気にならなかったため、For文で対処
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = copy[i][j];
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // C言語にJavaでいうところのGCはないはず
    // For文内で宣言をすると、メモリを都度確保する気がしたため、全ての変数宣言を外だし
    int gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    RGBTRIPLE copy[height][width];
    int gxR = 0;
    int gxG = 0;
    int gxB = 0;
    int gyR = 0;
    int gyG = 0;
    int gyB = 0;
    int red = 0;
    int grn = 0;
    int blu = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // 初期化
            gxR = 0;
            gxG = 0;
            gxB = 0;
            gyR = 0;
            gyG = 0;
            gyB = 0;
            // エッジの処理を行う
            // 以下のFor文でgxとgyを求める
            // 代入はFor文を抜けてから行う
            for (int k = -1; k < 2; k++)
            {
                for (int l = -1; l < 2; l++)
                {
                    if (i + k < 0 || i + k >= height || j + l < 0 || j + l >= width)
                    {
                        red = 0;
                        grn = 0;
                        blu = 0;
                    }
                    else
                    {
                        red = image[i + k][j + l].rgbtRed;
                        grn = image[i + k][j + l].rgbtGreen;
                        blu = image[i + k][j + l].rgbtBlue;
                    }
                    // gx(gy)が0の場合、計算が無駄になるがループ処理の都合上許容する
                    gxR += red * gx[k + 1][l + 1];
                    gxG += grn * gx[k + 1][l + 1];
                    gxB += blu * gx[k + 1][l + 1];
                    gyR += red * gy[k + 1][l + 1];
                    gyG += grn * gy[k + 1][l + 1];
                    gyB += blu * gy[k + 1][l + 1];
                }
            }
            // あまりやりたくないですが、既存変数を使い回す
            red = round(sqrt(gxR * gxR + gyR * gyR));
            grn = round(sqrt(gxG * gxG + gyG * gyG));
            blu = round(sqrt(gxB * gxB + gyB * gyB));
            // 255を超える値を調整する
            // 2乗しているため、0未満は考慮不要
            red = 255 < red ? 255 : red;
            grn = 255 < grn ? 255 : grn;
            blu = 255 < blu ? 255 : blu;
            copy[i][j].rgbtRed = red;
            copy[i][j].rgbtGreen = grn;
            copy[i][j].rgbtBlue = blu;
        }
    }

    // copyの中身をimageに上書きする
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = copy[i][j];
        }
    }
    return;
}
