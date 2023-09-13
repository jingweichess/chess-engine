CHESS_BITBOARDS = "src/chess/bitboards/inbetween.cpp" "src/chess/bitboards/infront.cpp" "src/chess/bitboards/magics.cpp" "src/chess/bitboards/passedpawn.cpp"

CHESS_BOARD = "src/chess/board/board.cpp"

CHESS_COMM = "src/chess/comm/xboard.cpp" "src/chess/comm/xboard/xboardsearchanalyzereventhandler.cpp"

CHESS_ENDGAME = "src/chess/endgame/endgame.cpp"

CHESS_EVAL = "src/chess/eval/constructor.cpp" "src/chess/eval/evaluator.cpp" "src/chess/eval/parameters.cpp"

CHESS_HASH = "src/chess/hash/hash.cpp" "src/chess/hash/chesshashtable.cpp"

CHESS_PLAYER = "src/chess/player/player.cpp"

CHESS_SEARCH = "src/chess/search/chesspv.cpp" "src/chess/search/movehistory.cpp" "src/chess/search/searcher.cpp"

CHESS_TYPES = "src/chess/types/square.cpp"

GAME_CLOCK = "src/game/clock/clock.cpp"

GAME_PERSONALITY = "src/game/personality/personality.cpp"

GAME_SEARCH = "src/game/search/aspiration.cpp" "src/game/search/hashtable.cpp"

ENGINE = "jing-wei/engine.cpp"

ENGINE_FILES = $(ENGINE) $(CHESS_BITBOARDS) $(CHESS_BOARD) $(CHESS_COMM) $(CHESS_ENDGAME) $(CHESS_EVAL) $(CHESS_HASH) $(CHESS_PLAYER) $(CHESS_SEARCH) $(CHESS_TYPES) $(GAME_CLOCK) $(GAME_PERSONALITY) $(GAME_SEARCH)

all: compile

compile:

	mkdir -p bin

	g++-12 -o bin/jing-wei $(ENGINE_FILES) -std=c++20 -DUSE_M128I -D__BMI__ -DNDEBUG -O3 -m64 -mbmi2 -mpopcnt -msse4.2 -march=native -flto=4

install:

	wget https://github.com/cutechess/cutechess/releases/download/1.2.0/cutechess_20200809+1.2.0+1.2.0-1_amd64.deb -O /tmp/cutechess-cli.deb
	sudo dpkg -i /tmp/cutechess-cli.deb

sprt: compile

	cutechess-cli -engine name=JingWeiExperimental cmd="./jing-wei" initstr="personality data/personality.txt" -engine name=JingWeiControl cmd="./jing-wei-old" -each proto=xboard st=10 nodes=100000 dir="./bin" timemargin=100000 book="bin/data/varied.bin" bookdepth=12 restart=on -recover -concurrency 12 -sprt elo0=0.5 elo1=1.0 alpha=0.05 beta=0.05 -rounds 5000000 -games 2 -repeat -ratinginterval 1

sprt-fast: compile

	cutechess-cli -engine name=JingWeiExperimental cmd="./jing-wei" initstr="personality data/personality.txt" -engine name=JingWeiControl cmd="./jing-wei-old" -each proto=xboard st=10 nodes=2048 dir="./bin" timemargin=100000 book="bin/data/varied.bin" bookdepth=12 restart=on -recover -concurrency 24 -sprt elo0=0.5 elo1=1.0 alpha=0.05 beta=0.05 -rounds 5000000 -games 2 -repeat -ratinginterval 1000

sprt-long: compile

	cutechess-cli -engine name=JingWeiExperimental cmd="./jing-wei" initstr="personality data/personality.txt" -engine name=JingWeiControl cmd="./jing-wei-old" -each proto=xboard tc=0/0:5+0 dir="./bin" timemargin=100000 book="./bin/data/varied.bin" bookdepth=12 restart=on -recover -concurrency 6 -sprt elo0=0.5 elo1=1.0 alpha=0.05 beta=0.05 -rounds 5000000 -games 2 -repeat -ratinginterval 1 -pgnout "./bin/data/test.pgn"
