#pragma once
class SimpleAerodynamics
{
public:
	static float LiftCoefficient(float angle, int flaps = 0)
	{
		float clf0[9] = { -0.54f, -0.2f, 0.2f, 0.57f, 0.92f, 1.21f, 1.43f, 1.4f,
						 1.0f };
		float clfd[9] = { 0.0f, 0.45f, 0.85f, 1.02f, 1.39f, 1.65f, 1.75f, 1.38f,
						 1.17f };
		float clfu[9] = { -0.74f, -0.4f, 0.0f, 0.27f, 0.63f, 0.92f, 1.03f, 1.1f,
						 0.78f };
		float a[9] = { -8.0f, -4.0f, 0.0f, 4.0f, 8.0f, 12.0f, 16.0f, 20.0f,
						   24.0f };
		float cl;
		int   i;

		cl = 0;
		for (i = 0; i < 8; i++)
		{
			if ((a[i] <= angle) && (a[i + 1] > angle))
			{
				switch (flaps)
				{
				case 0:// flaps not deflected
					cl = clf0[i] - (a[i] - angle) * (clf0[i] - clf0[i + 1]) /
						(a[i] - a[i + 1]);
					break;
				case -1: // flaps down
					cl = clfd[i] - (a[i] - angle) * (clfd[i] - clfd[i + 1]) /
						(a[i] - a[i + 1]);
					break;
				case 1: // flaps up
					cl = clfu[i] - (a[i] - angle) * (clfu[i] - clfu[i + 1]) /
						(a[i] - a[i + 1]);
					break;
				}
				break;
			}
		}

		return cl;
	}

	//------------------------------------------------------------------------//
	//  Given the attack angle and the status of the flaps, this function
	//  returns the appropriate drag coefficient for a cambered airfoil with
	//  a plain trailing-edge flap (+/- 15 degree deflection).
	//------------------------------------------------------------------------//
	static float DragCoefficient(float angle, int flaps = 0)
	{
		float cdf0[9] = { 0.01f, 0.0074f, 0.004f, 0.009f, 0.013f, 0.023f, 0.05f,
						 0.12f, 0.21f };
		float cdfd[9] = { 0.0065f, 0.0043f, 0.0055f, 0.0153f, 0.0221f, 0.0391f, 0.1f,
						 0.195f, 0.3f };
		float cdfu[9] = { 0.005f, 0.0043f, 0.0055f, 0.02601f, 0.03757f, 0.06647f,
						 0.13f, 0.18f, 0.25f };
		float a[9] = { -8.0f, -4.0f, 0.0f, 4.0f, 8.0f, 12.0f, 16.0f, 20.0f,
						   24.0f };
		float cd;
		int   i;

		cd = 0.5;
		for (i = 0; i < 8; i++)
		{
			if ((a[i] <= angle) && (a[i + 1] > angle))
			{
				switch (flaps)
				{
				case 0:// flaps not deflected
					cd = cdf0[i] - (a[i] - angle) * (cdf0[i] - cdf0[i + 1]) /
						(a[i] - a[i + 1]);
					break;
				case -1: // flaps down
					cd = cdfd[i] - (a[i] - angle) * (cdfd[i] - cdfd[i + 1]) /
						(a[i] - a[i + 1]);
					break;
				case 1: // flaps up
					cd = cdfu[i] - (a[i] - angle) * (cdfu[i] - cdfu[i + 1]) /
						(a[i] - a[i + 1]);
					break;
				}
				break;
			}
		}

		return cd;
	}
	static float getControlCurve(float angle, float speed, float junction = 80)
	{
		float angleScale = exp(-angle * angle / (35 * 35));
		float result;
		if(speed< junction)
		{
			result = speed * speed;
		}
		else
		{
			const float b = 1 / (junction * junction * junction);
			result =  1 / (b * speed);
		}
		result *= angleScale;
		return result;
	}
};