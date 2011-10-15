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

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "FontTexture.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Add some size options
    ui->cbxSize->addItem( "1024x1024" );
    ui->cbxSize->addItem( "512x512" );
    ui->cbxSize->addItem( "256x256" );

    // Create the font texture widget
    FontTexture * ft = new FontTexture(this);

    // Make the widget the center of the scroll area
    ui->scaFontTexture->setWidget(ft);

    // Setup the signals
    QObject::connect(ui->btnSetFont, SIGNAL(clicked()), ft, SLOT(changeFont()));
    QObject::connect(ui->btnExport, SIGNAL(clicked()), ft, SLOT(exportBitmapFont()));
    QObject::connect(ui->cbxSize, SIGNAL(currentIndexChanged(QString)), ft, SLOT(changeSize(QString)));
    QObject::connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->actionExport, SIGNAL(triggered()), ft, SLOT(exportBitmapFont()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
