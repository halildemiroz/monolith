#include <SnakeGame.h>
#include <Input.h>
#include <Renderer.h>
#include <TextureLib.h>
#include <Log.h>
#include <imgui.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>

SnakeGame::SnakeGame(Monolith::Camera2D& cam) : m_cam(cam) {}
SnakeGame::~SnakeGame() = default;

glm::vec2 SnakeGame::GridToWorld(glm::ivec2 gridPos) const {
	// Returns the center of the cell in world coordinates
	return glm::vec2(
		gridPos.x * CellSize + CellSize * 0.5f,
		gridPos.y * CellSize + CellSize * 0.5f
	);
}

bool SnakeGame::IsOnSnake(glm::ivec2 pos) const {
	for(const auto& segment : m_body){
		if(segment == pos) return true;
	}
	return false;
}

void SnakeGame::SpawnFood(){
	// Keep picking random positions until we find one not on the snake
	do {
		m_foodPos.x = std::rand() % GridWidth;
		m_foodPos.y = std::rand() % GridHeight;
	} while(IsOnSnake(m_foodPos));
}

void SnakeGame::Reset(){
	m_body.clear();

	// Start with 3 segments in the center, moving right
	int startX = GridWidth / 2;
	int startY = GridHeight / 2;
	m_body.push_front({startX, startY});     // head
	m_body.push_back({startX - 1, startY});  // body
	m_body.push_back({startX - 2, startY});  // tail

	m_direction = Direction::Right;
	m_nextDirection = Direction::Right;
	m_stepTimer = 0.0f;
	m_score = 0;
	m_gameOver = false;

	SpawnFood();
}

void SnakeGame::OnAttach(){
	std::srand(static_cast<unsigned>(std::time(nullptr)));

	// Load textures
	m_snakeHeadTex = Monolith::TextureLib::Load("sandbox/snake/assets/snake_head.png");
	m_snakeBodyTex = Monolith::TextureLib::Load("sandbox/snake/assets/snake_body.png");
	m_foodTex      = Monolith::TextureLib::Load("sandbox/snake/assets/food.png");
	m_bgTex        = Monolith::TextureLib::Load("sandbox/snake/assets/cell_bg.png");

	// Center camera on the grid
	float gridWorldWidth  = GridWidth * CellSize;
	float gridWorldHeight = GridHeight * CellSize;
	m_cam.setPosition({gridWorldWidth * 0.5f, gridWorldHeight * 0.5f});

	Reset();

	APP_INFO("Snake game started!");
}

void SnakeGame::OnUpdate(float deltaTime){
	// Read input — update next direction with 180-degree reversal prevention
	if(Monolith::Input::isKeyPressed(Monolith::Key::W) && m_direction != Direction::Down)
		m_nextDirection = Direction::Up;
	if(Monolith::Input::isKeyPressed(Monolith::Key::S) && m_direction != Direction::Up)
		m_nextDirection = Direction::Down;
	if(Monolith::Input::isKeyPressed(Monolith::Key::A) && m_direction != Direction::Right)
		m_nextDirection = Direction::Left;
	if(Monolith::Input::isKeyPressed(Monolith::Key::D) && m_direction != Direction::Left)
		m_nextDirection = Direction::Right;
}

void SnakeGame::Step(){
	// Apply the queued direction
	m_direction = m_nextDirection;

	// Calculate new head position
	glm::ivec2 head = m_body.front();
	switch(m_direction){
		case Direction::Up:    head.y += 1; break;
		case Direction::Down:  head.y -= 1; break;
		case Direction::Left:  head.x -= 1; break;
		case Direction::Right: head.x += 1; break;
	}

	// Check wall collision
	if(head.x < 0 || head.x >= GridWidth || head.y < 0 || head.y >= GridHeight){
		m_gameOver = true;
		if(m_score > m_highScore) m_highScore = m_score;
		APP_INFO("Game Over! Hit wall. Score: ", m_score);
		return;
	}

	// Check self collision (skip the tail since it will move away, unless we're growing)
	for(size_t i = 0; i < m_body.size() - 1; ++i){
		if(m_body[i] == head){
			m_gameOver = true;
			if(m_score > m_highScore) m_highScore = m_score;
			APP_INFO("Game Over! Bit yourself. Score: ", m_score);
			return;
		}
	}

	// Move: push new head
	m_body.push_front(head);

	// Check food collision
	if(head == m_foodPos){
		// Grow — don't remove the tail
		m_score++;
		SpawnFood();

		// Speed up slightly as score increases
		m_stepInterval = std::max(0.05f, 0.12f - m_score * 0.003f);
	} else {
		// Not eating — remove tail
		m_body.pop_back();
	}
}

void SnakeGame::OnFixedUpdate(float fixedDeltaTime){
	if(m_gameOver){
		// Press any direction key to restart
		if(Monolith::Input::isKeyPressed(Monolith::Key::W) ||
		   Monolith::Input::isKeyPressed(Monolith::Key::A) ||
		   Monolith::Input::isKeyPressed(Monolith::Key::S) ||
		   Monolith::Input::isKeyPressed(Monolith::Key::D)){
			Reset();
		}
		return;
	}

	m_stepTimer += fixedDeltaTime;
	if(m_stepTimer >= m_stepInterval){
		Step();
		m_stepTimer -= m_stepInterval;
	}
}

void SnakeGame::OnRender(float alpha){
	const float padding = 2.0f; // Small gap between cells
	const Monolith::Vec2 cellDrawSize = {CellSize - padding, CellSize - padding};

	// Draw background grid cells
	for(int y = 0; y < GridHeight; ++y){
		for(int x = 0; x < GridWidth; ++x){
			Monolith::Renderer::DrawFilledRect(GridToWorld({x, y}), cellDrawSize, Monolith::Color{0.15f, 0.15f, 0.15f, 1.0f});
		}
	}

	// Draw grid border with lines
	Monolith::Color borderColor = {0.4f, 0.4f, 0.4f, 1.0f};
	Monolith::Vec2 bottomLeft = {0.0f, 0.0f};
	Monolith::Vec2 bottomRight = {GridWidth * CellSize, 0.0f};
	Monolith::Vec2 topLeft = {0.0f, GridHeight * CellSize};
	Monolith::Vec2 topRight = {GridWidth * CellSize, GridHeight * CellSize};
	Monolith::Renderer::DrawLine(bottomLeft, bottomRight, borderColor);
	Monolith::Renderer::DrawLine(bottomRight, topRight, borderColor);
	Monolith::Renderer::DrawLine(topRight, topLeft, borderColor);
	Monolith::Renderer::DrawLine(topLeft, bottomLeft, borderColor);

	// Draw food (apple) as a red circle
	Monolith::Renderer::DrawCircle(GridToWorld(m_foodPos), (CellSize - padding) * 0.45f, Monolith::Color::Red);

	// Draw snake body (back to front so head draws on top)
	for(size_t i = m_body.size(); i > 0; --i){
		size_t idx = i - 1;
		Monolith::Color color = (idx == 0) ? Monolith::Color{0.2f, 0.9f, 0.2f, 1.0f} : Monolith::Color{0.1f, 0.6f, 0.1f, 1.0f};
		Monolith::Renderer::DrawFilledRect(GridToWorld(m_body[idx]), cellDrawSize, color);
	}
}

void SnakeGame::OnImGuiRender(){
	ImGui::Begin("Snake");

	ImGui::Text("Score: %d", m_score);
	ImGui::Text("High Score: %d", m_highScore);
	ImGui::Text("Speed: %.0f%%", (0.12f / m_stepInterval) * 100.0f);
	ImGui::Text("Length: %zu", m_body.size());

	if(m_gameOver){
		ImGui::Separator();
		ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "GAME OVER!");
		ImGui::Text("Press WASD to restart");
	}

	ImGui::Separator();
	ImGui::Text("Controls: WASD");
	ImGui::Text("Frame time: %.3f ms (%.1f FPS)",
		1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();
}
