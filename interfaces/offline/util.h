#ifndef UTIL_H
#define UTIL_H

#include <qimage.h>
#include "modules/edgeimage.h"

#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X)

// RGB -> YUV
#define RGB2Y(R, G, B) CLIP(( (  66 * (R) + 129 * (G) +  25 * (B) + 128) >> 8) +  16)
#define RGB2U(R, G, B) CLIP(( ( -38 * (R) -  74 * (G) + 112 * (B) + 128) >> 8) + 128)
#define RGB2V(R, G, B) CLIP(( ( 112 * (R) -  94 * (G) -  18 * (B) + 128) >> 8) + 128)

// YUV -> RGB
#define C(Y) ( (Y) - 16  )
#define D(U) ( (U) - 128 )
#define E(V) ( (V) - 128 )

#define YUV2R(Y, U, V) CLIP(( 298 * C(Y)              + 409 * E(V) + 128) >> 8)
#define YUV2G(Y, U, V) CLIP(( 298 * C(Y) - 100 * D(U) - 208 * E(V) + 128) >> 8)
#define YUV2B(Y, U, V) CLIP(( 298 * C(Y) + 516 * D(U)              + 128) >> 8)

// RGB -> YCbCr
#define CRGB2Y(R, G, B) CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16)
#define CRGB2Cb(R, G, B) CLIP((36962 * (B - CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16) ) >> 16) + 128)
#define CRGB2Cr(R, G, B) CLIP((46727 * (R - CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16) ) >> 16) + 128)

// YCbCr -> RGB
#define CYCbCr2R(Y, Cb, Cr) CLIP( Y + ( 91881 * Cr >> 16 ) - 179 )
#define CYCbCr2G(Y, Cb, Cr) CLIP( Y - (( 22544 * Cb + 46793 * Cr ) >> 16) + 135)
#define CYCbCr2B(Y, Cb, Cr) CLIP( Y + (116129 * Cb >> 16 ) - 226 )

namespace MVision
{
    QImage image2qimage(const Image& img)
    {
        QImage result = QImage(img.width(), img.height(), QImage::Format_RGB888);
        for (unsigned int y=0; y<img.height(); ++y)
            for (unsigned int x=0; x<img.width(); ++x)
            {
                Image::Pixel p = img.getPixel(x, y);

                QColor c;

                c.setRed(  CYCbCr2R(p.y, p.cb, p.cr));
                c.setGreen(CYCbCr2G(p.y, p.cb, p.cr));
                c.setBlue( CYCbCr2B(p.y, p.cb, p.cr));

                result.setPixel(x, y, c.rgb());
            }

        return result;
    }

    Image loadImage(const QString& filename)
    {
        QImage qimg = QImage(filename);//.scaled(500, 600, Qt::KeepAspectRatio);
        Image image;
        image.resize(qimg.width(), qimg.height());
        for (unsigned int y=0; y<(unsigned)qimg.height(); ++y)
            for (unsigned int x=0; x<(unsigned)qimg.width(); ++x)
            {
                Image::Pixel p;
                QColor c = QColor(qimg.pixel(x, y));

                p.y  = CRGB2Y(c.red(), c.green(), c.blue());
                p.cr = CRGB2Cr(c.red(), c.green(), c.blue());
                p.cb = CRGB2Cb(c.red(), c.green(), c.blue());

                image.getPixel(x, y) = p;
            }
        return image;
    }
}

#endif // UTIL_H
