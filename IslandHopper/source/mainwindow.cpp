#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_dialog.h"
#include <QTime>
#include <QTimer>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent,(Qt::WindowCloseButtonHint)), ui(new Ui::MainWindow), rules(new QDialog(this,(Qt::WindowCloseButtonHint))), scene(new QGraphicsScene), scene1(new QGraphicsScene), scene2(new QGraphicsScene), currGrid_(nullptr),
                                          wallMode_(0), moveMode_(0), breakMode_(0), turnCount_(0)
{
    srand(static_cast<unsigned>(QTime::currentTime().msec()));
    ui->setupUi(this);
    QGraphicsView *gridView = ui->graphicsView;
    QGraphicsView *p1View = ui->graphicsView_2;
    QGraphicsView *p2View = ui->graphicsView_3;

    //Set up the scene that the playing field will be displayed on
    gridView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gridView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gridView->setScene(scene);
    gridView->setSceneRect(0,0,gridView->frameSize().width(),gridView->frameSize().height());
    gridView->setBackgroundBrush(QPixmap(""));
    gridView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gridView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //Set up the scene that Player 1's wall graphics will be on
    p1View->setScene(scene1);
    p1View->setSceneRect(0,0, p1View->frameSize().width(), p1View->frameSize().height());
    p1View->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    p1View->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //Set up the scene that Player 2's wall graphics will be on
    p2View->setScene(scene2);
    p2View->setSceneRect(0,0, p2View->frameSize().width(), p2View->frameSize().height());
    p2View->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    p2View->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //Set the x and y offset of the grid so we can translate from each Cells x position on the canvas to its x position in the grid
    //Cricual for drawing cells at appropriate places on canvas and keeping things even looking
    xOffset_ = (gridView->frameSize().width() - Cell::get_width()*(COLS*2-1))>>1;
    yOffset_ = (gridView->frameSize().height() - Cell::get_width()*(ROWS*2-1))>>1;

    //Populate our graphics array (flyweight design)
    graphics_[0] = new QPixmap(":/icons/graphics/player1.png");
    graphics_[1] = new QPixmap(":/icons/graphics/player2.png");
    graphics_[2] = new QPixmap(":/icons/graphics/island.png");
    graphics_[3] = new QPixmap(":/icons/graphics/hammer2.png");
    graphics_[4] = new QPixmap(":/icons/graphics/wall2.png");
    graphics_[5] = new QPixmap(":/icons/graphics/coin2.png");
    graphics_[6] = new QPixmap(":/icons/graphics/wall.png");
    graphics_[7] = new QPixmap(":/icons/graphics/storm.png");
    currGrid_ = new Grid(xOffset_,yOffset_, graphics_); //Generate new grid, taking into account x and y offset for cell placement on canvas

    //Connect all the cells in the scene with the on_cell_selected slot of the window
    for(short i = 0; i < ROWS; i++)
    {
        for(short j = 0; j < COLS; j++)
        {
            scene->addItem(currGrid_->get_cell(j,i));
            connect(currGrid_->get_cell(j,i), &Cell::cellSelected, this, &MainWindow::on_cell_selected);
        }
    }

    //Add all the wall graphics to each players canvas
    for(short i = 0; i < WALLS; i++)
    {
        scene1->addPixmap(*graphics_[6])->setPos(0,i*Cell::get_width());
        scene2->addPixmap(*graphics_[6])->setPos(0,i*Cell::get_width());
    }
   //Set up the new player objects
    players_[0] = new Player(currGrid_->get_cell(0,ROWS>>1));
    players_[1] = new Player(currGrid_->get_cell(COLS-1,ROWS>>1));


    ui->graphicsView->setBackgroundBrush(QBrush(QPixmap((":/icons/graphics/background.png"))));
    Ui::Dialog other_ui;
    other_ui.setupUi(rules);
}

MainWindow::~MainWindow()
{
    scene->clear();
    scene1->clear();
    scene2->clear();
    delete ui;
    for(short i = 0; i < 2; i++)
    {
        delete players_[i];
    }
    for(short i = 0; i < 8; i++)
    {
        delete graphics_[i];
    }
    delete rules;
}

void MainWindow::wall_turn(Cell *c, bool left)
{
    c->set_type(CellType::Wall);
    std::vector<Cell*> neighbors = currGrid_->get_neighbors(to_relative_x(c), to_relative_y(c)); //Get neighbors of clicked cell
    std::vector<Cell*> filteredNeghbors;
    for(Cell*& i : neighbors) //Get either North & South neighbors OR West & East neighbors depending on left click or right click
    {
        if(left && i->get_y() != c->get_y() && i->get_type() == CellType::Empty)
        {
            filteredNeghbors.push_back(i);
            i->set_type(CellType::Wall);
        }
        else if(!left && i->get_x() != c->get_x() && i->get_type() == CellType::Empty)
        {
            filteredNeghbors.push_back(i);
            i->set_type(CellType::Wall);
        }
    }

    if(!currGrid_->depthFirstSearch(players_[0]->get_curr_cell(), COLS-1, xOffset_, yOffset_)
            || !currGrid_->depthFirstSearch(players_[1]->get_curr_cell(), 0, xOffset_, yOffset_)) //if either player cant make it to the other side because of a wall placement, reset this turns placed walls to empty
    {
        ui->infoLabel->setText("You can not fully block a player with walls");

        c->set_type(CellType::Empty);
        for(Cell*& i : filteredNeghbors)
        {
            i->set_type(CellType::Empty); //reset walls to empty
        }
        wallMode_ = false;
        return;
    }
    players_[turnCount_%2]->decrement_wall_count();

    if(!(turnCount_%2))  //Update the current players wall count canvas
    {
        scene1->clear();
        for(short i = 0; i < players_[0]->get_wall_count(); i++)
        {
            scene1->addPixmap(*graphics_[6])->setPos(0,i*Cell::get_width());
        }
    }
    else
    {
        scene2->clear();
        for(short i = 0; i < players_[1]->get_wall_count(); i++)
        {
           scene2->addPixmap(*graphics_[6])->setPos(0,i*Cell::get_width());
        }
    }


    c->set_graphic(graphics_[4]);
    c->update();
    for(Cell*& i : filteredNeghbors)
    {
        i->set_graphic(graphics_[4]);
        i->update();
    }

    update_text_ui();

    wallMode_ = false;
}

void MainWindow::move_turn(Cell *c)
{
    Player* currPlayer_ = players_[turnCount_ % 2];
    Cell* currPCell = currPlayer_->get_curr_cell();

    //If the selected cell is more than 1 block away
    if( (std::abs(currPCell->get_x() - c->get_x()) + std::abs(currPCell->get_y() - c->get_y())) > Cell::get_width()*2) //Manhattan Distance
    {
        if(!detect_jump(c)) //If the player is not in a valid jump position
        {
            ui->infoLabel->setText("Select move again with an adjacent cell");
            moveMode_ = false;
            return;
        }

    }

    //From here, the move is valid and we need to determine what the cell type the player has landed on is
    if(c->get_type() == CellType::Coin)
    {
        currPlayer_->increment_score(100);
    }
    else if(c->get_type() == CellType::AddWall)
    {
        if(!(turnCount_%2)) //If player 1
        {
            scene1->addPixmap(*graphics_[6])->setPos(0,currPlayer_->get_wall_count()*Cell::get_width()); //Add a tower graphic to the towers canvas
        }
        else
        {
            scene2->addPixmap(*graphics_[6])->setPos(0,currPlayer_->get_wall_count()*Cell::get_width());
        }
        currPlayer_->increment_wall_count();
    }

    c->set_type(currPCell->get_type()); //Update the cell the player is moving to
    c->update();
    c->set_graphic(currPCell->get_graphic());

    currPCell->set_type(CellType::Empty); //Update the cell the player is moving from
    currPCell->set_graphic(graphics_[2]);
    currPCell->update();

    update_text_ui();

    currPlayer_->set_curr_cell(c); //Set the players Cell* to the Cell they moved to

    if(check_win())
    {
        QMessageBox win;
        win.setText("Winner!");
        win.exec();
    }
    moveMode_ = false;
}

void MainWindow::break_turn(Cell* c)
{
    c->set_type(CellType::Empty);
    c->set_graphic(graphics_[2]);
    c->update();
    players_[turnCount_%2]->decrement_score(100);

    update_text_ui();
}

void MainWindow::on_cell_selected(Cell *c, bool left)
{
    qDebug() << "(" << c->get_x() << "," << c->get_y() << ")" << c->to_string().c_str();
    if(c->get_type() == CellType::Player)
        for(Cell*& i : currGrid_->get_neighbors(to_relative_x(c), to_relative_y(c)))
        {
            qDebug() << "Neighbor: (" << i->get_x() << "," << i->get_y() << ")";
        }

    CellType type = c->get_type();
    if(wallMode_ && type == CellType::Empty)
    {
        wall_turn(c, left);
    }
    else if(moveMode_ && (type != CellType::Wall && type != CellType::Storm && type != CellType::Player))
    {
        move_turn(c);
    }
    else if(breakMode_ && (type == CellType::Wall))
    {
        break_turn(c);
    }
}

void MainWindow::on_moveButton_clicked()
{
    moveMode_ = true;
    wallMode_ = false;
    breakMode_ = false;
    ui->infoLabel->setText("Select the tile you would like to move to");
}

void MainWindow::on_wallButton_clicked()
{
    moveMode_ = false;
    breakMode_ = false;
    if(players_[turnCount_%2]->get_wall_count() <= 0) //If the current player has no more walls
    {
        ui->infoLabel->setText("Not enough walls!!");
        wallMode_ = false;
        return;
    }
    wallMode_ = true;
    ui->infoLabel->setText("Select the tile you would like to place a wall on");
}

void MainWindow::on_breakButton_clicked()
{
    moveMode_ = false;
    wallMode_ = false;
    if(players_[turnCount_%2]->get_score() < 100) //If the current player does not have enough coins
    {
        breakMode_ = false;
        ui->infoLabel->setText("Not enough points!!");
        return;
    }
    breakMode_ = true;
    ui->infoLabel->setText("Select the wall you would like to break");
}

bool MainWindow::check_win() const
{
    return(to_relative_x(players_[0]->get_curr_cell()) == COLS-1 || to_relative_x(players_[1]->get_curr_cell()) == 0);
}

bool MainWindow::detect_jump(Cell*& c) const
{
    Cell* p1 = players_[0]->get_curr_cell();
    Cell* p2 = players_[1]->get_curr_cell();
    Cell* nextPlayer = turnCount_%2 ? p1 : p2;

    //if p1 & p2 are next to each other
    if( (std::abs(p1->get_x() - p2->get_x()) + std::abs(p1->get_y() - p2->get_y())) == Cell::get_width()*2)
    {
        for(Cell*& i : currGrid_->get_neighbors(to_relative_x(nextPlayer), to_relative_y(nextPlayer)))
        {
            if(i==c)
            {
                return true; //If the player has clicked one of the next players neighbors , i.e. a jump, then allow it
            }
        }
    }
    qDebug() << "false";
    return false;
}

void MainWindow::update_text_ui()
{
    if(!(turnCount_%2)) //if we are on player 1, update p1's score
    {
        ui->player1Score->setText(std::to_string(players_[0]->get_score()).c_str());
    }
    else
    {
        ui->player2Score->setText(std::to_string(players_[1]->get_score()).c_str());
    }
    turnCount_++;
    std::string s = "Player: " + std::to_string((turnCount_%2)+1);
    ui->playerLabel->setText(QString(s.c_str()));
    ui->infoLabel->setText("");
}

short MainWindow::to_relative_x(Cell*& c) const
{
    return (c->get_x()-xOffset_)/(Cell::get_width()*2);
}

short MainWindow::to_relative_y(Cell*& c) const
{
    return (c->get_y()-yOffset_)/(Cell::get_width()*2);
}

void MainWindow::on_resetButton_clicked()
{
    scene->clear();
    scene1->clear();
    scene2->clear();
    currGrid_ = new Grid(xOffset_, yOffset_, graphics_);
    for(short i = 0; i < ROWS; i++)
    {
        for(short j = 0; j < COLS; j++)
        {
            scene->addItem(currGrid_->get_cell(j,i));
            connect(currGrid_->get_cell(j,i), &Cell::cellSelected, this, &MainWindow::on_cell_selected);
        }
    }
    for(short i = 0; i < WALLS; i++) //New tower graphics
    {
        scene1->addPixmap(*graphics_[6])->setPos(0,i*Cell::get_width());
        scene2->addPixmap(*graphics_[6])->setPos(0,i*Cell::get_width());
    }
    delete players_[0];
    delete players_[1];
    players_[0] = new Player(currGrid_->get_cell(0,ROWS>>1)); //new players
    players_[1] = new Player(currGrid_->get_cell(COLS-1,ROWS>>1));

    update_text_ui();
    turnCount_ = 0;
    ui->playerLabel->setText("Player: 1");
    ui->player2Score->setText("0"); //Depending on which players turn it is, update_text_ui() only updates that players score
    ui->player1Score->setText("0"); //So we need to manually do both when we reset
}


void MainWindow::on_pushButton_clicked()
{
    rules->exec();
}

