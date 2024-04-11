//------------------------------------------------------------------------------------------------
enum ACE_ESettingsSource
{
	DEFAULT,
	CLIENT,
	MISSION, // Not yet implemented: Should be configurable on the game mode entity
	SERVER,
	PRIORITY // Select it based on enforced settings
	// Order depending on depending on set priorities
	// | C | M | S |
	// +---+---+---+
	// | * | 0 | 0 | => C > M > S > D
	// | * | 1 | 0 | => M > C > S > D
	// | * | 0 | 1 | => S > C > M > D
	// | * | 1 | 1 | => M > S > C > D
	// | * | 1 | 2 | => S > M > C > D
	// Order for global settings depending on set priorities
	// | C | M | S |
	// +---+---+---+
	// | * | 0 | 0 | => M > S > D
	// | * | 1 | 0 | => M > S > D
	// | * | 0 | 1 | => S > M > D
	// | * | 1 | 1 | => M > S > D
	// | * | 1 | 2 | => S > M > D
}
