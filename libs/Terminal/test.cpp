#include "Terminal.h"
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <cassert>
#include <time.h>
#include <ctype.h>

using namespace std;

int main(int argc, char *argv[]){
	if(argc < 2){
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		return EXIT_FAILURE;
	}

	vector<char> buf;
	FILE *f;
	assert((f = fopen(argv[1], "rt")) != NULL);

	int symbol = fgetc(f);
	do{
		buf.push_back((char)symbol);
		symbol = fgetc(f);
	} while(symbol != EOF);

	fclose(f);

	int total_rows, total_cols;
	assert(get_terminal_width_height(fileno(stdout), &total_cols, &total_rows) != -1);
	fprintf(stdout, "Terminal size %dx%d\n", total_rows, total_cols);
	assert(term_save_state() != -1);
	assert(term_canon_off() != -1);

	const int TAB_WIDTH = 8;
	int cur_col = 0, cur_row = 0;
	int symbol_index;

	const char *b = &buf[0];

	total_rows = 24, total_cols = 80;
	for(symbol_index = 0; symbol_index < buf.size(); ++symbol_index){
		int s = b[symbol_index];

		if(s == '\n'){
			cur_col = 0;
			cur_row++;
		} else if(s == '\t') {
			int tab_position = TAB_WIDTH * ((cur_col + TAB_WIDTH)/TAB_WIDTH);
			cur_col = tab_position;
		} else if(isprint(s)) {
			cur_col++;
		} else {
			continue;
		}

		fputc(s, stdout);		

		if(cur_col > total_cols - 1){
			cur_col = 0;
			cur_row++;
			fputc('\n', stdout);
		}

		if(cur_row == total_rows - 3){
			fputs("\nSLEEP\n", stdout);
			cur_col = 0;
			cur_row = 0;
			sleep(1);
		}

	}

	assert(term_restore_state() != -1);
	return EXIT_SUCCESS;
}
