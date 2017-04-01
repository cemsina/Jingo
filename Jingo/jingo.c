//
//  main.c
//  Jingo
//
//  Created by Cemsina on 17.03.2017.
//  Copyright © 2017 Cemsina. All rights reserved.
//

#include <stdio.h>
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_video.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_native_dialog.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#define ROWS 8
#define COLUMNS 8

#pragma region Type Definitions
typedef enum ColorsEnum {
	Red, Green, Blue, Yellow, Purple, Grey, Pink, Orange
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

#pragma endregion
#pragma region Globals
Color Table[ROWS][COLUMNS];
ALLEGRO_DISPLAY * display;
ALLEGRO_FONT * font;
double Unit;
Position GameTableStart;
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
	default:
		return al_map_rgb(0, 0, 0);
		break;
	}
}
List GetRow(int rowno) {
	List colorlist = NewList(COLOR);
	for (int i = 0; i < ROWS; i++) {
		Add(&colorlist, &Table[rowno][i]);
	}
	return colorlist;
}
List GetColumn(int colno) {
	List colorlist = NewList(COLOR);
	for (int i = 0; i < COLUMNS; i++) {
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
	int GameTableLength = ((width < height) ? width : height) * 7 / 10;
	GameTableLength -= GameTableLength % 8;
	Unit = GameTableLength/8;
	GameTableStart.x = (width-GameTableLength)/2;
	GameTableStart.y = (height - GameTableLength) / 2;
	display = al_create_display(width, height);
	al_set_window_title(display, "Jingo Game");
	al_init_ttf_addon();
	al_init_font_addon();
	al_install_keyboard();
	/*
		PATH DE SORUN VAR.
	ALLEGRO_PATH * path = al_get_standard_path("C:\Users\cemsi\Documents\Jingo\Jingo\Pacifico.ttf");
	font = al_load_font(path, 20, 0);
	*/
	
}
void CreateNodes() {
	for (int x = 0, y = 0; x<COLUMNS, y<ROWS; x++) {
		Table[y][x] = rand() % 8;
		if (x == COLUMNS-1) {x = -1; y++;}
	}
}
void DrawNode(int x,int y,Color color) {
	al_draw_filled_rectangle(x * Unit + GameTableStart.x, y * Unit + GameTableStart.y, x*Unit + Unit + GameTableStart.x, y*Unit + Unit + GameTableStart.y, getcolor(color));
}
void DrawNodes() {
	for (int x = 0, y = 0; x<COLUMNS, y<ROWS; x++) {
		DrawNode(x, y, Table[y][x]);
		if (x == COLUMNS - 1) { x = -1; y++; }
	}
}
void DrawTableBackground() {
	ALLEGRO_COLOR bg = al_map_rgb(111, 0, 0);
	al_draw_filled_rectangle(GameTableStart.x - Unit, GameTableStart.y - Unit, GameTableStart.x + Unit * 9, GameTableStart.y + Unit * 9, bg);
}
void OnTableChanged() {
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
	for (int x = 0, y = 0; x<COLUMNS, y<ROWS; x++) {
		if (Table[y][x] == color) { 
			Position pos;
			pos.x = x; pos.y = y;
			Add(&list, &pos);
		}
		if (x == COLUMNS - 1) { x = -1; y++; }
	}
	return list;
}
List CategoryNodesByColor() {
	List ListOfListOfPositions = NewList(LIST);
	for (int i = 0; i < 8; i++) { 
		List newlist = NewList(POSITION);
		Add(&ListOfListOfPositions, &newlist);
	}
	for (int x = 0, y = 0; x < COLUMNS, y < ROWS; x++) {
		Position pos;
		pos.x = x; pos.y = y;
		Add((List*)ListOfListOfPositions.ArrayPointer + Table[y][x], &pos);
		if (x == COLUMNS - 1) { x = -1; y++; }
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
#pragma endregion

int main() {
	Start();
	CreateNodes();
	
	List exp = SearchForExplode();
	char * s[8] = { "Red", "Green", "Blue", "Yellow", "Purple", "Grey", "Pink", "Orange" };
	for (int i = 0; i < exp.Length; i++) {
		List colorposlist = *(List *) Get(exp, i);
		printf("Color : %s - %d\n", s[i], colorposlist.Length);
	}
	OnTableChanged();
	//al_draw_text(font, al_map_rgb(255, 0, 0), 100, 100,0, "Hello");
	al_flip_display();
	al_rest(999999);
	return 0;
}


