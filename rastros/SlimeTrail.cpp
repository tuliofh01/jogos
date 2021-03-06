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

    // Moves the white marble forward and then adds a black one to the previous position.
    lastWhite->setState(Hole::BlackState);
    hole->setState(Hole::WhiteState);

    // Updates white marble data.
    lastWhite = hole;
    lastWhitePos[0] = id/8; lastWhitePos[1] = id % 8;

    // Game over (red wins).
    if(id/8 == 7 && id % 8 == 0 ){
        // Displays game result.
        QMessageBox::information(this, tr("Fim de jogo!"), tr("O jogador vermelho venceu!"));
        ui->statusbar->showMessage(tr("FIM DE JOGO!"));
        // Recalculates playable blocks.
        disableAll();
    }
    // Game over (blue wins).
    else if(id/8 == 0 && id % 8 == 7 ){
        // Displays game result.
        QMessageBox::information(this, tr("Fim de jogo!"), tr("O jogador azul venceu!"));
        ui->statusbar->showMessage(tr("FIM DE JOGO!"));
        // Recalculates playable blocks.
        disableAll();
    }
    // Next turn
    else {
        // Recalculates playable blocks.
        enableAdjacents();

        // Emits signal declaring the end of a turn.
        if(playableBlocks > 0){
        emit turnEnded();
        }
    }
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
        }
    }

    // Adds white marble to the starting position on the board.
    m_board[3][4]->setState(Hole::WhiteState);

    // Stores data regarding the position of the white marble.
    lastWhite = m_board[3][4]; lastWhitePos[0] = 3; lastWhitePos[1] = 4; playableBlocks = 8;

    // Calculates playable blocks.
    enableAdjacents();

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

// This function recalculates the playable blocks each turn.
void SlimeTrail::enableAdjacents(){

    // This variable stores the number of playable blocks available (if 0 game ends).
    playableBlocks = 0;

    // These variables store the current position of the white marble.
    int heightTeste = lastWhitePos[0];
    int widthTeste  = lastWhitePos[1];

    for(int i = 0; i < 8; i ++){
        for(int j = 0; j < 8; j ++){
            // Detects white marble position and disables it as playable.
            if (i == heightTeste && j == widthTeste){
                m_board[i][j]->setMarked(false);
                m_board[i][j]->setEnabled(false);
            }
            // Detects all blocks within same x pos as white marble.
            else if ( (i == heightTeste - 1) || (i == heightTeste) || (i == heightTeste + 1)  ){
                // Detects all blocks within same y pos as white marble.
                if ( (j == widthTeste -1 ) || (j == widthTeste) || (j == widthTeste + 1)  ){
                    // Enables directly adjacent blocks.
                    if (m_board[i][j]->state() != Hole::BlackState){
                        m_board[i][j]->setMarked(true);
                        m_board[i][j]->setEnabled(true);
                        // Updates the amount of playable blocks.
                        playableBlocks++;
                    }
                    // Disables non adjacent blocks.
                    else {
                        m_board[i][j]->setMarked(false);
                        m_board[i][j]->setEnabled(false);
                    }
                }
                // Disables non adjacent blocks.
                else {
                    m_board[i][j]->setMarked(false);
                    m_board[i][j]->setEnabled(false);
                }
            }
            // Disables non adjacent blocks.
            else {
                m_board[i][j]->setMarked(false);
                m_board[i][j]->setEnabled(false);
            }
        }
    }

    // Game over (opposite player wins)
    if(playableBlocks == 0){
        if(m_player == SlimeTrail::RedPlayer){
            // Displays game result.
            QMessageBox::information(this, tr("Fim de jogo!"), tr("O jogador azul venceu!"));
            ui->statusbar->showMessage(tr("FIM DE JOGO!"));
            // Recalculates playable blocks.
            disableAll();
        }
        else if(m_player == SlimeTrail::BluePlayer){
            // Displays game result.
            QMessageBox::information(this, tr("Fim de jogo!"), tr("O jogador vermelho venceu!"));
            ui->statusbar->showMessage(tr("FIM DE JOGO!"));
            // Recalculates playable blocks.
            disableAll();
        }
    }
}

// Disables all blocks (used in game over).
void SlimeTrail::disableAll(){
    for(int i = 0; i < 8; i ++){
        for(int j = 0; j < 8; j ++){
            m_board[i][j]->setMarked(false);
            m_board[i][j]->setEnabled(false);
        }
    }
}
