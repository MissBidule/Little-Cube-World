#include "App.hpp"

int main()
{
    App app(1920, 1080);
    app.init();
    app.update();
    app.clean();

    return 0;
}
