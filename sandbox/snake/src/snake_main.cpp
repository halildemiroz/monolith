#include <App.h>
#include <SnakeGame.h>

int main(){
	Monolith::App app;

	app.PushLayer(std::make_unique<SnakeGame>(app.GetCamera()));
	app.Run();

	return 0;
}
