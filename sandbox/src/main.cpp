#include <App.h>
#include <sandboxLayer.h>

int main(){
	Monolith::App app;
	
	app.PushLayer(std::make_unique<sandboxLayer>());
	app.Run();

	return 0;
}
