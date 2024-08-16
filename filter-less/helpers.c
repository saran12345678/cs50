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

// Convert image to sepia
void sepia(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int red = image[i][j].rgbtRed;
            int grn = image[i][j].rgbtGreen;
            int blu = image[i][j].rgbtBlue;

            // セピアに変換
            int sepiaRed = round(.393 * red + .769 * grn + .189 * blu);
            int sepiaGreen = round(.349 * red + .686 * grn + .168 * blu);
            int sepiaBlue = round(.272 * red + .534 * grn + .131 * blu);
            image[i][j].rgbtRed = sepiaRed > 255 ? 255 : sepiaRed;
            image[i][j].rgbtGreen = sepiaGreen > 255 ? 255 : sepiaGreen;
            image[i][j].rgbtBlue = sepiaBlue > 255 ? 255 : sepiaBlue;
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
