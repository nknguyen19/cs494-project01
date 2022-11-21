#include "server.h"

using namespace std;

int main (int argc, char *argv[])
{
    Server *server = new Server();
    server->run();
    return 0;
}