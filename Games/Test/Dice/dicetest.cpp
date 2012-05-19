
#include "gutil_dice.h"
#include <stdio.h>
USING_NAMESPACE_GUTIL1(Games);

void test_basics();
void test_uniformity();

int main(int, char **)
{
	test_basics();
	test_uniformity();

	return 0;
}


void test_basics()
{
	Dice d(6);
	d.Roll();
	printf("Rolled %d, %d, %d, %d, %d and %d\n\n", d.Inspect()[0], d.Inspect()[1], d.Inspect()[2], d.Inspect()[3], d.Inspect()[4], d.Inspect()[5]);
}


void test_uniformity()
{
	const int num_tries( 1000000 );
	Dice d(10);
	int roll_count(0);
	int counts[] = {0, 0, 0, 0, 0, 0};
	double percentages[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

	do
	{
		d.Roll();
		for(int i = 0; i < d.GetNumberDice(); ++i)
			counts[d.Inspect()[i] - 1]++;
		roll_count += d.GetNumberDice();
	}while(roll_count < num_tries);

	for(int i = 0; i < 6; ++i){
		percentages[i] = static_cast<double>(counts[i]) / num_tries;
		printf("%d: %f\n", i + 1, percentages[i]);
	}

}


