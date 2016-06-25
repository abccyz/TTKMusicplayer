#include "musicsongringtonemakerwidget.h"
#include "ui_musicsongringtonemakerwidget.h"
#include "musicbackgroundmanager.h"
#include "musiccutsliderwidget.h"
#include "musicuiobject.h"
#include "musicsongtag.h"
#include "musiccoremplayer.h"
#include "musictime.h"

#include <QFileDialog>
#include <QStyledItemDelegate>

MusicSongRingtoneMaker::MusicSongRingtoneMaker(QWidget *parent)
    : MusicAbstractMoveDialog(parent),
      ui(new Ui::MusicSongRingtoneMaker)
{
    ui->setupUi(this);

    ui->topTitleCloseButton->setIcon(QIcon(":/share/searchclosed"));
    ui->topTitleCloseButton->setStyleSheet(MusicUIObject::MToolButtonStyle03);
    ui->topTitleCloseButton->setCursor(QCursor(Qt::PointingHandCursor));
    ui->topTitleCloseButton->setToolTip(tr("Close"));
    connect(ui->topTitleCloseButton, SIGNAL(clicked()), SLOT(close()));

    ui->addSongButton->setStyleSheet(MusicUIObject::MPushButtonStyle08);
    ui->playSongButton->setStyleSheet(MusicUIObject::MPushButtonStyle08);
    ui->playRingButton->setStyleSheet(MusicUIObject::MPushButtonStyle08);
    ui->saveSongButton->setStyleSheet(MusicUIObject::MPushButtonStyle08);
    ui->formatCombo->setItemDelegate(new QStyledItemDelegate(ui->formatCombo));
    ui->formatCombo->setStyleSheet(MusicUIObject::MComboBoxStyle01 + MusicUIObject::MItemView01);
    ui->formatCombo->view()->setStyleSheet(MusicUIObject::MScrollBarStyle01);
    ui->kbpsCombo->setItemDelegate(new QStyledItemDelegate(ui->kbpsCombo));
    ui->kbpsCombo->setStyleSheet(MusicUIObject::MComboBoxStyle01 + MusicUIObject::MItemView01);
    ui->kbpsCombo->view()->setStyleSheet(MusicUIObject::MScrollBarStyle01);
    ui->hzCombo->setItemDelegate(new QStyledItemDelegate(ui->hzCombo));
    ui->hzCombo->setStyleSheet(MusicUIObject::MComboBoxStyle01 + MusicUIObject::MItemView01);
    ui->hzCombo->view()->setStyleSheet(MusicUIObject::MScrollBarStyle01);
    ui->msCombo->setItemDelegate(new QStyledItemDelegate(ui->msCombo));
    ui->msCombo->setStyleSheet(MusicUIObject::MComboBoxStyle01 + MusicUIObject::MItemView01);
    ui->msCombo->view()->setStyleSheet(MusicUIObject::MScrollBarStyle01);
    initControlParameter();

    m_startPos = 0;
    m_stopPos = 9999;
    m_playRingtone = false;

    ui->playSongButton->setEnabled(false);
    ui->playRingButton->setEnabled(false);
    ui->saveSongButton->setEnabled(false);
    ui->cutSliderWidget->resizeWindow(435, 55);
    m_player = new MusicCoreMPlayer(this);

    connect(ui->addSongButton, SIGNAL(clicked()), SLOT(initInputPath()));
    connect(ui->playSongButton, SIGNAL(clicked()), SLOT(playInputSong()));
    connect(ui->playRingButton, SIGNAL(clicked()), SLOT(playRingtone()));
    connect(ui->saveSongButton, SIGNAL(clicked()), SLOT(initOutputPath()));
    connect(ui->cutSliderWidget, SIGNAL(posChanged(qint64,qint64)), SLOT(posChanged(qint64,qint64)));
    connect(m_player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
    connect(m_player, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));
}

MusicSongRingtoneMaker::~MusicSongRingtoneMaker()
{
    delete m_player;
    delete ui;
}

void MusicSongRingtoneMaker::initInputPath()
{
    QStringList supportedFormat;
    supportedFormat << "mp3" <<"wav" <<"wma" << "ogg" << "flac" << "ac3" << "aac";

    QString filter = "Files (";
    for(int i=0; i<supportedFormat.count(); ++i)
    {
        filter += QString("*.%1 ").arg(supportedFormat[i]);
    }
    filter = filter.trimmed() + ")";

    QString path = QFileDialog::getOpenFileName(this, QString(), "./", filter);
    if(path.isEmpty())
    {
        return;
    }

    m_inputFilePath = path;
    MusicSongTag tag;
    if(tag.readFile(m_inputFilePath))
    {
        QString name = m_inputFilePath.split("/").last();
        ui->songLabelValue->setText(tr("SongName: %1 ( %2, %3, %4)").arg(name)
                .arg(tag.getLengthString()).arg(tag.getSamplingRate()).arg(tag.getBitrate()));
    }
    else
    {
        ui->songLabelValue->setText(tr("Open File Error!"));
        return;
    }

    ui->playSongButton->setEnabled(true);
    ui->playRingButton->setEnabled(true);
    ui->saveSongButton->setEnabled(true);

    m_player->setMedia(MusicCoreMPlayer::MusicCategory, m_inputFilePath);
    playInputSong();

}

void MusicSongRingtoneMaker::initOutputPath()
{

}

void MusicSongRingtoneMaker::playInputSong()
{
    m_playRingtone = false;
    if(m_player->state() == MusicCoreMPlayer::StoppedState ||
       m_player->state() == MusicCoreMPlayer::PausedState)
    {
        ui->playSongButton->setText(tr("Stop"));
    }
    else
    {
        ui->playSongButton->setText(tr("Play"));
    }
    m_player->play();
}

void MusicSongRingtoneMaker::playRingtone()
{
    m_playRingtone = true;
    m_player->setPosition(m_startPos);
}

void MusicSongRingtoneMaker::positionChanged(qint64 position)
{
    if(m_playRingtone && m_stopPos < position)
    {
        m_player->play();
        ui->playSongButton->setText(tr("Play"));
        return;
    }
    ui->cutSliderWidget->setPosition(position);
}

void MusicSongRingtoneMaker::durationChanged(qint64 duration)
{
    ui->cutSliderWidget->setDuration(duration);
}

void MusicSongRingtoneMaker::posChanged(qint64 start, qint64 end)
{
    m_startPos = start;
    m_stopPos = end;
    ui->startTimeLabel->setText( MusicTime::toString(start, MusicTime::All_Sec, "mm:ss:zzz") );
    ui->stopTimeLabel->setText( MusicTime::toString(end, MusicTime::All_Sec, "mm:ss:zzz") );
}

int MusicSongRingtoneMaker::exec()
{
    QPixmap pix(M_BACKGROUND_PTR->getMBackground());
    ui->background->setPixmap(pix.scaled( size() ));
    return MusicAbstractMoveDialog::exec();
}

void MusicSongRingtoneMaker::initControlParameter() const
{
    ui->formatCombo->addItems(QStringList() << "MP3" << "WAV");
    ui->kbpsCombo->addItems(QStringList() << "32" << "48" << "56" << "64" << "80"
                     << "96" << "112" << "128" << "192" << "224" << "256" << "320");
    ui->hzCombo->addItems(QStringList() << "8000" << "12050" << "16000" << "22050"
                            << "24000" << "32000" << "44100" << "48000");
    ui->msCombo->addItems(QStringList() << "Mono" << "Stereo");

    ui->kbpsCombo->setCurrentIndex(7);
    ui->hzCombo->setCurrentIndex(6);
}