#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "token.h"

void preprocess_file(FILE* ftarget, const char* source) {
	FILE *f = fopen(source, "r");
	fsource = f;

	 // TODO: use lexer to skip line literals & comments
	 while (1) {
		int ch = fgetc(f);
		if (ch == EOF) break;

		fputc(ch, ftarget);

		if (ch == '#') {
			ch = fgetc(f);
			if (ch >= 'a' && ch <= 'z') {
				int i = 0;
				while (ch >= 'a' && ch <= 'z') {
					fputc(ch, ftarget);

					token_id[i++] = ch;
					ch = fgetc(f);
				}
				token_id[i] = 0;

				if (!strcmp(token_id, "include")) {
					while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
						fputc(ch, ftarget);

						ch = fgetc(f);
					}

					if (ch == '"') {
						fputc(ch, ftarget);
						ch = fgetc(f);

						i = 0;
						char filename[256];

						while (ch != '"' && ch != EOF) {
							fputc(ch, ftarget);

							filename[i++] = ch;

							ch = fgetc(f);
						}
						filename[i] = 0;

						fprintf(ftarget, "\"\n");
						ch = fgetc(f);

						preprocess_file(ftarget, filename);
					}
				}
			}

			ungetc(ch, f);
		}
	}

	fclose(f);
}

void preprocess(const char* target, const char* source) {
	FILE* ftarget = fopen(target, "w");
	preprocess_file(ftarget, source);

	fclose(ftarget);
}