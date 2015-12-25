#pragma once

#include "management/game.h"
#include "menuscene.h"

struct GameState {
	int highScore;

	GameState () : highScore (0) {}

	const GameState& operator << (istream& stream) {
		//TODO: implement a general purpose JSON serializer/deserializer into base code...

		bool foundEnd = false;
		while (!foundEnd && stream.good ()) {
			char ch = 0;
			stream.read (&ch, 1);

			if (stream.good ()) {
				switch (ch) {
				case '}':
					foundEnd = true;
					break;
				case ':':
					stream >> highScore;
					break;
				default:
					break;
				}
			}
		}
		return *this;
	}
};

inline static ostream& operator << (ostream& stream, const GameState& state) {
	stream << "{\"highScore\":" << state.highScore << "}";
	return stream;
}

class BallFallGame : public Game {
	GameState _state;

public:
	BallFallGame (IUtil& util, IContentManager& contentManager) : Game (util, contentManager) {}

	virtual void Init (int width, int height) override {
		Game::Init (width, height);
		ReadGameState ();
		SetCurrentScene (shared_ptr<Scene> (new MenuScene ()));
	}

	const GameState& State () const {
		return _state;
	}

	GameState& State () {
		return _state;
	}

	void ReadGameState () {
		istringstream ss (ContentManager ().ReadFile ("state.save"));
		_state << ss;
	}

	void SaveGameState () {
		stringstream ss;
		ss << _state;
		ContentManager ().WriteFile ("state.save", ss.str ());
	}
};
