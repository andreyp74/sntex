#include "client.hpp"

int main(int argc, const char** argv)
{
    Client client("localhost", 9191);
    client.start();

    return 0;
}