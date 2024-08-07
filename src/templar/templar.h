#ifndef TEMPLAR_H
#define TEMPLAR_H

char *wrap_with_braces(const char *word);
int insert_text(const char *text, const char *templar_keywork, char *html);
char *get_keywords();
#endif