#include "grid.h"

Cell::Cell(const short x, const short y, CellType type, QPixmap* graphic) : x_(x), y_(y), type_(type), graphic_(graphic){}

std::string Cell::to_string() const
{
    switch(type_){
        case(CellType::Wall):
            return "Wall";
        case(CellType::Coin):
            return "Coin";
        case(CellType::Empty):
            return "Empty";
        case(CellType::AddWall):
            return "AddWall";
        case(CellType::Player):
            return "Player";
        case(CellType::Storm):
            return "Storm";
        default:
            return "Error, Cell Type unidentified";
    }
}

QRectF Cell::boundingRect() const
{
    return QRectF(x_, y_, width_, width_);
}

QPainterPath Cell::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void Cell::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    Q_UNUSED(widget);
    painter->drawPixmap(this->x_, this->y_, *graphic_);
}

void Cell::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {
        emit cellSelected(this, true);
    }
    else if(event->buttons() == Qt::RightButton)
    {
        emit cellSelected(this, false);
    }
}
Grid::Grid(short offset_x, short offset_y, QPixmap* graphics[3])
{
    short roll;
    for(short i = 0; i < ROWS; i++) //For each row
    {
        for(short j = 0; j < COLS; j++) //For each column
        {
            roll = rand() % 20; //Roll a random number
            if(roll < 2) //10% to set a coin
            {
                grid_[i][j] = new Cell(j*Cell::get_width()*2+offset_x, i*Cell::get_width()*2+offset_y, CellType::Coin, graphics[5]);
                continue;
            }
            else if(roll == 3) //5% to set a hammer
            {
                grid_[i][j] = new Cell(j*Cell::get_width()*2+offset_x, i*Cell::get_width()*2+offset_y, CellType::AddWall, graphics[3]);
                continue;
            }
            grid_[i][j] = new Cell(j*Cell::get_width()*2+offset_x, i*Cell::get_width()*2+offset_y, CellType::Empty, graphics[2]); //else set to empty island
        }
    }

    grid_[ROWS>>1][0]->set_type(CellType::Player);  //Set player 1 starting position
    grid_[ROWS>>1][0]->set_graphic(graphics[0]);
    grid_[ROWS>>1][COLS-1]->set_type(CellType::Player);  //Set player 2 starting position
    grid_[ROWS>>1][COLS-1]->set_graphic(graphics[1]);

    delete grid_[ROWS>>1][COLS>>1]; //delete the center cell in the grid
    grid_[ROWS>>1][COLS>>1] = new StormCell((COLS>>1)*Cell::get_width()*2+offset_x, (ROWS>>1)*Cell::get_width()*2+offset_y, CellType::Storm, graphics[7]); //create a new StormCell in the center of the grid

    //Note: the above delete and new initialization is done to meet the technical requirement of inheritance and a virtual method
}

Grid::~Grid()
{
    for(short i = 0; i < ROWS; i++)
    {
        for(short j = 0; j < COLS; j++)
        {
            delete grid_[i][j];
        }
    }
}

Cell* Grid::get_cell(int x, int y) const
{
    if(x >= COLS || y >= ROWS || x < 0 || y < 0) //Make sure we dont go out of bounds
    {
        QMessageBox error;
        error.setText("Attempted to get out of bounds cell. Exiting application");
        error.exec();
        exit(1);
    }
    return grid_[y][x];
}

std::vector<Cell*> Grid::get_neighbors(short x, short y) const
{
    std::vector<Cell*> v;
    if(x + 1 < COLS)
        v.push_back(grid_[y][x+1]);
    if(x - 1 >= 0)
        v.push_back(grid_[y][x-1]);
    if(y+1 < ROWS)
        v.push_back(grid_[y+1][x]);
    if(y-1 >= 0)
        v.push_back(grid_[y-1][x]);
    return v;
}

bool Grid::depthFirstHelper(Cell* c, bool** visited, short destX, short xOffset, short yOffset)
{
    short relativeX = (c->get_x()-xOffset)/(Cell::get_width()*2);  //get the cells x and y position in the grid according to its position in the window
    short relativeY = (c->get_y()-yOffset)/(Cell::get_width()*2);

    qDebug() << " I am searching on " << c->get_x() << "," << c->get_y() << "whose cell type is " << c->to_string().c_str();

    if(relativeX == destX) //If we have reached the destination return true
    {
        return true;
    }
    visited[relativeY][relativeX] = true;

    bool flag = false; //Assume there is no path until we find it
    for(Cell*& i : get_neighbors(relativeX,relativeY)) //For each of the current cells neighbors
    {
        relativeX = (i->get_x()-xOffset)/(Cell::get_width()*2); //Get the current neighbors relative x and y
        relativeY = (i->get_y()-yOffset)/(Cell::get_width()*2);
        if(i->get_type() == CellType::Wall || i->get_type() == CellType::Storm || visited[relativeY][relativeX]) //If its a wall or a storm or already visited, do not run DFS on it
            continue;
        flag = depthFirstHelper(i,visited,destX, xOffset, yOffset); //Recursively call DFS helper, setting the flag to the result of each search
        if(flag) //If we found a valid path, stop the search
            break;
    }
    return flag;
}

bool Grid::depthFirstSearch(Cell* c, short destX, short xOffset, short yOffset)
{
    bool** visited = new bool*[ROWS]; //set up an array of booleans to represent the cells in the grid being visited or not
    for(short i = 0; i < ROWS; i++)
        visited[i] = new bool[COLS];
    for(short i = 0; i < ROWS; i++)  //set all to false
        for(short j = 0; j < COLS; j++)
            visited[i][j] = false;

    qDebug() << "I am starting my search at " << c->get_x() << "," << c->get_y();

    bool pathFound =  depthFirstHelper(c,visited,destX, xOffset, yOffset); //Use our dfs helper function, starting at the origin Cell c

    for(short i = 0; i < ROWS; i++) //Free the memory of the visited array
        delete[] visited[i];
    delete[] visited;

    return pathFound;
}
