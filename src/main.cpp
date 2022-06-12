#include "uhero/uhero.hpp"

int main(int argc, char** args)
{
	uhero::Context engine = uhero::Context::create_context(argc, args);

	engine.shutdown();
	return 0;
}