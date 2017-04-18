//
//  jingo.c
//  Jingo
//
//  Created by Cemsina on 17.03.2017.
//  Copyright © 2017 Cemsina. All rights reserved.
//

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_video.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#define EDGE_SIZE 8

#pragma region Type Definitions
typedef enum {
    Red, Green, Blue, Yellow, Purple, Grey, Pink, Orange,Empty
} Color;
typedef struct {
    int x;
    int y;
} Position;
typedef enum TypeEnum {
    INT, POSITION, COLOR, LIST
} TYPE;
typedef struct {
    void * ArrayPointer;
    unsigned int Length;
    TYPE Type;
} List;
typedef struct {
    float x;
    float y;
} Location;
typedef enum{
None,Vertical,Horizontal
} Direction;
typedef struct{
    bool isActive;
    Location start;
    Direction direction;
    ALLEGRO_MOUSE_STATE state;
} Mouse;
#pragma endregion
#pragma region Globals
Color Table[EDGE_SIZE][EDGE_SIZE];
ALLEGRO_DISPLAY * display;
ALLEGRO_FONT * font;
float Unit;
float Margin;
Location GameTableStart;
float GameTableLength;
ALLEGRO_COLOR TableBackgroundColor;
Mouse mouse;
bool isGameActive;
#pragma endregion
#pragma region Game Functions
Position NewPosition(int x,int y) {
    Position p; p.x = x; p.y = y;return p;
}
List NewList(TYPE type) {
    List * list = (List *)malloc(sizeof(List));
    (*list).Length = 0;
    (*list).Type = type;
    return *list;
}
void Add(List * _ref_list,void * item) {
    switch ((*_ref_list).Type) {
        case INT:
            if ((*_ref_list).Length == 0) (*_ref_list).ArrayPointer = (int *)malloc(sizeof(int));
            else (*_ref_list).ArrayPointer = realloc((*_ref_list).ArrayPointer, sizeof(int)*((*_ref_list).Length + 1));
            *(((int *)(*_ref_list).ArrayPointer)+ (*_ref_list).Length) = *(int*)item;
            break;
        case POSITION:
            if ((*_ref_list).Length == 0) (*_ref_list).ArrayPointer = (Position *)malloc(sizeof(Position));
            else (*_ref_list).ArrayPointer = realloc((*_ref_list).ArrayPointer, sizeof(Position)*((*_ref_list).Length + 1));
            *(((Position *)(*_ref_list).ArrayPointer) + (*_ref_list).Length) = *(Position*)item;
            break;
        case COLOR:
            if ((*_ref_list).Length == 0) (*_ref_list).ArrayPointer = (Color *)malloc(sizeof(Color));
            else (*_ref_list).ArrayPointer = realloc((*_ref_list).ArrayPointer, sizeof(Color)*((*_ref_list).Length+1));
            *(((Color *)(*_ref_list).ArrayPointer) + (*_ref_list).Length) = *(Color*)item;
            break;
        case LIST:
            if ((*_ref_list).Length == 0) (*_ref_list).ArrayPointer = (List *)malloc(sizeof(List));
            else (*_ref_list).ArrayPointer = realloc((*_ref_list).ArrayPointer, sizeof(List)*((*_ref_list).Length + 1));
            *(((List *)(*_ref_list).ArrayPointer) + (*_ref_list).Length) = *(List*)item;
            break;
    }
    (*_ref_list).Length++;
}
ALLEGRO_COLOR getcolor(Color color) {
    switch (color) {
        case Red:
            return al_map_rgb(255, 0, 0);
            break;
        case Green:
            return al_map_rgb(0, 255, 0);
            break;
        case Blue:
            return al_map_rgb(0, 0, 255);
            break;
        case Yellow:
            return al_map_rgb(255, 255, 0);
            break;
        case Purple:
            return al_map_rgb(128, 0, 128);
            break;
        case Grey:
            return al_map_rgb(128, 128, 128);
            break;
        case Pink:
            return al_map_rgb(255, 105, 180);
            break;
        case Orange:
            return al_map_rgb(255, 165, 0);
            break;
        case Empty:
            return TableBackgroundColor;
        default:
            return al_map_rgb(0, 0, 0);
            break;
    }
}
List GetRow(int rowno) {
    List colorlist = NewList(COLOR);
    for (int i = 0; i < EDGE_SIZE; i++) {
        Add(&colorlist, &Table[rowno][i]);
    }
    return colorlist;
}
List GetColumn(int colno) {
    List colorlist = NewList(COLOR);
    for (int i = 0; i < EDGE_SIZE; i++) {
        Add(&colorlist, &Table[i][colno]);
    }
    return colorlist;
}
void Exit() {
    al_destroy_display(display);
}
void Start() {
    srand(time(NULL));
    al_init();
    al_init_primitives_addon();
    ALLEGRO_MONITOR_INFO monitor;
    al_get_monitor_info(0, &monitor);
    int width = monitor.x2 - monitor.x1 + 1;
    int height = monitor.y2 - monitor.y1 + 1;
    GameTableLength = (width < height) ? width : height;
    Unit = GameTableLength/(float)EDGE_SIZE;
    Margin = 0.01;
    GameTableStart.x = (width-GameTableLength)/2;
    GameTableStart.y = (height - GameTableLength) / 2;
    TableBackgroundColor = al_map_rgb(111, 0, 0);
    display = al_create_display(width, height);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_set_window_title(display, "Jingo Game");
    al_init_ttf_addon();
    al_init_font_addon();
    al_install_keyboard();
    al_install_mouse();
    
    /*
     PATH DE SORUN VAR.
     ALLEGRO_PATH * path = al_get_standard_path("C:\Users\cemsi\Documents\Jingo\Jingo\Pacifico.ttf");
     font = al_load_font(path, 20, 0);
     */
}
void CreateNodes() {
    for (int x = 0, y = 0; x<EDGE_SIZE, y<EDGE_SIZE; x++) {
        Table[y][x] = rand() % EDGE_SIZE;
        if (x == EDGE_SIZE-1) {x = -1; y++;}
    }
}
void GetNodeLocation(Position pos,Location * p1,Location * p2){
    p1->x = pos.x*Unit + Unit*Margin + GameTableStart.x;
    p1->y = pos.y*Unit + Unit*Margin +GameTableStart.y;
    p2->x = (pos.x+1)*Unit  - Unit*Margin + GameTableStart.x;
    p2->y = (pos.y+1)*Unit - Unit*Margin + GameTableStart.y;
}
void DrawNode(Position pos,Color color) {
    if(color == Empty) return;
    Location p1;
    Location p2;
    GetNodeLocation(pos, &p1, &p2);
    al_draw_filled_rectangle(p1.x, p1.y, p2.x, p2.y, getcolor(color));
}
void DrawNodes() {
    for (int x = 0, y = 0; x<EDGE_SIZE, y<EDGE_SIZE; x++) {
        Position pos;
        pos.x = x; pos.y = y;
        DrawNode(pos, Table[y][x]);
        if (x == EDGE_SIZE - 1) { x = -1; y++; }
    }
}
void DrawTableBackground() {
    al_draw_filled_rectangle(GameTableStart.x - Unit, GameTableStart.y - Unit, GameTableStart.x + Unit * 9, GameTableStart.y + Unit * 9, TableBackgroundColor);
}
void DrawTable() {
    DrawTableBackground();
    DrawNodes();
}
void DrawMenuItem(char * s,Position pos) {
    al_draw_text(font, getcolor(Red), pos.x, pos.y, 0, s);
}
void DrawMenu() {
    DrawTableBackground();
    DrawMenuItem("Deneme", NewPosition(100, 100));
    
}
Color GetPositionColor(Position pos) {
    return Table[pos.y][pos.x];
}
List GetNodesByColor(Color color) {
    List list = NewList(POSITION);
    for (int x = 0, y = 0; x<EDGE_SIZE, y<EDGE_SIZE; x++) {
        if (Table[y][x] == color) {
            Position pos;
            pos.x = x; pos.y = y;
            Add(&list, &pos);
        }
        if (x == EDGE_SIZE - 1) { x = -1; y++; }
    }
    return list;
}
List CategoryNodesByColor() {
    List ListOfListOfPositions = NewList(LIST);
    for (int i = 0; i < EDGE_SIZE; i++) {
        List newlist = NewList(POSITION);
        Add(&ListOfListOfPositions, &newlist);
    }
    for (int x = 0, y = 0; x < EDGE_SIZE, y < EDGE_SIZE; x++) {
        Position pos;
        pos.x = x; pos.y = y;
        Add((List*)ListOfListOfPositions.ArrayPointer + Table[y][x], &pos);
        if (x == EDGE_SIZE - 1) { x = -1; y++; }
    }
    return ListOfListOfPositions;
}
void * Get(List list,unsigned int i) {
    if (list.Length < i + 1) return NULL;
    switch (list.Type) {
        case INT:
            return (void *)((int *) list.ArrayPointer + i);
        case POSITION:
            return (void *)((Position *)list.ArrayPointer + i);
        case COLOR:
            return (void *)((Color *)list.ArrayPointer + i);
        case LIST:
            return (void *)((List *)list.ArrayPointer + i);
    }
    return NULL;
}
int isPositionExists(Position pos, List positionList) {
    for (int i = 0; i < positionList.Length; i++) {
        if (pos.x == (*((Position *)positionList.ArrayPointer + i)).x && pos.y == (*((Position *)positionList.ArrayPointer + i)).y) return 1;
    }
    return 0;
}

List SearchForExplode() {
    List category = CategoryNodesByColor();
    List explodes = NewList(LIST);
    for (int i = 0; i < category.Length; i++) {
        List ColorPositionList = *((List*)category.ArrayPointer + i);
        if (ColorPositionList.Length == 0) continue;
        List ExplodeListOfPositionList = NewList(LIST);
        
        int isChanged = 1;
        int current = 0;
        while (isChanged) {
            List ExplodePositionList = NewList(POSITION);
            isChanged = 0;
            for (int j= current; j < ColorPositionList.Length; j++) {
                Position pos = *((Position *)ColorPositionList.ArrayPointer + j);
                int used = 0;
                for (int l = 0; l < ExplodeListOfPositionList.Length; l++) {
                    List posList = *(List*)Get(ExplodeListOfPositionList, l);
                    if (isPositionExists(pos, posList)) {
                        used = 1; break;
                    }
                }
                if (isPositionExists(pos, ExplodePositionList)) used = 1;
                if (used) continue;
                if (ExplodePositionList.Length == 0) { Add(&ExplodePositionList, &pos); continue; }
                for (int g = 0; g < ExplodePositionList.Length; g++) {
                    Position pos2 = *((Position *)ExplodePositionList.ArrayPointer + g);
                    int diffX = abs(pos.x - pos2.x);
                    int diffY = abs(pos.y - pos2.y);
                    if (diffX + diffY == 1) {
                        Add(&ExplodePositionList, &pos);
                        j = -1;
                    }
                }
            }
            if (ExplodePositionList.Length > 2) {
                Add(&ExplodeListOfPositionList, &ExplodePositionList);
                isChanged = 1;
            }
            else if(current<ColorPositionList.Length){
                current++;
                isChanged = 1;
            }
        }
        Add(&explodes, &ExplodeListOfPositionList);	
    }
    return explodes;
}
void StartDrawing(){
    DrawTable();
}
void EndDrawing(){
    al_flip_display();
}
void FillEmpty(List positionList){
    for(int i=0;i<positionList.Length;i++){
        Position pos = *(Position *)Get(positionList,i);
        Table[pos.y][pos.x] = Empty;
    }
}
void FallColumn(int colNo){
    // Optimize Edilecek
    List colList = GetColumn(colNo);
    bool isFalling = true;
    List fallsList = NewList(POSITION);
    while(isFalling){
        isFalling = false;
        for(int i=1;i<colList.Length;i++){
            Color color = *(Color *)Get(colList, i);
            if(color == Empty && Table[i-1][colNo] != Empty){
                for(int j=i;j>=0;j--){
                    Table[j][colNo] = Table[j-1][colNo];
                }
                colList = GetColumn(colNo);
                Position pos; pos.x = colNo; pos.y = fallsList.Length;
                Add(&fallsList, &pos);
                isFalling = true;
                break;
            }
        }
    }
    FillEmpty(fallsList);
}
void Fall(){
    for(int i=0;i<EDGE_SIZE;i++){
        FallColumn(i);
    }
}
void FillEmptyNodes(){
    for (int x = 0, y = 0; x<EDGE_SIZE, y<EDGE_SIZE; x++) {
        if(Table[y][x] == Empty) Table[y][x] = rand() % EDGE_SIZE;
        if (x == EDGE_SIZE-1) {x = -1; y++;}
    }
}
int Explode(){
    int exploded = 0;
    List exp;
    bool isExploding = true;
    do {
        isExploding = false;
        exp = SearchForExplode();
        for (int i = 0; i < exp.Length; i++) {
            List colorposlist = *(List *) Get(exp, i);
            for(int j=0;j<colorposlist.Length;j++){
                List poslist = *(List *)Get(colorposlist, j);
                if(poslist.Length > 0){
                    isExploding = true;
                    exploded += poslist.Length;
                    FillEmpty(poslist);
                }
            }
        }
        Fall();
        FillEmptyNodes();
    } while (isExploding);
    return exploded;
}
void NewGame(){
    mouse.direction = None;
    mouse.isActive = false;
    mouse.start.x = -1;
    mouse.start.y = -1;
    CreateNodes();
    Explode();
    isGameActive = true;
    StartDrawing();
    EndDrawing();
}
void CloneTable(Color (*from)[EDGE_SIZE],Color (*to)[EDGE_SIZE]){
    for(int i=0;i<EDGE_SIZE;i++){
        for(int j=0;j<EDGE_SIZE;j++){
            to[i][j] = from[i][j];
        }
    }
}
void ModShift(float * shift){
    float k = *shift / GameTableLength;
    if(k > 0){
        *shift -= floor(k)*GameTableLength;
    }else if(k<0){
        *shift += (floor(-k)+1)*GameTableLength;
    }
}
void MoveRow(int rowno,float shift){
    List colorList = GetRow(rowno);
    Location start;
    Location end;
    start.x = GameTableStart.x;
    start.y = rowno*Unit + GameTableStart.y;
    end.x = Unit*EDGE_SIZE + GameTableStart.x;
    end.y = (rowno+1)*Unit + GameTableStart.y;
    al_draw_filled_rectangle(start.x, start.y, end.x, end.y, TableBackgroundColor);
    start.x += Unit*Margin;
    end.x -= Unit*Margin;
    for(int i=0;i<colorList.Length;i++){
        Color color = *(Color *)Get(colorList, i);
        Location p1;
        Location p2;
        Position pos;
        pos.x = i;
        pos.y = rowno;
        GetNodeLocation(pos, &p1, &p2);
        ModShift(&shift);
        p1.x += shift;
        p2.x += shift;
        if(p1.x > end.x){
            p1.x -= GameTableLength;
            p2.x -= GameTableLength;
        }
        if(p2.x > end.x){
            float remaining = p2.x - end.x;
            al_draw_filled_rectangle(p1.x, p1.y, end.x, p2.y, getcolor(color));
            al_draw_filled_rectangle(start.x, p1.y, start.x+remaining-Unit*Margin*2, p2.y, getcolor(color));
        }else{
            al_draw_filled_rectangle(p1.x, p1.y, p2.x, p2.y, getcolor(color));
        }
    }
}
void MoveColumn(int colno,float shift){
    List colorList = GetColumn(colno);
    Location start;
    Location end;
    start.x = colno*Unit + GameTableStart.x;
    start.y = GameTableStart.y;
    end.x = (colno+1)*Unit + GameTableStart.x;
    end.y = GameTableLength + GameTableStart.y;
    al_draw_filled_rectangle(start.x, start.y, end.x, end.y, TableBackgroundColor);
    start.y += Unit*Margin;
    end.y -= Unit*Margin;
    for(int i=0;i<colorList.Length;i++){
        Color color = *(Color *)Get(colorList, i);
        Location p1;
        Location p2;
        Position pos;
        pos.x = colno;
        pos.y = i;
        GetNodeLocation(pos, &p1, &p2);
        ModShift(&shift);
        p1.y += shift;
        p2.y += shift;
        if(p1.y > end.y){
            p1.y -= GameTableLength;
            p2.y -= GameTableLength;
        }
        if(p2.y > end.y){
            float remaining = p2.y - end.y;
            al_draw_filled_rectangle(p1.x, p1.y, p2.x, end.y, getcolor(color));
            al_draw_filled_rectangle(p1.x, start.y, p2.x, start.y+remaining-Unit*Margin*2, getcolor(color));
        }else{
            al_draw_filled_rectangle(p1.x, p1.y, p2.x, p2.y, getcolor(color));
        }
    }
}

Position GetPositionOnTable(Location loc){
    Position pos;
    pos.x = floor((loc.x - GameTableStart.x)/Unit);
    pos.y = floor((loc.y - GameTableStart.y)/Unit);
    return pos;
}
void MouseMoveEventHandler(){
    al_get_mouse_state(&mouse.state);
    if(mouse.state.x < GameTableStart.x) return;
    if(mouse.state.y < GameTableStart.y) return;
    if(mouse.state.x > GameTableStart.x + GameTableLength) return;
    if(mouse.state.y > GameTableStart.x + GameTableLength) return;
    if(mouse.direction == None){
        float diffX = fabsf(mouse.start.x - mouse.state.x);
        float diffY = fabsf(mouse.start.y - mouse.state.y);
        if(diffX > Unit/10){
            mouse.direction = Horizontal;
        }else if(diffY > Unit/10){
            mouse.direction = Vertical;
        }
    }else if(mouse.direction == Horizontal){
        Position mousestart;
        mousestart = GetPositionOnTable(mouse.start);
        StartDrawing();
        MoveRow(mousestart.y, mouse.state.x - mouse.start.x);
        EndDrawing();
    }else if(mouse.direction == Vertical){
        Position mousestart;
        mousestart = GetPositionOnTable(mouse.start);
        StartDrawing();
        MoveColumn(mousestart.x, mouse.state.y - mouse.start.y);
        EndDrawing();
    }
}
void ShiftRow(int rowNo){
    Color temp = Table[rowNo][0];
    for(int i=0;i<EDGE_SIZE-1;i++){
        Table[rowNo][i] = Table[rowNo][i+1];
    }
    Table[rowNo][EDGE_SIZE-1] = temp;
}
void ShiftColumn(int colNo){
    Color temp = Table[0][colNo];
    for(int i=0;i<EDGE_SIZE-1;i++){
        Table[i][colNo] = Table[i+1][colNo];
    }
    Table[EDGE_SIZE-1][colNo] = temp;
}
void MouseUpEventHandler(){
    mouse.isActive = false;
    al_get_mouse_state(&mouse.state);
    Position MouseStartPos = GetPositionOnTable(mouse.start);
    Color newtable[EDGE_SIZE][EDGE_SIZE];
    CloneTable(Table,newtable);
    if(mouse.direction == Horizontal){
        int rowno = MouseStartPos.y;
        float shifted = ((float)mouse.state.x) - mouse.start.x;
        ModShift(&shifted);
        int shiftUnit = round(shifted/Unit);
        for(int i=0;i<EDGE_SIZE-shiftUnit;i++) ShiftRow(rowno);
    }else if(mouse.direction == Vertical){
        int colno = MouseStartPos.x;
        float shifted = ((float)mouse.state.y) - mouse.start.y;
        ModShift(&shifted);
        int shiftUnit = round(shifted/Unit);
        for(int i=0;i<EDGE_SIZE-shiftUnit;i++) ShiftColumn(colno);
    }
    int exploded = Explode();
    if(exploded == 0) CloneTable(newtable, Table);
    StartDrawing();
    EndDrawing();
}
void MouseDownEventHandler(){
    al_get_mouse_state(&mouse.state);
    mouse.isActive = true;
    mouse.start.x = mouse.state.x;
    mouse.start.y = mouse.state.y;
    mouse.direction = None;
}
void TimerEventHandler(){

}
#pragma endregion

int main() {
    Start();
    NewGame();
    ALLEGRO_TIMER * timer;
    timer = al_create_timer(0.1);
    ALLEGRO_EVENT_QUEUE * queue = al_create_event_queue();
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());
    al_start_timer(timer);
    while(isGameActive){
        ALLEGRO_EVENT e;
        al_wait_for_event(queue, &e);
        switch (e.type) {
            case ALLEGRO_EVENT_TIMER:
                TimerEventHandler();
                if(mouse.isActive) MouseMoveEventHandler();
                break;
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                MouseDownEventHandler();
                break;
            case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                MouseUpEventHandler();
                break;
            default:
                break;
        }
        
    }
    //al_draw_text(font, al_map_rgb(255, 0, 0), 100, 100,0, "Hello");
    al_rest(999999);
    return 0;
}
