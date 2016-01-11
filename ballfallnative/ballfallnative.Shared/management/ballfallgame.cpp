#include "pch.h"
#include "ballfallgame.h"

GameState::GameState () :
	highScore (0) {
}

const GameState& GameState::operator << (istream& stream) {
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

BallFallGame::BallFallGame (IUtil & util, IContentManager & contentManager) :
	Game (util, contentManager),
	_musicID (0) {
}

void BallFallGame::Init (int width, int height) {
	Game::Init (width, height);
	ReadGameState ();

	IContentManager& contentManager = Game::ContentManager ();
	_musicID = contentManager.LoadSound ("whistleblower.ogg");
	contentManager.PlaySound (_musicID, 0.7f, true);

	SetCurrentScene (shared_ptr<Scene> (new MenuScene ()));
}

void BallFallGame::Shutdown () {
	Game::Shutdown ();

	IContentManager& contentManager = Game::ContentManager ();
	contentManager.StopSound (_musicID);
	contentManager.UnloadSound (_musicID);
	_musicID = 0;
}

void BallFallGame::ReadGameState () {
	istringstream ss (ContentManager ().ReadFile ("state.save"));
	_state << ss;
}

void BallFallGame::SaveGameState () {
	stringstream ss;
	ss << _state;
	ContentManager ().WriteFile ("state.save", ss.str ());
}
