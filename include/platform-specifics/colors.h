//tui uses ansi colors, gui does not
#if defined use_ansi_colors

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define RESET "\033[0m"


#else 

#define RED ""
#define GREEN ""
#define YELLOW ""
#define BLUE ""
#define RESET ""

#endif
