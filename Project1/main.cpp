#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>

using std::vector;
sf::Vector2u size;
vector<sf::RectangleShape> rectangles;
sf::RenderWindow externalWindow;

class Player
{

public:
	float position[2];
	Player(float pos[2] = {})
	{
		if (!pos)
		{
			position[0] = size.x / 2.f;
			position[1] = size.y / 2.f;
		}
		else
		{
			position[0] = pos[0];
			position[1] = pos[1];
		}
	}
	void move(float movement[2]);
	sf::CircleShape shape(float radius = 5, sf::Color color = sf::Color(100, 100, 100, 255));
};


sf::VertexArray getLine(sf::Vector2f first, sf::Vector2f second, sf::Color color = sf::Color::Red)
{	
	sf::VertexArray line(sf::LineStrip, 2);
	line[0].position = first;
	line[1].position = second;
	return line;
}

float getDistance(float first[2], float second[2])
{
	return sqrt(pow(first[0] - second[0], 2) + pow(first[1] - second[1], 2));
}

float getDistance(sf::Vector2f first, sf::Vector2f second)
{
	return sqrt(pow(first.x - second.x, 2) + pow(first.y - second.y, 2));
}

float getAngle(float first[2], float second[2], float third[2])
{
	float a = getDistance(first, third);
	float b = getDistance(first, second);
	float c = getDistance(second, third);
	float angle = acos((pow(b, 2) + pow(c, 2) - pow(a, 2)) / (2 * b*c));
	return angle;
}

float getAngle(sf::Vector2f first, sf::Vector2f second, sf::Vector2f third)
{
	float c = getDistance(first, third);
	float b = getDistance(first, second);
	float a = getDistance(second, third);
	float ratio = (pow(b, 2) + pow(a, 2) - pow(c, 2)) / (2 * b*a);
	float angle = acos(ratio);
	return angle;
}

vector<vector<float>> getVertices(sf::RectangleShape shape)
{
	sf::Vector2f position = shape.getPosition();
	sf::Vector2f size = shape.getSize();
	vector<vector<float>> ret;
	ret.push_back(vector<float>(vector<float>({ position.x, position.y }))); //UpLeft
	ret.push_back(vector<float>(vector<float>({ position.x + size.x, position.y }))); //UpRight
	ret.push_back(vector<float>(vector<float>({ position.x, position.y + size.y }))); //DownLeft
	ret.push_back(vector<float>(vector<float>({ position.x + size.x, position.y + size.y }))); //DownRight
	return ret;
};

vector<sf::Vector2f> getsfVectorVertices(sf::RectangleShape shape)
{
	sf::Vector2f position = shape.getPosition();
	sf::Vector2f size = shape.getSize();
	vector<sf::Vector2f> ret;
	ret.push_back(sf::Vector2f(position.x, position.y)); //UpLeft
	ret.push_back(sf::Vector2f(position.x + size.x, position.y )); //UpRight
	ret.push_back(sf::Vector2f(position.x, position.y + size.y )); //DownLeft
	ret.push_back(sf::Vector2f(position.x + size.x, position.y + size.y )); //DownRight
	return ret;
}

sf::Vector2f getMiddlePosition(sf::RectangleShape shape)
{
	return sf::Vector2f(shape.getSize().x / 2 + shape.getPosition().x, shape.getSize().y / 2 + shape.getPosition().y);
}

sf::Vector2f getMiddlePosition(sf::CircleShape shape)
{
	return sf::Vector2f(shape.getRadius() + shape.getPosition().x, shape.getRadius() + shape.getPosition().y);
}

float * vector2floatptr(vector<float> vec)
{
	float ret[2] = { vec[0], vec[1] };
	return ret;
}

float * sfVector2f2floatptr(sf::Vector2f vec)
{
	float ret[] = { vec.x, vec.y };
	return ret;
}

vector<sf::Vector2f> pointsWithBiggestAngle(sf::RectangleShape shape, Player player)
{
	vector<sf::Vector2f> vertices = getsfVectorVertices(shape);
	sf::Vector2f middle = getMiddlePosition(shape);
	sf::Vector2f rootPosition = getMiddlePosition(player.shape());
	vector <sf::Vector2f> ret;
	float biggestYet = 0.f;
	//Finding the biggest angle from all pairs, this is slow, improve by separating left and right and I am pretty sure you won't understand it later, sorry
	for (int i = 0; i < 4; i++)
	{
		for (int ii = 0; ii < 4; ii++)
		{
			float current = getAngle(vertices[ii], rootPosition, vertices[i]);
			if (current > biggestYet)
			{
				biggestYet = current;
				ret.clear();
				ret.push_back(vertices[i]);
				ret.push_back(vertices[ii]);
			}
		}
	}
	return ret;
}


void setMiddlePosition(sf::RectangleShape &shape, sf::Vector2f position)
{
	shape.setPosition(sf::Vector2f(position.x - shape.getSize().x, position.y - shape.getSize().y));
}

void Player::move(float movement[2])
{
	position[0] += movement[0];
	position[1] += movement[1];
	vector<sf::Vector2f> vertices;
	vertices = getsfVectorVertices(rectangles[0]);
	sf::Vector2f x(position[0], position[1]);
}

sf::Vector2f halfLinePoint(sf::Vector2f start, sf::Vector2f direction, float length)
{
	float distance = getDistance(start, direction);
	float xratio = (direction.x - start.x) / distance;
	float yratio = (direction.y - start.y) / distance;
	sf::Vector2f ret = sf::Vector2f(length*xratio + start.x, length*yratio + start.y);
	return ret;
}


sf::CircleShape Player::shape(float radius, sf::Color color)
{

	sf::CircleShape ret = sf::CircleShape(radius);
	ret.setPosition(sf::Vector2f(position[0]-radius, position[1]-radius));
	return ret;
}

sf::ConvexShape getShadeTriangle(sf::Vector2f first, sf::Vector2f second, sf::Vector2f playerPosition, float length)
{
	sf::ConvexShape ret = sf::ConvexShape(4);
	ret.setPoint(0, first);
	ret.setPoint(1, second);
	ret.setPoint(2, halfLinePoint(playerPosition, second, length));
	ret.setPoint(3, halfLinePoint(playerPosition, first, length));
	ret.setFillColor(sf::Color(0, 0, 0, 255));
	return ret;
}

int main()
{
	{
		rectangles.push_back(sf::RectangleShape(sf::Vector2f(50, 50)));
		rectangles[0].setFillColor(sf::Color(200, 200, 200, 255));
		setMiddlePosition(rectangles[0], sf::Vector2f(400, 200));

		rectangles.push_back(sf::RectangleShape(sf::Vector2f(50, 50)));
		rectangles[1].setFillColor(sf::Color(200, 200, 200, 255));
		setMiddlePosition(rectangles[1], sf::Vector2f(200, 300));
		
		rectangles.push_back(sf::RectangleShape(sf::Vector2f(300, 50)));
		rectangles[2].setFillColor(sf::Color(200, 200, 200, 255));
		setMiddlePosition(rectangles[2], sf::Vector2f(400, 500));
	
		rectangles.push_back(sf::RectangleShape(sf::Vector2f(300, 50)));
		rectangles[3].setFillColor(sf::Color(200, 200, 200, 255));
		setMiddlePosition(rectangles[2], sf::Vector2f(400, 500));

		rectangles.push_back(sf::RectangleShape(sf::Vector2f(300, 50)));
		rectangles[4].setFillColor(sf::Color(200, 200, 200, 255));
		setMiddlePosition(rectangles[2], sf::Vector2f(400, 500));

	}
	
	float speed = 5;
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
	window.setFramerateLimit(60);
	size = window.getSize();
	sf::RectangleShape ground(sf::Vector2f(size.x-100, size.y-100));
	Player player = Player();

	ground.setFillColor(sf::Color(100,100,100,255));
	ground.setPosition(50, 50);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		//Controls
		float movement[2] = { 0,0 };
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) movement[0] -= speed;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) movement[0] += speed;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) movement[1] -= speed;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) movement[1] += speed;

		player.move(movement);
		//end
		window.clear();
		window.draw(ground);

		//Drawing best angle lines
		//for (int i = 0; i < rectangles.size(); i++)
		//{
		//	vector<sf::Vector2f> points = pointsWithBiggestAngle(rectangles[i], player);
		//	getLine(points[0], points[1]);

		//}

		//drawing objects
		for (unsigned i = 0; i < rectangles.size(); i++)
		{
			vector<sf::Vector2f> points = pointsWithBiggestAngle(rectangles[i], player);
			window.draw(getShadeTriangle(points[0], points[1], getMiddlePosition(player.shape()), 10000));
			window.draw(rectangles[i]);
		}
		window.draw(player.shape());
		window.display();
	}

	return 0;
}

