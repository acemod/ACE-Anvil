class ACE_Drawing
{
    static ref Shape DrawBounds(IEntity entity)
    {
	vector minBounds;
	vector maxBounds;
	entity.GetBounds(minBounds, maxBounds);

	float diffX = maxBounds[0] - minBounds[0];
	float diffZ = maxBounds[1] - minBounds[1];
	float diffY = maxBounds[2] - minBounds[2];

	vector tmp;
	vector pps[10];

	tmp = { 0, 0, 0 };
	pps[0] = minBounds + tmp;

	tmp = { diffX, 0, 0 };
	pps[1] = minBounds + tmp;

	tmp = { diffX, diffZ, 0 };
	pps[2] = minBounds + tmp;

	tmp = { 0, diffZ, 0 };
	pps[3] = minBounds + tmp;

	tmp = { 0, 0, 0 };
	pps[4] = minBounds + tmp;

	tmp = { 0, 0, diffY };
	pps[5] = minBounds + tmp;

	tmp = { diffX, 0, diffY };
	pps[6] = minBounds + tmp;

	tmp = { diffX, diffZ, diffY };
	pps[7] = minBounds + tmp;

	tmp = { 0, diffZ, diffY };
	pps[8] = minBounds + tmp;

	tmp = { 0, 0, diffY };
	pps[9] = minBounds + tmp;

	vector matrix[4];
	entity.GetWorldTransform(matrix);

	for (int i = 0; i < 10; i++) { pps[i] = pps[i].Multiply4(matrix); }

	return Shape.CreateLines(0xff0000, ShapeFlags.DEFAULT, pps, 10);
    }

    static array<ref Shape> DrawDamageSphere(vector origin, int count, ACE_DrawingDamageFunction damageFunction)
    {
	float goldenRatio = 1.618;
	int N = count / 2;
	int counter = 0;
	array<ref Shape> shapes = {};

	for (int n = N * -1; n <= N; n++)
	{
	    float phi = 2 * Math.PI * n / goldenRatio;
	    float theta = Math.PI / 2 + Math.Asin(2 * n / (2 * N + 1));
	    float radius = 0.5;

	    while (true)
	    {
		vector relPoint = { radius * Math.Sin(theta) * Math.Cos(phi), radius * Math.Sin(theta) * Math.Sin(phi), radius * Math.Cos(theta) };
		vector point = relPoint + origin;

		float damage = damageFunction.CalculateDamage(origin, point);
		if (damage <= 0)
		{
		    break;
		}

		vector pps[] = { point - relPoint * 0.5, point };
		int red = Math.Round(Math.Min(1, damage / 100) * 255.0);
		int green = Math.Round((1 - Math.Min(1, damage / 100)) * 255.0);
		int alpha = Math.Round(Math.Pow(Math.Min(1, damage / 100), 2) * 255.0);
		int color = (alpha << 24) + (red << 16) + (green << 8);
		shapes.Insert(Shape.CreateLines(color, ShapeFlags.TRANSP, pps, 2));

		radius += 0.5;

		counter++;
	    }
	}

	return shapes;
    }
}

class ACE_DrawingDamageFunction
{
    float CalculateDamage(vector origin, vector target)
    {
	return 0;
    }
}
