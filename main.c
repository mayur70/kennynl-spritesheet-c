#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

typedef struct Slice {
	char* data;
	size_t len;
} Slice;

typedef struct SubTexture {
	Slice name;
	int x, y, w, h;
} SubTexture;

typedef struct TextureAtlas {
	char* buffer;
	Slice image_path;
	size_t subtextures_count;
	SubTexture* subtextures;
} TextureAtlas;

char* next_line(char* ptr) {
	char* end = strchr(ptr, '\n');
	return end;
}

bool texture_atlas_load(TextureAtlas* atlas, const char* filename) {
	FILE *fp = NULL;
	fp = fopen("Spritesheet/sheet.xml", "r");

	if(fp == NULL) {
		fprintf(stderr, "failed to open file %s\n", strerror(errno));
		return false;
	}

	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	rewind(fp);
	char* buf = malloc(sz + 1 * sizeof(char));
	if(buf == NULL) {
		fprintf(stderr, "failed to allocate memory\n");
		return false;
	}
	size_t r = fread(buf, sizeof(char), sz, fp);
	assert(r == sz);
	buf[sz] = '\0';
	atlas->buffer = buf;
	char* beg = strstr(atlas->buffer, "imagePath=") + 11;
	char* end = strchr(beg, '"');
	atlas->image_path.data = beg;
	atlas->image_path.len = end - beg;
	beg = next_line(end);
	char* nline = next_line(beg + 1);
	while(strstr(beg, "SubTexture") != NULL) {
		atlas->subtextures_count++;
		atlas->subtextures = realloc(atlas->subtextures,sizeof(*atlas->subtextures)* atlas->subtextures_count);
		if(atlas->subtextures == NULL) {
			fprintf(stderr, "failed to allocate memory\n");
			return false;
		}
		char* nbeg=strstr(beg, "name=") + 6;
		char* nend = strchr(nbeg, '"');
		SubTexture* st = &atlas->subtextures[atlas->subtextures_count-1];
		st->name.data = nbeg;
		st->name.len = nend - nbeg;
		int read =sscanf(nend + 1, " x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\"", &st->x, &st->y, &st->w, &st->h);
		assert(read = 4);
		beg = next_line(nend);
	}
	return true;
}

void texture_atlas_free(TextureAtlas* atlas) {
	free(atlas->subtextures);
	free(atlas->buffer);
}

void texture_atlas_print(TextureAtlas* atlas) {
	printf("TextureAtlas image_path %.*s\n",atlas->image_path.len, atlas->image_path); 
	for(size_t i = 0; i < atlas->subtextures_count; i++){
		SubTexture* st = &atlas->subtextures[i];
		printf("subtexture name %.*s, [%d %d %d %d]\n",st->name.len, st->name.data, st->x, st->y, st->w, st->h);
	}
}


int main(void) {
	TextureAtlas atlas = {0};
	texture_atlas_load(&atlas, "Spritesheet/sheet.xml");
	texture_atlas_print(&atlas);
	texture_atlas_free(&atlas);
}


