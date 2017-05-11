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
#include <string.h>
#define COLOR_COUNT 8
#define ADD(type,list,item) ListNode * n = NewListNode();if(list.Length == 0){list.start = n;list.end = n;}else{list.end->next = n;list.end = n;}   (type *) n->pointer = (type *)malloc(sizeof(type));\
								*((type *)n->pointer) = item;list.Length++;
#define GET_PTR(type,list,i)	(type *) Get(list,i)
#define GET(type,list,i) *GET_PTR(type,list,i)
#define NewList	{NULL,NULL,0}
#define loop(type,list,i,start,itemname,nodename) {\
		ListNode * nodename = GetListNode(list, start);\
		type * itemname;\
		for (int i = start; i < list.Length; i++, nodename = nodename->next) {\
			itemname = (type *)nodename->pointer;
#define endloop }}
/* TYPE DEFINITIONS */ 
typedef enum {
	Red, Green, Blue, Yellow, Purple, Grey, Pink, Orange, Empty
} Color;
typedef struct {
	int x;
	int y;
} Position;
typedef struct ListNode {
	void * pointer;
	struct ListNode * next;
} ListNode;
typedef struct {
	ListNode * start;
	ListNode * end;
	unsigned int Length;
} List;
typedef struct {
	float x;
	float y;
} Location;
typedef enum {
	None, Vertical, Horizontal
} Direction;
typedef struct {
	bool isActive;
	Location start;
	Direction direction;
	ALLEGRO_MOUSE_STATE state;
} Mouse;
typedef struct {
	Position start;
	int endRow;
	float CompletedUnits;
	Color color;
}FallingObject;
/* GLOBALS*/
Color Table[COLOR_COUNT][COLOR_COUNT];
ALLEGRO_DISPLAY * display;
ALLEGRO_FONT * font;
float Unit;
float Margin;
Location GameTableStart;
float GameTableLength;
ALLEGRO_COLOR TableBackgroundColor;
ALLEGRO_TIMER * timer;
ALLEGRO_EVENT_QUEUE * queue;
Mouse mouse;
bool isGameActive;
float FallingSpeed;
bool isFalling;
List Falling[COLOR_COUNT];
/* FUNCTIONS */
ListNode * NewListNode() {
	ListNode * n = (ListNode *)malloc(sizeof(ListNode));
	n->next = NULL;
	return n;
}
ListNode * GetListNode(List list, int no) {
	if (no > list.Length) return NULL;
	ListNode * n = list.start;
	for (int i = 1; i <= no; i++) {
		n = n->next;
	}
	return n;
}
void * Get(List list, int no) {
	ListNode * n = GetListNode(list, no);
	return n->pointer;
}
Position NewPosition(int x, int y) {
	Position p; p.x = x; p.y = y; return p;
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
	List colorlist = NewList;
	for (int i = 0; i < COLOR_COUNT; i++) {
		ADD(Color, colorlist, Table[rowno][i]);
	}
	return colorlist;
}
List GetColumn(int colno) {
	List colorlist = NewList;
	for (int i = 0; i < COLOR_COUNT; i++) {
		ADD(Color, colorlist, Table[i][colno]);
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
	Unit = GameTableLength / (float)COLOR_COUNT;
	Margin = 0.01;
	GameTableStart.x = (width - GameTableLength) / 2;
	GameTableStart.y = (height - GameTableLength) / 2;
	TableBackgroundColor = al_map_rgb(0, 0, 0);
	FallingSpeed = 0.4;
	display = al_create_display(width, height);
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_init_ttf_addon();
	al_init_font_addon();
	al_install_keyboard();
	al_install_mouse();
	al_set_window_title(display, "Jingo Game");
	ALLEGRO_PATH * path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	//al_set_path_filename(path, "Pacifico.ttf");
	//font = al_load_ttf_font(al_path_cstr(path, '/'), 20, 0);
}
void CreateNodes() {
	for (int x = 0, y = 0; x<COLOR_COUNT, y<COLOR_COUNT; x++) {
		Table[y][x] = rand() % COLOR_COUNT;
		if (x == COLOR_COUNT - 1) { x = -1; y++; }
	}
}
void GetNodeLocation(Position pos, Location * p1, Location * p2) {
	p1->x = pos.x*Unit + Unit*Margin + GameTableStart.x;
	p1->y = pos.y*Unit + Unit*Margin + GameTableStart.y;
	p2->x = (pos.x + 1)*Unit - Unit*Margin + GameTableStart.x;
	p2->y = (pos.y + 1)*Unit - Unit*Margin + GameTableStart.y;
}
void GetFallingObjectLocation(FallingObject obj, Location *p1, Location *p2) {
	Location start_p1, start_p2;
	GetNodeLocation(obj.start, &start_p1, &start_p2);
	p1->y = start_p1.y + Unit * obj.CompletedUnits;
	p1->x = start_p1.x;
	p2->x = start_p2.x;
	p2->y = start_p2.y + Unit * obj.CompletedUnits;
}
void DrawNodeByLocation(Location p1, Location p2, Color color) {
	al_draw_filled_rectangle(p1.x, p1.y, p2.x, p2.y, getcolor(color));
}
void DrawNode(Position pos, Color color) {
	Location p1;
	Location p2;
	GetNodeLocation(pos, &p1, &p2);
	DrawNodeByLocation(p1, p2, color);
}
void DrawNodes() {
	for (int x = 0, y = 0; x<COLOR_COUNT, y<COLOR_COUNT; x++) {
		Position pos;
		pos.x = x; pos.y = y;
		DrawNode(pos, Table[y][x]);
		if (x == COLOR_COUNT - 1) { x = -1; y++; }
	}
}
void DrawTableBackground() {
	al_draw_filled_rectangle(GameTableStart.x - Unit, GameTableStart.y - Unit, GameTableStart.x + Unit * 9, GameTableStart.y + Unit * 9, TableBackgroundColor);
}
void DrawTable() {
	DrawTableBackground();
	DrawNodes();
}
void DrawMenuItem(char * s, Position pos) {
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
	List list = NewList;
	for (int x = 0, y = 0; x<COLOR_COUNT, y<COLOR_COUNT; x++) {
		if (Table[y][x] == color) {
			Position pos;
			pos.x = x; pos.y = y;
			ADD(Position, list, pos);
		}
		if (x == COLOR_COUNT - 1) { x = -1; y++; }
	}
	return list;
}
List CategoryNodesByColor() {
	List ListOfListOfPositions = NewList;
	for (int i = 0; i < COLOR_COUNT; i++) {
		List newlist = NewList;
		ADD(List, ListOfListOfPositions, newlist);
	}
	for (int x = 0, y = 0; x < COLOR_COUNT, y < COLOR_COUNT; x++) {
		Position pos;
		pos.x = x; pos.y = y;
		int tt = Table[y][x];
		ADD(Position, (GET(List, ListOfListOfPositions, Table[y][x])), pos);
		if (x == COLOR_COUNT - 1) { x = -1; y++; }
	}
	return ListOfListOfPositions;
}
bool isPositionExists(Position pos, List positionList) {
	if (positionList.Length == 0) return false;
	
	loop(Position, positionList, i, 0,item, node)
		if (pos.x == item->x && pos.y == item->y) return true;
	endloop
	return false;
}
List SearchForExplode() {
	List category = CategoryNodesByColor();
	List explodes = NewList;
	loop(List,category,i,0, ColorPositionList,node)
		if (ColorPositionList->Length == 0) continue;
		List ExplodeListOfPositionList = NewList;

		int isChanged = 1;
		int current = 0;
		while (isChanged) {
			List ExplodePositionList = NewList;
			isChanged = 0;
			bool checking = true;
			while (checking) {
				checking = false;
				loop(Position, (*ColorPositionList), j, current, pos, node2)
					int used = 0;
					loop(List, ExplodeListOfPositionList,l,0, posList, node3)
						if (isPositionExists(*pos, *posList)) {
							used = 1; break;
						}
					endloop
					if (isPositionExists(*pos, ExplodePositionList)) used = 1;
					if (used) continue;
					if (ExplodePositionList.Length == 0) { ADD(Position, ExplodePositionList, (*pos)); continue; }
					loop(Position, ExplodePositionList,g,0,pos2, node3)
						int diffX = abs(pos->x - pos2->x);
						int diffY = abs(pos->y - pos2->y);
						if (diffX + diffY == 1) {
							ADD(Position, ExplodePositionList, (*pos));
							j = ColorPositionList->Length;
							checking = true;
						}
					endloop
				endloop
			}
			if (ExplodePositionList.Length > 2) {
				ADD(List, ExplodeListOfPositionList, ExplodePositionList);
				isChanged = 1;
			}
			else if (current<ColorPositionList->Length) {
				current++;
				isChanged = 1;
			}
		}
		ADD(List, explodes, ExplodeListOfPositionList);
	endloop
	return explodes;
}
void StartDrawing() {
	DrawTable();
}
void EndDrawing() {
	//al_draw_text(font, getcolor(Red), 50, 100, 0, "Test");
	al_flip_display();
}
void SetEmpty(List positionList) {
	loop(Position, positionList,i,0,pos,node)
		Table[pos->y][pos->x] = Empty;
	endloop
}
List FallColumn(int colNo) {
	List colList = GetColumn(colNo);
	List fallList = NewList;
	loop(Color, colList,i,0,color,node)
		Color color = GET(Color, colList, i);
		if (color == Empty) continue;
		int emptycount = 0;
		loop(Color, colList,j, i+1,belowColor,node2)
			if (*belowColor == Empty) emptycount++;
		endloop
		if (emptycount > 0) {
			FallingObject obj;
			obj.color = color;
			obj.start.x = colNo;
			obj.start.y = i;
			obj.CompletedUnits = 0;
			obj.endRow = i + emptycount;
			ADD(FallingObject, fallList, obj);
		}
		if (i >= colList.Length - 2) break;
	endloop
	return fallList;
}
void FallHandler() {
	isFalling = true;
	while (isFalling && isGameActive) {
		ALLEGRO_EVENT e;
		al_wait_for_event(queue, &e);
		switch (e.type) {
		case ALLEGRO_EVENT_TIMER:
			isFalling = false;
			StartDrawing();
			for (int i = 0; i<COLOR_COUNT; i++) {
				if (Falling[i].Length > 0) {
					FallingObject last = *(FallingObject *) Falling[i].end->pointer;
					Location top_p1, top_p2, bottom_p1, bottom_p2;
					GetNodeLocation(NewPosition(i, 0), &top_p1, &top_p2);
					GetNodeLocation(NewPosition(i, COLOR_COUNT - 1), &bottom_p1, &bottom_p2);
					al_draw_filled_rectangle(top_p1.x, top_p1.y, bottom_p2.x, bottom_p2.y, getcolor(Empty));
					for (int j = last.start.y + 1; j < COLOR_COUNT; j++) {
						Position pos = NewPosition(i, j);
						Color color = GetPositionColor(pos);
						DrawNode(pos, color);
					}
				}
				loop(FallingObject, Falling[i],j,0,obj,node)
					float diff = obj->endRow - obj->start.y;
					if (diff > obj->CompletedUnits) {
						isFalling = true;
						obj->CompletedUnits += FallingSpeed;
					}
					Location p1, p2;
					GetFallingObjectLocation(*obj, &p1, &p2);
					DrawNodeByLocation(p1, p2, obj->color);
				endloop
			}
			EndDrawing();
			break;
		default:
			break;
		}
	}
	for (int i = 0; i<COLOR_COUNT; i++) {
		if (Falling[i].Length > 0) {
			FallingObject obj = *(FallingObject *) Falling[i].start->pointer;
			int diff = obj.endRow - obj.start.y;
			if (!obj.endRow == 0)
				for (int j = 0; j < diff; j++) {
					Table[j][i] = Empty;
				}
		}
		loop(FallingObject, Falling[i],j,0,obj,node)
			Table[obj->endRow][i] = obj->color;
		endloop
	}
	isFalling = false;
}
void Fall() {
	for (int i = 0; i<COLOR_COUNT; i++) {
		Falling[i] = FallColumn(i);
	}
	FallHandler();
	StartDrawing();
	EndDrawing();
	for (int i = 0; i < COLOR_COUNT; i++) {
		List col = GetColumn(i);
		int emptycount = 0;
		loop(Color, col,j,0,color,node)
			if (*color == Empty) emptycount++;
		endloop
		List newNodeList = NewList;
		for (int j = 0; j < emptycount; j++) {
			FallingObject newobj;
			newobj.start.x = i;
			newobj.start.y = j - emptycount - 1;
			newobj.color = rand() % COLOR_COUNT;
			newobj.CompletedUnits = 0;
			newobj.endRow = j;
			ADD(FallingObject, newNodeList, newobj);
		}
		Falling[i] = newNodeList;
	}
	FallHandler();
	StartDrawing();
	EndDrawing();
}
void FillEmptyNodes() {
	for (int x = 0, y = 0; x<COLOR_COUNT, y<COLOR_COUNT; x++) {
		if (Table[y][x] == Empty) Table[y][x] = rand() % COLOR_COUNT;
		if (x == COLOR_COUNT - 1) { x = -1; y++; }
	}
}
int Explode() {
	int exploded = 0;
	List exp;
	bool isExploding = true;
	int combo = 0;
	do {
		isExploding = false;
		exp = SearchForExplode();
		loop(List, exp,i,0, colorposlist,node)
			loop(List,(*colorposlist),j,0, poslist,node2)
				if (poslist->Length > 0) {
					isExploding = true;
					exploded += poslist->Length;
					SetEmpty(*poslist);
					combo++;
				}
			endloop
		endloop
		StartDrawing();
		EndDrawing();
		Fall();
		FillEmptyNodes();
	} while (isExploding);
	return exploded;
}
void NewGame() {
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
void CloneTable(Color(*from)[COLOR_COUNT], Color(*to)[COLOR_COUNT]) {
	for (int i = 0; i<COLOR_COUNT; i++) {
		for (int j = 0; j<COLOR_COUNT; j++) {
			to[i][j] = from[i][j];
		}
	}
}
void ModShift(float * shift) {
	float k = *shift / GameTableLength;
	if (k > 0) {
		*shift -= floor(k)*GameTableLength;
	}
	else if (k<0) {
		*shift += (floor(-k) + 1)*GameTableLength;
	}
}
void MoveRow(int rowno, float shift) {
	List colorList = GetRow(rowno);
	Location start;
	Location end;
	start.x = GameTableStart.x;
	start.y = rowno*Unit + GameTableStart.y;
	end.x = Unit*COLOR_COUNT + GameTableStart.x;
	end.y = (rowno + 1)*Unit + GameTableStart.y;
	al_draw_filled_rectangle(start.x, start.y, end.x, end.y, TableBackgroundColor);
	start.x += Unit*Margin;
	end.x -= Unit*Margin;
	loop(Color, colorList,i,0,color,node)
		Location p1;
		Location p2;
		Position pos;
		pos.x = i;
		pos.y = rowno;
		GetNodeLocation(pos, &p1, &p2);
		ModShift(&shift);
		p1.x += shift;
		p2.x += shift;
		if (p1.x > end.x) {
			p1.x -= GameTableLength;
			p2.x -= GameTableLength;
		}
		if (p2.x > end.x) {
			float remaining = p2.x - end.x;
			al_draw_filled_rectangle(p1.x, p1.y, end.x, p2.y, getcolor(*color));
			al_draw_filled_rectangle(start.x, p1.y, start.x + remaining - Unit*Margin * 2, p2.y, getcolor(*color));
		}
		else {
			al_draw_filled_rectangle(p1.x, p1.y, p2.x, p2.y, getcolor(*color));
		}
	endloop
}
void MoveColumn(int colno, float shift) {
	List colorList = GetColumn(colno);
	Location start;
	Location end;
	start.x = colno*Unit + GameTableStart.x;
	start.y = GameTableStart.y;
	end.x = (colno + 1)*Unit + GameTableStart.x;
	end.y = GameTableLength + GameTableStart.y;
	al_draw_filled_rectangle(start.x, start.y, end.x, end.y, TableBackgroundColor);
	start.y += Unit*Margin;
	end.y -= Unit*Margin;
	loop(Color, colorList,i,0,color,node)
		Location p1;
		Location p2;
		Position pos;
		pos.x = colno;
		pos.y = i;
		GetNodeLocation(pos, &p1, &p2);
		ModShift(&shift);
		p1.y += shift;
		p2.y += shift;
		if (p1.y > end.y) {
			p1.y -= GameTableLength;
			p2.y -= GameTableLength;
		}
		if (p2.y > end.y) {
			float remaining = p2.y - end.y;
			al_draw_filled_rectangle(p1.x, p1.y, p2.x, end.y, getcolor(*color));
			al_draw_filled_rectangle(p1.x, start.y, p2.x, start.y + remaining - Unit*Margin * 2, getcolor(*color));
		}
		else {
			al_draw_filled_rectangle(p1.x, p1.y, p2.x, p2.y, getcolor(*color));
		}
	endloop
}
Position GetPositionOnTable(Location loc) {
	Position pos;
	pos.x = floor((loc.x - GameTableStart.x) / Unit);
	pos.y = floor((loc.y - GameTableStart.y) / Unit);
	return pos;
}
void MouseMoveEventHandler() {
	al_get_mouse_state(&mouse.state);
	if (mouse.state.x < GameTableStart.x) return;
	if (mouse.state.y < GameTableStart.y) return;
	if (mouse.state.x > GameTableStart.x + GameTableLength) return;
	if (mouse.state.y > GameTableStart.x + GameTableLength) return;
	if (mouse.direction == None) {
		float diffX = fabsf(mouse.start.x - mouse.state.x);
		float diffY = fabsf(mouse.start.y - mouse.state.y);
		if (diffX > Unit / 10) {
			mouse.direction = Horizontal;
		}
		else if (diffY > Unit / 10) {
			mouse.direction = Vertical;
		}
	}
	else if (mouse.direction == Horizontal) {
		Position mousestart;
		mousestart = GetPositionOnTable(mouse.start);
		StartDrawing();
		MoveRow(mousestart.y, mouse.state.x - mouse.start.x);
		EndDrawing();
	}
	else if (mouse.direction == Vertical) {
		Position mousestart;
		mousestart = GetPositionOnTable(mouse.start);
		StartDrawing();
		MoveColumn(mousestart.x, mouse.state.y - mouse.start.y);
		EndDrawing();
	}
}
void ShiftRow(int rowNo) {
	Color temp = Table[rowNo][0];
	for (int i = 0; i<COLOR_COUNT - 1; i++) {
		Table[rowNo][i] = Table[rowNo][i + 1];
	}
	Table[rowNo][COLOR_COUNT - 1] = temp;
}
void ShiftColumn(int colNo) {
	Color temp = Table[0][colNo];
	for (int i = 0; i<COLOR_COUNT - 1; i++) {
		Table[i][colNo] = Table[i + 1][colNo];
	}
	Table[COLOR_COUNT - 1][colNo] = temp;
}
void MouseUpEventHandler() {
	mouse.isActive = false;
	al_get_mouse_state(&mouse.state);
	Position MouseStartPos = GetPositionOnTable(mouse.start);
	Color newtable[COLOR_COUNT][COLOR_COUNT];
	CloneTable(Table, newtable);
	if (mouse.direction == Horizontal) {
		int rowno = MouseStartPos.y;
		float shifted = ((float)mouse.state.x) - mouse.start.x;
		ModShift(&shifted);
		int shiftUnit = round(shifted / Unit);
		for (int i = 0; i<COLOR_COUNT - shiftUnit; i++) ShiftRow(rowno);
	}
	else if (mouse.direction == Vertical) {
		int colno = MouseStartPos.x;
		float shifted = ((float)mouse.state.y) - mouse.start.y;
		ModShift(&shifted);
		int shiftUnit = round(shifted / Unit);
		for (int i = 0; i<COLOR_COUNT - shiftUnit; i++) ShiftColumn(colno);
	}
	int exploded = Explode();
	if (exploded == 0) CloneTable(newtable, Table);
	StartDrawing();
	EndDrawing();
}
void MouseDownEventHandler() {
	al_get_mouse_state(&mouse.state);
	mouse.isActive = true;
	mouse.start.x = mouse.state.x;
	mouse.start.y = mouse.state.y;
	mouse.direction = None;
}
void TimerEventHandler() {

}
int main() {
	Start();
	NewGame();
	timer = al_create_timer(0.05);
	queue = al_create_event_queue();
	al_register_event_source(queue, al_get_timer_event_source(timer));
	al_register_event_source(queue, al_get_mouse_event_source());
	al_start_timer(timer);
	while (isGameActive) {
		ALLEGRO_EVENT e;
		al_wait_for_event(queue, &e);
		switch (e.type) {
		case ALLEGRO_EVENT_TIMER:
			TimerEventHandler();
			if (mouse.isActive) MouseMoveEventHandler();
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			if (isFalling) break;
			MouseDownEventHandler();
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			if (isFalling) break;
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
