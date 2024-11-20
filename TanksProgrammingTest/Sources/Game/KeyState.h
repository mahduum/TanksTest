#pragma once
#include "Engine.h"
#include <array>

enum class KeyEventType : uint8
{
	None,		// Key did not generate an event this tick and is not being held
	Actuated,	// Key has generated an event this tick
	Held,		// Key generated no event, but is in a held state and wants to continue applying modifiers and triggers
};

struct KeyState
{
	/*Raw value reported by the device*/
	Vector2 RawValue;
	/*Final value after processing*/
	Vector2 Value;
	float LastUpDownTransitionTime;
	bool bDown;
	bool bDownPrevious;
	bool bConsumed;

	/* How many samples contributed to RawValueAccumulator. Used for smoothing operations, e.g. mouse */
	short SampleCountAccumulator;

	/* How many of each event type had been received when input was last processed. */
	std::array<int, 5> EventCounts;
	
	/* Used to accumulate events during the frame and flushed when processed.*/
	std::array<int, 5> EventAccumulator;

	/* Used to accumulate input values during the frame and flushed after processing. */
	Vector2 RawValueAccumulator;
};