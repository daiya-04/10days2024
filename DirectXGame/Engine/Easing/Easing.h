#pragma once
#include"Vec2.h"
#include"Vec3.h"

//イージング関数チートシートサイトURL(https://easings.net/ja)

class Easing {
public:

	enum class EaseName
	{

		EaseInSine,
		EaseOutSine,
		EaseInOutSine,
		EaseInQuad,
		EaseOutQuad,
		EaseInOutQuad,
		EaseInCubic,
		EaseOutCubic,
		EaseInOutCubic,
		EaseInQuart,
		EaseOutQuart,
		EaseInOutQuart,
		EaseInQuint,
		EaseOutQuint,
		EaseInOutQuint,
		EaseInExpo,
		EaseOutExpo,
		EaseInOutExpo,
		EaseInCirc,
		EaseOutCirc,
		EaseInOutCirc,
		EaseInBack,
		EaseOutBack,
		EaseInOutBack,
		EaseInElastic,
		EaseOutElastic,
		EaseInOutElastic,
		EaseInBounce,
		EaseOutBounce,
		EaseInOutBounce,
		Lerp

	};

private:
	//1
	static float easeInSine(float x);
	//2
	static float easeOutSine(float x);
	//3
	static float easeInOutSine(float x);
	//4
	static float easeInQuad(float x);
	//5
	static float easeOutQuad(float x);
	//6
	static float easeInOutQuad(float x);
	//7
	static float easeInCubic(float x);
	//8
	static float easeOutCubic(float x);
	//9
	static float easeInOutCubic(float x);
	//10
	static float easeInQuart(float x);
	//11
	static float easeOutQuart(float x);
	//12
	static float easeInOutQuart(float x);
	//13
	static float easeInQuint(float x);
	//14
	static float easeOutQuint(float x);
	//15
	static float easeInOutQuint(float x);
	//16
	static float easeInExpo(float x);
	//17
	static float easeOutExpo(float x);
	//18
	static float easeInOutExpo(float x);
	//19
	static float easeInCirc(float x);
	//20
	static float easeOutCirc(float x);
	//21
	static float easeInOutCirc(float x);
	//22
	static float easeInBack(float x);
	//23
	static float easeOutBack(float x);
	//24
	static float easeInOutBack(float x);
	//25
	static float easeInElastic(float x);
	//26
	static float easeOutElastic(float x);
	//27
	static float easeInOutElastic(float x);
	//28
	static float easeInBounce(float x);
	//29
	static float easeOutBounce(float x);
	//30
	static float easeInOutBounce(float x);

	static float SetEaseT(Easing::EaseName name, float t);

public:

	//float
	static float Ease(Easing::EaseName name, const float start, const float end, float t);
	//Vector2
	static Vector2 Ease(Easing::EaseName name, const Vector2& start, const Vector2& end, float t);
	//Vector3
	static Vector3 Ease(Easing::EaseName name, const Vector3& start, const Vector3& end, float t);

};
