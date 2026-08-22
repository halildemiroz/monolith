#pragma once
#include <Camera2D.h>
#include <TextureHandle.h>
#include <Layer.h>
#include <deque>
#include <glm/glm.hpp>

enum class Direction { Up, Down, Left, Right };

class SnakeGame : public Monolith::Layer {
public:
	explicit SnakeGame(Monolith::Camera2D& cam);
	~SnakeGame() override;

	void OnAttach() override;
	void OnUpdate(float deltaTime) override;
	void OnFixedUpdate(float fixedDeltaTime) override;
	void OnRender(float alpha) override;
	void OnImGuiRender() override;

private:
	void Reset();
	void SpawnFood();
	void Step();
	bool IsOnSnake(glm::ivec2 pos) const;
	glm::vec2 GridToWorld(glm::ivec2 gridPos) const;

	Monolith::Camera2D& m_cam;

	// Grid settings
	static constexpr int GridWidth = 20;
	static constexpr int GridHeight = 20;
	static constexpr float CellSize = 32.0f;

	// Snake state
	std::deque<glm::ivec2> m_body; // front = head
	Direction m_direction = Direction::Right;
	Direction m_nextDirection = Direction::Right;

	// Food
	glm::ivec2 m_foodPos{};

	// Timing
	float m_stepTimer = 0.0f;
	float m_stepInterval = 0.12f;

	// Game state
	int m_score = 0;
	int m_highScore = 0;
	bool m_gameOver = false;

	// Textures
	Monolith::TextureHandle m_snakeHeadTex;
	Monolith::TextureHandle m_snakeBodyTex;
	Monolith::TextureHandle m_foodTex;
	Monolith::TextureHandle m_bgTex;
};
