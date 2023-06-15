/* Get next token from string s (NULL on 2nd, 3rd, etc. calls),
 * where tokens are nonempty strings separated by runs of
 * chars from delim.  Writes NULs into s to end tokens.  delim need not
 * remain constant from call to call.
 */

char *strtok(char *s, const char *delim);

char *strtok(char *s, const char *delim)	 /* 0 if no token left */
{
	register char *scan;
	char *tok;
	register const char *dscan;
	static char *scanpoint = 0;

	if (s == 0 && scanpoint == 0)
		return (0);
	if (s != 0)
		scan = s;
	else
		scan = scanpoint;

	/* Scan leading delimiters. */
	for (; *scan != '\0'; scan++)
	{
		for (dscan = delim; *dscan != '\0'; dscan++)
			if (*scan == *dscan)
				break;
		if (*dscan == '\0')
			break;
	}
	if (*scan == '\0')
	{
		scanpoint = 0;
		return (0);
	}
	tok = scan;

	/* Scan token. */
	for (; *scan != '\0'; scan++)
	{
		for (dscan = delim; *dscan != '\0';)	/* ++ moved down. */
			if (*scan == *dscan++)
			{
				scanpoint = scan + 1;
				*scan = '\0';
				return (tok);
			}
	}

	/* Reached end of string. */
	scanpoint = 0;
	return (tok);
}
