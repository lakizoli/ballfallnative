#pragma once

#include "management/game.h"
#include "menuscene.h"

struct GameState {
	int highScore;

	GameState ();
	const GameState& operator << (istream& stream);
};

inline static ostream& operator << (ostream& stream, const GameState& state) {
	stream << "{\"highScore\":" << state.highScore << "}";
	return stream;
}

class BallFallGame : public Game {
	GameState _state;
	int _musicID;

public:
	BallFallGame (IUtil& util, IContentManager& contentManager);

	virtual void Init (int width, int height) override;
	virtual void Shutdown () override;

	const GameState& State () const {
		return _state;
	}

	GameState& State () {
		return _state;
	}

	void ReadGameState ();
	void SaveGameState ();
};
