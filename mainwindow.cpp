#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QObject>
#include <QtWidgets>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mainWindowIcon(":/Resources/icons/mainWindowIcon.jpg"),
    isPlaying(false),
    prevButtonIcon(":/Resources/icons/Button-Prev-icon.png"),
    playButtonPlayIcon(":/Resources/icons/Button-Play-icon.png"),
    playButtonPauseIcon(":/Resources/icons/Button-Pause-icon.png"),
    nextButtonIcon(":/Resources/icons/Button-Next-icon.png"),
    _filename(new QLabel(this)),
    _player(0),
    _playlist(new QMediaPlaylist(this))
{
    ui->setupUi(this);
    setWindowIcon(mainWindowIcon);
    setWindowTitle("Music Player");

    setFixedWidth(8*72);
    setFixedHeight(5*72);

    /****************SETTING UP MENUS*********************/
    QWidget *topFiller = new QWidget;
    topFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

#ifdef Q_OS_SYMBIAN
    infoLabel = new QLabel(tr("<i>Choose a menu option</i>"));
#else
    infoLabel = new QLabel(tr("<i>Choose a menu option, or right-click to "
                              "invoke a context menu</i>"));
#endif
    infoLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    infoLabel->setAlignment(Qt::AlignCenter);

    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(5);
    layout->addWidget(topFiller);
    layout->addWidget(infoLabel);
    layout->addWidget(bottomFiller);

    createActions();
    createMenus();

    /************SETTING UP STATUS BAR********************/
#ifndef Q_OS_SYMBIAN
    QString message = tr("A context menu is available by right-clicking");
    statusBar()->showMessage(message);
#endif

    setupButtons();

    /*************SETTING UP VOLUME SLIDER******************/

    volumeLabel = new QLabel;
    volumeLabel->setText(tr("<b>Volume:</b>"));
    volumeLabel->setParent(this);
    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setParent(this);
    volumeSlider->setMinimum(0);
    volumeSlider->setMaximum(100);
    volumeSlider->setSliderPosition(volumeSlider->maximum());
    volumeSlider->setSingleStep(10);
    volumeSlider->setGeometry
            (
                nextButton.geometry().x(),
                playButton.geometry().y()-20,
                playButton.width(),
                20
            );

    connect(volumeSlider,SIGNAL(valueChanged(int)),this,SLOT(volumeSliderValueChanged()));

    /*************SETTING UP VOLUME LABEL******************/
    volumeLabel->setGeometry
            (
                volumeSlider->geometry().x()-60,
                volumeSlider->geometry().y(),
                60,
                volumeSlider->height()
            );
    volumeLabel->show();
    volumeSlider->show();

    /************SETTING UP FILENAME LABEL*********/
    _filename->setGeometry
            (
                volumeLabel->geometry().x()-120,
                volumeLabel->geometry().y(),
                volumeLabel->geometry().width(),
                volumeLabel->geometry().height()
            );
    _filename->show();

    /******************SOUND CODE******************/
    _player = new QMediaPlayer;
    _player->setVolume(100);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/***********PLAY BUTTON FUNCTION SLOT***********/
void MainWindow::playButtonIsPressed ()
{
    _player->pause();

    if(!isPlaying)
    {
        playButton.setIcon(playButtonPauseIcon);
        _player->play();
    }
    else playButton.setIcon(playButtonPlayIcon);

    isPlaying = !isPlaying;
}

/***********NEXT BUTTON FUNCTION SLOT***********/
void MainWindow::nextButtonIsPressed ()
{
    _playlist->next();
    _filename->setText
            (
                _playlist->media(_playlist->currentIndex()).canonicalUrl().fileName()
            );
}

/***********PREV BUTTON FUNCTION SLOT***********/
void MainWindow::prevButtonIsPressed ()
{
    _playlist->previous();
}

/***********VOLUME SLIDER FUNCTION SLOT***********/
void MainWindow::volumeSliderValueChanged()
{
        _player->setVolume(volumeSlider->value());
}

/**************MENU OPTION SLOTS****************/
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(playAct);
    menu.addAction(nextSongAct);
    menu.addAction(previousSongAct);
    menu.exec(event->globalPos());
}

void MainWindow::play()
{
    playButtonIsPressed();
}

void MainWindow::nextSong()
{
    nextButtonIsPressed();
}

void MainWindow::previousSong()
{
    prevButtonIsPressed();
}

void MainWindow::open()
{
    QFileDialog openFileDialog(this);
    openFileDialog.setNameFilter(tr("Audio (*.mp3 *.mp4 *.wav *.flac *.ogg)"));
    openFileDialog.setViewMode(QFileDialog::List);
    openFileDialog.setFileMode(QFileDialog::ExistingFiles);
    openFileDialog.setDirectory("../cs372-FinalProject/");

    QStringList fileNames;
    if(openFileDialog.exec())
       fileNames = openFileDialog.selectedFiles();

    QList<QMediaContent> playListFiles;

    for(QStringList::iterator file = fileNames.begin(); file < fileNames.end(); file++)
       playListFiles.append(QMediaContent(QUrl::fromLocalFile(*file)));

    _playlist->clear();
    _playlist->addMedia(playListFiles);
    _playlist->setPlaybackMode(QMediaPlaylist::Loop);
    _player->stop();
    _player->setPlaylist(_playlist);
    _player->setPosition(0);

    if(isPlaying) playButtonIsPressed();
}

void MainWindow::about()
{
    infoLabel->setText(tr("Invoked <b>Help|About</b>"));
    QMessageBox::about(this, tr("About Menu"),
            tr("Hit Play to Play Mooxzikz. "
               "Open to Open More Moozikz."));
//TODO: docoomentimgz
}

void MainWindow::aboutQt()
{
    infoLabel->setText(tr("Invoked <b>Help|About Qt</b>"));
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    playAct = new QAction(tr("&Play/Pause"), this);
    playAct->setStatusTip(tr("Play a song"));
    connect(playAct, SIGNAL(triggered()), this, SLOT(play()));

    nextSongAct = new QAction(tr("&Next Song"), this);
    nextSongAct->setStatusTip(tr("Switches to the Next Song"));
    connect(nextSongAct, SIGNAL(triggered()), this, SLOT(nextSong()));

    previousSongAct = new QAction(tr("&Previous Song"), this);
    previousSongAct->setStatusTip(tr("Switches to the Previous Song"));
    connect(previousSongAct, SIGNAL(triggered()), this, SLOT(previousSong()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(aboutQtAct, SIGNAL(triggered()), this, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    playMenu = menuBar()->addMenu(tr("&Play"));
    playMenu->addSeparator();
    playMenu->addAction(playAct);
    playMenu->addAction(nextSongAct);
    playMenu->addAction(previousSongAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::setupButtons()
{
    const int mediaButtonYCoordinate =
            (
                height()- ((width()/5) + statusBar()->geometry().height() - 8)
            );

    //Setup signals
    connect(&prevButton, SIGNAL(clicked()), this, SLOT(prevButtonIsPressed()));
    connect(&playButton, SIGNAL(clicked()), this, SLOT(playButtonIsPressed()));
    connect(&nextButton, SIGNAL(clicked()), this, SLOT(nextButtonIsPressed()));

    //Setup parents
    prevButton.setParent(this);
    playButton.setParent(this);
    nextButton.setParent(this);

    //Setup positions
    prevButton.setGeometry
            (
                0,
                mediaButtonYCoordinate,
                width()/5,
                width()/5
            );
    playButton.setGeometry
            (
                prevButton.width()*2,
                mediaButtonYCoordinate,
                width()/5,
                width()/5
            );
    nextButton.setGeometry
            (
                playButton.width()*4,
                mediaButtonYCoordinate,
                width()/5,
                width()/5
            );

    //Setup icons
    prevButton.setIcon(prevButtonIcon);
    prevButton.setIconSize(QSize(prevButton.height(),prevButton.height()));

    playButton.setIcon(playButtonPlayIcon);
    playButton.setIconSize(QSize(playButton.height(),playButton.height()));

    nextButton.setIcon(nextButtonIcon);
    nextButton.setIconSize(QSize(nextButton.height(),nextButton.height()));

    //Show them off
    prevButton.show();
    playButton.show();
    nextButton.show();
}
