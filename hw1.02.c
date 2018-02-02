#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <endian.h>
#include "rooms.h"

// Structure representing a room located at (x,y) with a 
// specific height and width
struct Room {
	int x;
	int y;
	int height;
	int width;
};

// Header prototypes
void createRooms(char board[21][80], struct Room rooms[num], 
		 int hardness[21][80]);
int isSafe(char board[21][80], struct Room room);
void addRoom(char board[21][80], struct Room room, int hardness[21][80]);
void connectRooms(char board[21][80], struct Room rooms[num],
		  int hardness[21][80]);
int rerouteTunnel(char board[21][80], int x, int y);
void printBoard(char board[21][80], int hardness[21][80]);
void initializeBoard(char board[21][80], int hardness[21][80]);
int save(FILE *file, char* path, char board[21][80], int hardness[21][80],
	 struct Room rooms[num]);
int load(FILE *file, char *path, char board[21][80], int hardness[21][80],
	struct Room *rooms, int flag);


// Utility for creating rooms. Possible rooms are generated, and each one is
// tested. If successful, add it to rooms array; else, keep generating rooms
// until one is found
void createRooms(char board[21][80], 
		 struct Room rooms[5], 
		 int hardness[21][80]) {
	int i;
	
	for(i=0; i < num; i++) {
		// Generate starting (x, y) for room
		rooms[i].x = (rand() % 64) + 2;
	       	rooms[i].y = (rand() % 11) + 2;
		while(board[rooms[i].y][rooms[i].x] == '.')
			rooms[i].x = rand() % 64 + 2,
			rooms[i].y = rand() % 11 + 2;		
		// Generate room height and width
		rooms[i].height = rand() % 6 + 2;
		rooms[i].width = rand() % 12 + 3;

		// Keep generating rooms until one is successful
		while(isSafe(board, rooms[i]) != 1)
			rooms[i].x = (rand() % 64) + 2,
	       		rooms[i].y = (rand() % 11) + 2,
			rooms[i].height = rand() % 6 + 2,
			rooms[i].width = rand() % 12 + 3;
		
		addRoom(board, rooms[i], hardness);
	}
}

// Utility to check if a room can be placed with the specific parameters, and
// does not violate any rules
int isSafe(char board[21][80], struct Room room) {
	int row, column;

	for(row=room.y; row < room.y+room.height; row++)
		for(column=room.x; column < room.x+room.width; column++) {
			if(row == room.y && board[row-1][column] == '.')
				return 0;
			if(row == (room.y+room.height)-1 &&
					board[row+1][column] == '.')
				return 0;
			if(column == room.x && board[row][column-1] == '.')
				return 0;
			if(column == (room.x+room.width)-1 &&
					board[row][column+1] == '.')
				return 0;
			if(board[row][column] == '.')
				return 0;
		}
	return 1;
}

// Utility to add room to board
void addRoom(char board[21][80], struct Room room, int hardness[21][80]) {
	int y, x;

	for(y=room.y; y < room.y+room.height; y++)
		for(x=room.x; x < room.x+room.width; x++)
			board[y][x] = '.',
			hardness[y][x] = 0;
}

// Utility to generate tunnels between rooms
void connectRooms(char board[21][80], struct Room rooms[5], 
		  int hardness[21][80]) {
	int i, high, low, decision;
	int y, x, targetY, targetX;
	for(i=0; i < num-1; i++) {
		// Starting cell location in room may be randomly changed
		if(rand() % 2 == 0)
			y = rooms[i].y + rand() % ((rooms[i].y+rooms[i].height) 
				- rooms[i].y);
		else
			y = rooms[i].y;
		if(rand() % 2 == 0)
			x = rooms[i].x + rand() % ((rooms[i].x+rooms[i].width) 
				- rooms[i].x);
		else
			x = rooms[i].x;
		
		// Select a random target cell in next room
		low = rooms[i+1].y;
		high = rooms[i+1].y+rooms[i+1].height;
		targetY = low + rand() % (high - low);
		low = rooms[i+1].x;
		high = rooms[i+1].x+rooms[i+1].width;
		targetX = low + rand() % (high - low);	
		
		// Generating path on y-axis towards target y-value
		while(y != targetY) {
			if(y <= targetY) y+=1;
			else y-=1;
			
			if(rand() % 3 == 0)
				decision = rerouteTunnel(board, x, y);
			
			if(decision == 0) {
				if(board[y][x] != '.') 
					board[y][x] = '#',
					hardness[y][x] = 0;
				if(x+1 != 80) x+=1;
			}
			
			else if(decision == 1) {	
				if(board[y][x] != '.') 
					board[y][x] = '#',
					hardness[y][x] = 0;
				if(x-1 != 0) x-=1;
			}
			
			if(board[y][x] != '.') 
				board[y][x] = '#',
				hardness[y][x] = 0;
		}

		// Generating path on x-axis towards target x-value (linear)
		while(x != targetX) {
			if(x <= targetX) x+=1;
			else x-=1;
			if(board[y][x] != '.')
				board[y][x] = '#',
				hardness[y][x] = 0;
		}	
	}
}

// Utility to determine whether a cell will be generated at 
// x-1 or x+1 from current x location
int rerouteTunnel(char board[21][80], int x, int y) {
	int decision = rand() % 2;

	switch(decision) {
		case 0: if(board[y][x+1] != '.') return 0;
		case 1: if(board[y][x-1] != '.') return 1;
		default: return -1;
	}
}

// Utility to print board
void printBoard(char board[21][80], int hardness[21][80]) {
	int row, column;

	for(row=0; row < 21; row++) {
		for(column=0; column < 80; column++) {
			printf("%c", board[row][column]);
		}
		printf("\n");
	}
}

// Utility to initialize board values
void initializeBoard(char board[21][80], int hardness[21][80]) {
	int row, column;

	for(row=0; row < 21; row++)
		for(column=0; column < 80; column++)
			if(row == 0 || row == 20)
				board[row][column] = '-', 
				hardness[row][column] = 255;
			else if(column == 0 || column == 79)
				board[row][column] = '|',
				hardness[row][column] = 255;
			else
				board[row][column] = ' ',
				hardness[row][column] = (rand()%255) + 1;
}

// Function to save a dungeon to disk. Returns 0 upon success.
int save(FILE *file, char* path, char board[21][80], int hardness[21][80],
	 struct Room rooms[num]) {
	int i, row, column, size;

	file = fopen(path, "w");

	char *semantic = "RLG327-S2018";
	fwrite(semantic, 1, 12, file);
	
	int version = 0; int be = htobe32(version);
	fwrite(&be, 4, 1, file);
	
	// semantic(12)+version(4)+size(4)+hardness(80*21)+rooms(4*num)
	size = 12+4+4+(80*21)+(4*num); int beSize = htobe32(size);
	fwrite(&beSize, 4, 1, file);

	// Write hardness
	for(row=0; row < 21; row++)
		for(column=0; column < 80; column++)
			fwrite(&hardness[row][column], 1, 1, file);
	
	// Write rooms
	for(i=0; i < num; i++) {
		fwrite(&rooms[i].y, 1, 1, file); 
		fwrite(&rooms[i].x, 1, 1, file);
		fwrite(&rooms[i].height, 1, 1, file);
		fwrite(&rooms[i].width, 1, 1, file);
	}

	fclose(file);
	return 0;
}

// Function to load a dungeon file from disk. Returns 0 upon success.
int load(FILE *file, char *path, char board[21][80], int hardness[21][80],
	struct Room *rooms, int flag) {
	
	char semantic[12];
	int version; int size;

	initializeBoard(board, hardness);
	
	file = fopen(path, "r");
	
	fread(&semantic, sizeof(char), 12, file);
	
	fread(&version, 4, 1, file);
	
	fread(&size, 4, 1, file);
	int hSize = be32toh(size);

	// Read hardness, update hardness and board arrays
	int rows, columns;
	for(rows=0; rows<21; rows++) {
		for(columns=0; columns<80; columns++) {
			unsigned char h;
			fread(&h, sizeof(h), 1, file);
			hardness[rows][columns] = h;
			if(hardness[rows][columns] == 0)
				board[rows][columns] = '#';
			else if(rows == 0 || rows == 20)
				board[rows][columns] = '-';
			else if(columns == 0 || columns == 79)
				board[rows][columns] = '|';
			else
				board[rows][columns] = ' ';
		}
	}

	// Read rooms
	hSize = (hSize-1700)/4;
	rooms = malloc(hSize*(sizeof(int)*4));
	int i;
	for(i=0; i<hSize; i++) {
		fread(&rooms[i].y, 1, 1, file);
		fread(&rooms[i].x, 1, 1, file);
		fread(&rooms[i].height, 1, 1, file);
		fread(&rooms[i].width, 1, 1, file);

		addRoom(board, rooms[i], hardness);
	}
	
	// If two switches were included
	if(flag==0)
		save(file, path, board, hardness, rooms);

	fclose(file);
	free(rooms);
	return 0;
}

int main(int argc, char *argv[]) {
	srand(time(NULL));
	char board[21][80];
	int hardness[21][80];
	struct Room rooms[num];
	struct Room *r;
	int length = strlen(getenv("HOME")) + strlen("/.rlg327/Dungeon") + 1;
	char *path = malloc(length * sizeof(char));
	strcpy(path, getenv("HOME"));
      	strcat(path, "/.rlg327/dungeon");
	FILE *file;
	
	initializeBoard(board, hardness);
	
	createRooms(board, rooms, hardness);

	connectRooms(board, rooms, hardness);
	
	// Check input parameters
	switch(argc-1) {
		case 1: 
		       if(strcmp(argv[1], "--save") == 0) {
			       save(file, path, board, hardness, rooms); break;
		       }
		       else if(strcmp(argv[1], "--load") == 0) {
			       load(file, path, board, hardness, r, -1); break;
		       }
		case 2:
		       if((strcmp(argv[1], "--save") == 0 && 
			   strcmp(argv[2], "--load") == 0) ||
			  
			  (strcmp(argv[2], "--save") == 0 &&
			   strcmp(argv[2], "--save") == 0)) {
			       load(file, path, board, hardness, r, 0); break;
	
		       }
	}

	printBoard(board, hardness);
	
	free(path);

	return 0;	
}
