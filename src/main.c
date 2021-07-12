int process_args(int argc, char **argv);
void process_code(int code);

int main(int argc, char **argv){
	int ret;
	ret = process_args(argc, argv);
	process_code(ret);
	return ret;
}
