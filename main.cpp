#include <vector>
#include <cstdlib>  // for rand()
#include <iostream> // for "game over" message
#include <SFML/Graphics.hpp>

namespace PC
{
	const unsigned Width = 600, Height = 400;
	const int Framerate = 60;
	
	const sf::Color BackgroundColor = sf::Color::Black;
	const sf::Color SnakeColor = sf::Color::Green;	
	const sf::Color FruitColor = sf::Color::Red;

	// Walk approximately 13 times in a second
	const sf::Time WalkSpeed = sf::milliseconds(75);
}

class Object
{
	public:
		int GetX() { return x; }
		int GetY() { return y; }

		void
		SetTargetWindow(sf::RenderWindow *TargetWindow) 
		{ 
			this->TargetWindow = TargetWindow; 
		}

		bool
		CheckCollision(int x, int y)
		{
			if (x == this->x && y == this->y) return true; else return false;
		}
	
		void
		SetSister(Object *Sister)
		{
			SisterInstance = Sister;
		}

		virtual void
		Destroy()
		{
			return;
		}

	protected:
		int x;
		int y;
		sf::RenderWindow *TargetWindow;
		Object *SisterInstance;
};

class Snake : public Object
{
	public:
		void
		Draw()
		{
			for (sf::RectangleShape SnakePart : SnakeBody)
				TargetWindow->draw(SnakePart);
		}

		void
		ChangeDirection(unsigned Dir) 
		{
			if (Dir == GetOppositeDirection()) return;

			Direction = Dir;
		}	

		void
		Destroy() {
			TargetWindow->close();
			std::cout << "Game Over\nScore: " << Length*10 << std::endl;
		}

		void
		Move(bool Increase=false)
		{
			if (x < -10) x = PC::Width;
			if (y < -10) y = PC::Height;
			if (x > PC::Width)  x = -10;
			if (y > PC::Height) y = -10;

			int FruitX = SisterInstance->GetX();
			int FruitY = SisterInstance->GetY();

			if (CheckCollision(FruitX, FruitY)) {
				SisterInstance->Destroy();
				IncreaseSnake();
			}

			for (int i = 1; i < Length; i++) {
				sf::Vector2f Position = SnakeBody.begin()[i].getPosition();
				if (this->x == Position.x && this->y == Position.y)
					this->Destroy();
			}

			switch (Direction)
			{
				case 1: x -= 10; break;
				case 2: x += 10; break;
				case 3: y -= 10; break;
				case 4: y += 10; break;
			}

			if (!Increase)
				SnakeBody.pop_back();

			SnakeBody.emplace(SnakeBody.begin(), sf::Vector2f(10, 10));
			SnakeBody.front().setFillColor(PC::SnakeColor);
			SnakeBody.front().setPosition(x, y);
		}

		void
		IncreaseSnake()
		{
			Length++;
			Move(true); // Increase=true
		}

		Snake()
		{
			x = PC::Width /2;
			y = PC::Height/2;
			Length = 1;

			SnakeBody.emplace_back(sf::Vector2f(10, 10));
			SnakeBody.front().setFillColor(PC::SnakeColor);
			SnakeBody.front().setPosition(x, y);
		}

	protected:
		unsigned
		GetOppositeDirection()
		{
			switch (Direction)
			{
				case 1:  return 2;
				case 2:  return 1;
				case 3:  return 4;
				case 4:  return 3;
				default: return -1;
			}
		}

		unsigned Direction;
		unsigned Length;
		std::vector<sf::RectangleShape> SnakeBody;
};

class Fruit : public Object
{
	public:
		void Draw() { TargetWindow->draw(FruitRect); }
		void Destroy() { FruitRect.setPosition(GetRandomPosition()); }		

		Fruit() : FruitRect(sf::Vector2f(10, 10))
		{
			FruitRect.setFillColor(PC::FruitColor);
			FruitRect.setPosition(GetRandomPosition());
		}

	protected:
		int
		Round(int n)
		{
			int a = (n/10)*10;
			int b = a+10;
			return (n-a>b-n)?b:a;
		}

		bool IsPositionOK(int x, int y)
		{
			if (x <= 0 || x >= PC::Width || y <= 0 || y >= PC::Height) return false;
			if (SisterInstance->CheckCollision(x, y)) return false;

			return true;
		}

		sf::Vector2f
		GetRandomPosition()
		{
			do {
				x = Round(rand() % PC::Width);
				y = Round(rand() % PC::Height);
			} while (!IsPositionOK(x, y));

			return sf::Vector2f(x, y);
		}

		sf::RectangleShape FruitRect;
};

int
main()
{
	sf::RenderWindow MainWindow(sf::VideoMode(PC::Width, PC::Height), "Snake");
	MainWindow.setFramerateLimit(PC::Framerate);

	srand((unsigned)time(0));

	Snake *SnakeInstance = new Snake();
	Fruit *FruitInstance = new Fruit();

	SnakeInstance->SetTargetWindow(&MainWindow);
	FruitInstance->SetTargetWindow(&MainWindow);

	SnakeInstance->SetSister(FruitInstance);
	FruitInstance->SetSister(SnakeInstance);

	while (MainWindow.isOpen()) {
		sf::Event Event;
		while (MainWindow.pollEvent(Event)) {
			if (Event.type == sf::Event::Closed)
				MainWindow.close();

			if (Event.type == sf::Event::KeyPressed) {
				switch (Event.key.code)
				{
					case sf::Keyboard::Left:
						SnakeInstance->ChangeDirection(1);
						break;

					case sf::Keyboard::Right:
						SnakeInstance->ChangeDirection(2);
						break;
					
					case sf::Keyboard::Up:
						SnakeInstance->ChangeDirection(3);
						break;

					case sf::Keyboard::Down:
						SnakeInstance->ChangeDirection(4);
						break;
					
					case sf::Keyboard::A:
						SnakeInstance->IncreaseSnake();
						break;

					case sf::Keyboard::Escape:
						MainWindow.close();
						break;
					
					default:
						break;
				}
			}
		}

		MainWindow.clear(PC::BackgroundColor);

		sf::sleep(PC::WalkSpeed);
		SnakeInstance->Move();

		FruitInstance->Draw();
		SnakeInstance->Draw();

		MainWindow.display();
	}

	return 0;
}
