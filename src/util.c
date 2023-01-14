#include <include/util.h>
#include <stdio.h>

void
print_info(const char *msg, color_t color, bool exit_process,
	bool print_line, const char* file, int line)
{
	char *text_col_bold = col_str_style(color, BOLD);

	if(print_line) {
		printf("\n%s%s Line %d: %s%s\n",
			text_col_bold,
			file,
			line,
			msg,
			RESET);
	} else {
		printf("%s%s%s\n", text_col_bold, msg, RESET);
	}
	fflush(stdout);
	free(text_col_bold);

	if(exit_process) {
		exit(1);
	}
}