#include <cli.h>

int main(int argc, char **argv){
	int ret;
	ret = process_args(argc, argv);
	process_code(ret);
	ARGS_FREE_MALLOCD();
	return ret;
}
