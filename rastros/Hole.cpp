#include "Hole.h"
#include <algorithm>

// A constructor for the hole button
Hole::Hole(QWidget *parent) : QPushButton(parent), m_row(-1), m_col(-1), m_state(Hole::EmptyState), m_marked(false) {
    // Makes a conncetion between functions related to the change of state so they're both called.
    QObject::connect(this, SIGNAL(stateChanged(Hole::State)), this, SLOT(updateHole()));

    // Makes a conncetion between functions related to the green highlighting so they're both called.
    QObject::connect(this, SIGNAL(markedChanged(bool)), this, SLOT(updateHole()));

    // Initializes hole.
    this->updateHole();
}

// A destructor for the hole button.
Hole::~Hole() {
    // Doesn't do anything.
}

// Changes hole state if possible.
void Hole::setState(State state) {
    // Condition is necessary in order to avoid repetitions.
    if (m_state != state) {
        m_state = state;
        // Implemented and handled by Qt
        emit stateChanged(state);
    }
}

// Makes the hole highlightable (or not).
void Hole::setMarked(bool marked) {
    // Condition is necessary in order to avoid repetitions.
    if (m_marked != marked) {
        m_marked = marked;
        // Implemented and handled by Qt
        emit markedChanged(marked);
    }
}

// Brings the hole button to its initial state.
void Hole::reset() {
    this->setEnabled(true);
    m_state = Hole::EmptyState;
    m_marked = false;
    this->updateHole();
}

// Returns icons for hole button (used in other functions within this class).
QPixmap Hole::stateToPixmap(State state) {
    switch (state) {
        case Hole::WhiteState:
            return QPixmap(":/white");
        case Hole::BlackState:
            return QPixmap(":/black");
        case Hole::EmptyState:
            // Doesn't return icon
        default:
            return QPixmap();
    }
}

// Changes the appearance of a hole according to its state.
void Hole::updateHole() {
    // Setup icon.
    this->setIcon(Hole::stateToPixmap(m_state));

    // Makes marked blocks unclickable.
    if(m_state == Hole::BlackState){
        this->setEnabled(false);
     }

    // Force reloading stylesheet.
    this->setStyleSheet(this->styleSheet());

}
