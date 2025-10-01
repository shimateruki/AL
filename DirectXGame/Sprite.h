#pragma once
class Sprite 
{
public:
	void lib();
	void Update();
	void Draw();

private:
	int x_;
	int y_;
	int width_;
	int height_;
	int textureHandle_;

};
