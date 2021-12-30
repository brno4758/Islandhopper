#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "player.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow& get_instance(){ //singleton design pattern
        static MainWindow instance;
        return instance;
    }

    ~MainWindow();

    //Sets the x offset of the window
    //This is used to translate the Cell's x value on the window to the Cell's x position in the Grid
    void set_x_offset(short offset) {xOffset_ = offset;}


    //Sets the y offset of the window
    //This is used to translate the Cell's y value on the window to the Cell's y position in the Grid
    void set_y_offset(short offset) {yOffset_ = offset;}

    //Sets the windows wallMode_ to true and all other modes to false
    void wall_turn(Cell *c, bool left);

    //Sets the windows moveMode_ to true and all other modes to false
    void move_turn(Cell *c);

    //Sets the windows breakMode_ to true and all other modes to false
    //Sets all modes to false if current player does not have enough points to break a wall
    void break_turn(Cell *c);

    //Checks if either player has reached any cell on the opposite edge of their starting position
    bool check_win() const;

    //Updates the ui after a player turn is complete
    //Does not update graphics, only text ui
    void update_text_ui();

    //translates the Cell c's x value to its x position in the grid
    //returns translation
    short to_relative_x(Cell*& c) const;

    //translates the Cell c's y value to its y position in the grid
    //returns translation
    short to_relative_y(Cell*& c) const;

    MainWindow(MainWindow const&) = delete;
    void operator=(MainWindow const&) = delete;

private:
    MainWindow(QWidget *parent = nullptr);
    Ui::MainWindow* ui;
    QDialog* rules;
    QGraphicsScene* scene;
    QGraphicsScene* scene1;
    QGraphicsScene* scene2;
    Grid* currGrid_;
    Player* players_[2];
    QPixmap* graphics_[8];
    bool wallMode_;
    bool moveMode_;
    bool breakMode_;
    short turnCount_;
    short xOffset_;
    short yOffset_;

    //Checks if the current cell selected is in a jump situation
    bool detect_jump(Cell*& c) const;

private slots:
    void on_cell_selected(Cell *c, bool left);
    void on_moveButton_clicked();
    void on_wallButton_clicked();
    void on_breakButton_clicked();
    void on_resetButton_clicked();
    void on_pushButton_clicked();
};
#endif // MAINWINDOW_H
