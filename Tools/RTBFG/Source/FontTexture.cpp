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

#include <QFontDialog>
#include <QPainter>
#include <QResizeEvent>
#include <QFileDialog>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "FontTexture.h"
#include <QDesktopServices>

FontTexture::FontTexture(QWidget * parent) : QWidget(parent)
{
    _texFont = QFont( QString( "Helvetica Neue" ), 48 );
    changeSize( "1024x1024" );
}

void FontTexture::paintEvent(QPaintEvent *event)
{
    // Let the widget do what it needs too
    QWidget::paintEvent(event);

    // Create a painter to do fancy things
    QPainter painter( this );

    // Make the background dark gray
    //QColor colour = QColor( 128, 128, 128, 128 );
    //painter.fillRect( rect(), colour );
    painter.fillRect( rect(), Qt::gray );
    //painter.fillRect( rect(), Qt::black );

    // Set the pen for the grid
    QPen pen;
    pen.setStyle( Qt::SolidLine );
    pen.setBrush( Qt::white );
    pen.setCapStyle( Qt::RoundCap );
    pen.setJoinStyle( Qt::RoundJoin );
    painter.setPen( pen );

    // Draw the grid
    int step = width()/16;

    for(int i = step; i < width(); i+= step)
    {
        // Draw the vertical line
        painter.drawLine(i, 0, i, height());

        // Draw the horizontal line
        painter.drawLine(0, i, width(), i);
    }

    // Create the glyph render
    QImage img( size(), QImage::Format_ARGB32 );
    buildTexture(img);

    // Render the glyph image
    painter.drawImage( rect(), img, img.rect() );
}

void FontTexture::changeFont()
{
    bool ok;

    // Create a new font dialog and set it  default to our current font
    QFont font = QFontDialog::getFont( &ok, _texFont, this );

    if (ok == true)
    {
        // The user pressed OK
        _texFont = font;
    }

    // Update and repaint the widget
    update();
    repaint();
}

void FontTexture::buildTexture(QImage &img)
{
    // Calculate the step
    int step = img.width()/16;

    // Set the font size in pixels
    //_texFont.setPixelSize(step);

    // Create a new painter to pain to the image
    QPainter painter( &img );
    painter.setFont( _texFont );

    // Tell it to antialiase the fonts and make it look pretty
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setRenderHint( QPainter::TextAntialiasing );

    // Clear the texture to be 100% transparent
    img.fill( 0xaaaaff );

    // Set the pen color to white to draw all the text white
    QPen pen;
    pen.setStyle( Qt::SolidLine );
    pen.setBrush( Qt::white);
    pen.setCapStyle( Qt::RoundCap );
    pen.setJoinStyle( Qt::RoundJoin );

    painter.setPen( pen );

    // Write the characters
    QFontMetrics fontMetrics( _texFont );
    const float widthScale = 1.0f / img.width();
    const float heightScale = 1.0f / img.height();
    const float scaledBlockHeight = step * heightScale;
    int letterIndex = 0;

    for(int y = 0; y < 16; y++)
    {
        for(int x = 0; x < 16; x++)
        {
            // Make the current character
            QChar curChar = QChar( letterIndex );
            if( letterIndex == 0 )
            {
                curChar = QChar( (unsigned char)'£' );
            }
            QString curString = QString( curChar );

            // Paint all the characters
            painter.drawText( x*step, y*step, step, step, Qt::AlignLeft, curString );

            const float scaledX = x * step * widthScale;
            const float scaledY = y * step * heightScale;
            const float letterWidth = fontMetrics.width( curChar );

            Letters &letter = letters[letterIndex];
            letter.x1 = scaledX;
            letter.y1 = scaledY;
            letter.x2 = scaledX + letterWidth * widthScale;
            letter.y2 = scaledY + scaledBlockHeight;
            letterIndex++;
        }
    }
}

void FontTexture::changeSize(QString size)
{
    if( size.compare( "256x256" ) == 0 )
    {
        resize( 256, 256 );
        setMaximumSize( 256, 256 );
        setMinimumSize( 256, 256 );
    }
    else if( size.compare( "512x512" ) == 0 )
    {
        resize( 512, 512 );
        setMaximumSize( 512, 512 );
        setMinimumSize( 512, 512 );
    }
    else if( size.compare( "1024x1024" ) == 0 )
    {
        resize( 1024, 1024 );
        setMaximumSize( 1024, 1024 );
        setMinimumSize( 1024, 1024 );
    }

    // Update and repaint the widget
    update();
    repaint();
}

void FontTexture::exportBitmapFont()
{
    // Get the file name of where to save the TGA
    QString desktopDirectory = QDesktopServices::storageLocation( QDesktopServices::DesktopLocation );
    QString defaultFile;
    {
        QStringList split = _texFont.rawName().split( " " );
        for( int i=0; i<split.length(); ++i )
        {
            defaultFile.append( split[i] );
        }
    }
    QString defaultPath = desktopDirectory + QString( "/" ) + defaultFile;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), defaultPath, tr("PNG (*.png)"));
    fileName.truncate( fileName.length()-4 );

    // Check if the user cancelled
    if(fileName.length() == 0)
        return;

    // Create the image
    QImage img(size(), QImage::Format_ARGB32);
    buildTexture(img);

    // Write the tex image to file
    bool saved = img.save(fileName+".png", "png");
    if( saved )
    {
        QString outData;
        for( int i=0; i<256; ++i )
        {
            Letters &letter = letters[i];
            outData.append( QString::number( letter.x1 ) );
            outData.append( "," );
            outData.append( QString::number( letter.y1 ) );
            outData.append( "," );
            outData.append( QString::number( letter.x2 ) );
            outData.append( "," );
            outData.append( QString::number( letter.y2 ) );
            if( i < 255 )
            {
                outData.append( "\n" );
            }
        }

        QFile file( fileName + ".data" );
        if( file.open( QIODevice::WriteOnly ) )
        {
            file.write( outData.toUtf8() );
            file.close();
        }
    }
}
