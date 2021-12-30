#ifndef GRID_H
#define GRID_H

#include <QColor>
#include <QGraphicsItem>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <QtWidgets>

#define COLS    9
#define ROWS    7
enum class CellType {Wall, Player,  Coin, AddWall, Empty, Storm};

class Cell: public QObject, public QGraphicsItem{
    Q_OBJECT
public:
    //create a new cell with the provided x, y, and type
    Cell(const short x, const short y, CellType type, QPixmap* graphic);

    //get the x_ value of the cell
    short get_x() const { return x_; }

    //get the y_ value of the cell
    short get_y() const { return y_; }

    //get the width of the cell
    static short get_width() { return width_; }

    //get the graphic pointer of the cell
    QPixmap* get_graphic() {return graphic_;}

    //get the CellType of the cell
    virtual CellType get_type() const {return type_;}

    //set the CellType of the cell
    void set_type(CellType type) {type_ = type;}

    //set the graphic pointer of the cell
    void set_graphic(QPixmap* graphic) {graphic_ = graphic;}

    //Return string representation of Cell's type
    std::string to_string() const;

    QRectF boundingRect() const override; //adds clickable area to the object of the ui
    QPainterPath shape() const override; //allows us to draw standard shapes
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override; //allows us to add color to object we are drawing

protected:
    const static short width_ = 50;
    short x_;
    short y_;
    CellType type_;
    QPixmap* graphic_;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void cellSelected(Cell *c, bool left); //singal when cell is clicked

};


class StormCell: public Cell{
public:
    StormCell(const short x, const short y, CellType type, QPixmap* graphic) : Cell(x,y,type,graphic){};

    //returns CellType Storm
    CellType get_type() const {return CellType::Storm;}
};

class Grid
{
public:
    Grid(short offset_x, short offset_y, QPixmap* graphics[3]);
    ~Grid();

    //returns the cell at grid_[y][x]
    Cell* get_cell(const int x, const int y) const;

    //returns the number of rows of the grid
    short get_num_rows() const {return ROWS;}

    //returns the number of cols of the grid
    short get_num_cols() const {return COLS;}

    //return the vector of Cell* containing North, South, East, and West neighbors(if they exist) of the cell at position (x,y)
    std::vector<Cell*> get_neighbors(short x, short y) const;

    //runs a depth first search beginning at Cell c's position.
    //returns true if there is a path available to the destinations x position (destX)
    //returns false if there is no available path to the destinations x position (destX)
    bool depthFirstSearch(Cell* c, short destX, short xOffset, short yOffset);

private:
    Cell* grid_[ROWS][COLS]; //[numRows_][numCols_]

    //Helper function for the depth first search
    //returns true if there is a path available to the destinations x position (destX)
    //returns false if there is no available path to the destinations x position (destX)
    bool depthFirstHelper(Cell* c, bool** visited, short destX, short xOffset, short yOffset);
};
#endif // GRID_H
