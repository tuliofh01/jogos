#include "SlimeTrail.h"
#include "ui_SlimeTrail.h"

#include <QDebug>
#include <QMessageBox>
#include <QActionGroup>
#include <QSignalMapper>

// Main window function.
SlimeTrail::SlimeTrail(QWidget *parent) : QMainWindow(parent), ui(new Ui::SlimeTrail), m_player(SlimeTrail::RedPlayer) {
    // Necessary for Qt initialization.
    ui->setupUi(this);

    // Attaches action to the new game button on the top menu.
    QObject::connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(reset()));

    // Attaches action to the quit game button on the top menu.
    QObject::connect(ui->actionQuit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));

    // Attaches action to the help button on the top menu.
    QObject::connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(showAbout()));

    // Virtual entity, will manage all hole button outputs.
    QSignalMapper* map = new QSignalMapper(this);

    // These two for loops associate the hole buttons to the map entity.
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {

            // Finds hole according to the for loop
            QString holeName = QString("hole%1%2").arg(row).arg(col);
            Hole* hole = this->findChild<Hole*>(holeName);

            // Checks for errors during button query.
            Q_ASSERT(hole != nullptr);
            Q_ASSERT(hole->row() == row && hole->col() == col);

            // Stores hole button instance within the main window object through an array (so we can access then later).
            m_board[row][col] = hole;

            // Calculates an id for the hole button to be used alongside with the map entity.
            int id = row * 8 + col;

            // Associates hole button with its id on the map object.
            map->setMapping(hole, id);

            // Redirects clicked signals from the specific hole to the map output.
            QObject::connect(hole, SIGNAL(clicked(bool)), map, SLOT(map()));
        }
    }

    // Adaptation due to version peculiarities.
    #if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    // Passes the button that was clicked to the play function through the map entity.
    QObject::connect(map, SIGNAL(mapped(int)), this, SLOT(play(int)));
    #else
    // Passes the button that was clicked to the play function through the map entity.
    QObject::connect(map, SIGNAL(mappedInt(int)), this, SLOT(play(int)));
    #endif

    // Swiches players once the turn ends.
    QObject::connect(this, SIGNAL(turnEnded()), this, SLOT(switchPlayer()));

    // Qt initialization calls.
    this->reset();
    this->adjustSize();
    this->setFixedSize(this->size());
}

// Frees memory when application ends.
SlimeTrail::~SlimeTrail() {
    delete ui;
}

// Runs whenever a button is pressed, causing changes on the grid.
void SlimeTrail::play(int id) {
    // Returns a button object given its id.
    Hole* hole = m_board[id / 8][id % 8];

    // Print function (debugging purposes).
    qDebug() << "clicked on: " << hole->objectName();

    // Changes the hole's state if possible.
    hole->setState(Hole::BlackState);

    // Emits signal declaring the end of a turn.
    emit turnEnded();
}

// Gets the opposite player's id.
SlimeTrail::Player otherPlayer(SlimeTrail::Player player) {
    return (player == SlimeTrail::RedPlayer ? SlimeTrail::BluePlayer : SlimeTrail::RedPlayer);
}

// Switches the current player and calls the status bar update function.
void SlimeTrail::switchPlayer() {
    m_player = otherPlayer(m_player);
    this->updateStatusBar();
}

// Resets board.
void SlimeTrail::reset() {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Hole* hole = m_board[row][col];
            hole->reset();

            // FIXME: Only neighboor holes should be marked.
            hole->setMarked(true);
        }
    }

    // Marks the starting position.
    m_board[3][4]->setState(Hole::WhiteState);

    // Resets the current player.
    m_player = SlimeTrail::RedPlayer;

    // Updates the status bar.
    this->updateStatusBar();
}

// Displays information regarding the game's creators.
void SlimeTrail::showAbout() {
    QMessageBox::information(this, tr("Sobre"), tr("Rastros\n\nTulio Horta (tuliofh01@gmail.com) & Henrique Cristofano (henrique.henri25@gmail.com)"));
}

// Status bar update function.
void SlimeTrail::updateStatusBar() {
    QString player(m_player == SlimeTrail::RedPlayer ? "Vermelho" : "Azul");
    ui->statusbar->showMessage(tr("Vez do Jogador %2").arg(player));
}
