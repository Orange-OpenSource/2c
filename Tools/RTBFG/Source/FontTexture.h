/* Copyright 2011 Wynand Marais. All rights reserved. (Modified FreeBSD Lisence)
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 *  o Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 *    disclaimer.
 *
 *  o Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *    following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the authors and should not be
 * interpreted as representing official policies, either expressed or implied, of any organisation.
 */

/** @author Wynand Marais
  * @date 26 March 2011
  * @version 0.1
  */

#ifndef FONTTEXTURE_H
#define FONTTEXTURE_H

#include <QWidget>

/** This class is used to build the bitmap font texture to be exported to a 32bit TGA with the associated glyph width
  * values to allow the use of variable width fonts.
  */
class FontTexture : public QWidget
{
    Q_OBJECT

    /** The font to use for creating the texure */
    QFont _texFont;
public:
    /** This is the default constructor which initialise the state of font, size, etc
      * @param[in] parent   The parent of the this widget
      */
    FontTexture(QWidget * parent = 0);

    /** This function overwrite the repaint function of the widget, inherited from QWidget.
      * @param[in] event    This specify the region to be repainted
      */
    void paintEvent(QPaintEvent * event);

    /** This function paints the image for the font for the font texture
      * @param[out] img The image to paint the glyphs too
      */
    void buildTexture(QImage & img);

public slots:
    /** This function (slot) is used to display a font dialog so the user can specify what font they want to use  for
      * generating the bitmap font.
      */
    void changeFont();

    /** This function (slot) is invoked when the user change the size of the bitmap font texture which is achieved
      * using combo box on the main window.
      * @param[in] size The string indicating the new size which is the same as the text in the combo box
      */
    void changeSize(QString size);

    /** This function is used to display a save dialog and write the texture to a 32bit TGA with the acompanying binary
      * file indicating the width of the glyphs.
      */
    void exportBitmapFont();

protected:
    struct Letters
    {
        float x1, x2, y1, y2;
    };
    Letters letters[256];
};

#endif // FONTTEXTURE_H
