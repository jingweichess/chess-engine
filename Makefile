CHESS_BITBOARDS = "src/chess/bitboards/inbetween.cpp" "src/chess/bitboards/infront.cpp" "src/chess/bitboards/magics.cpp" "src/chess/bitboards/passedpawn.cpp"

CHESS_BOARD = "src/chess/board/board.cpp"

CHESS_COMM = "src/chess/comm/xboard.cpp" "src/chess/comm/xboard/xboardsearchanalyzereventhandler.cpp"

CHESS_ENDGAME = "src/chess/endgame/endgame.cpp"

CHESS_EVAL = "src/chess/eval/constructor.cpp" "src/chess/eval/evaluator.cpp" "src/chess/eval/parameters.cpp"

CHESS_HASH = "src/chess/hash/hash.cpp" "src/chess/hash/chesshashtable.cpp"

CHESS_PLAYER = "src/chess/player/player.cpp"

CHESS_SEARCH = "src/chess/search/chesspv.cpp" "src/chess/search/searcher.cpp"

CHESS_TYPES = "src/chess/types/square.cpp"

GAME_CLOCK = "src/game/clock/clock.cpp"

GAME_PERSONALITY = "src/game/personality/personality.cpp"

GAME_SEARCH = "src/game/search/aspiration.cpp" "src/game/search/hashtable.cpp"

ENGINE = "jing-wei/engine.cpp"

ENGINE_FILES = $(ENGINE) $(CHESS_BITBOARDS) $(CHESS_BOARD) $(CHESS_COMM) $(CHESS_ENDGAME) $(CHESS_EVAL) $(CHESS_HASH) $(CHESS_PLAYER) $(CHESS_SEARCH) $(CHESS_TYPES) $(GAME_CLOCK) $(GAME_PERSONALITY) $(GAME_SEARCH)

LEVEL_IN_SECONDS = 1
NODE_COUNT = 100000
TIME_IN_SECONDS = 5

MINUTES = 0
SECONDS = $(LEVEL_IN_SECONDS)
INTERVAL = 0.01

LEVEL = 0/$(MINUTES):$(SECONDS)+$(INTERVAL)

OPPONENT = 4ku
OPPONENT_DEPTH = 7

ELO0 = 0.5
ELO1 = 2.5

SPRT = elo0=$(ELO0) elo1=$(ELO1) alpha=0.05 beta=0.05
RESIGN = 
#-resign movecount=3 score=500 twosided=true

all: compile

compile:

	mkdir -p bin

	g++-12 -o bin/jing-wei $(ENGINE_FILES) -std=c++20 -DUSE_M128I -D__BMI__ -DNDEBUG -O3 -m64 -mbmi2 -mpopcnt -msse4.2 -march=native -flto=4 -s

install:

	wget https://github.com/cutechess/cutechess/releases/download/v1.3.1/cutechess_20230730+1.3.1-1_amd64.deb -O /tmp/cutechess-cli.deb
	sudo dpkg -i /tmp/cutechess-cli.deb

4ku:

	cutechess-cli -engine conf=4ku -engine conf=4ku-old -each tc=0/0:$(LEVEL_IN_SECONDS) timemargin=100000 restart=on -openings file="bin/data/openings.epd" format=epd order=random -concurrency 6 -sprt $(SPRT) -rounds 5000000 -games 2 -repeat -ratinginterval 1 $(RESIGN)

personality: compile

	cutechess-cli -engine name=JingWeiExperimental cmd="./jing-wei" initstr="personality data/personality.txt" -engine name=JingWeiControl cmd="./jing-wei-old" -each proto=xboard tc=$(LEVEL) dir="./bin" timemargin=100000 restart=on -openings file="bin/data/openings.epd" format=epd order=random -concurrency 6 -sprt $(SPRT) -rounds 5000000 -games 2 -repeat -ratinginterval 1 $(RESIGN)

opponents-same: compile

	cutechess-cli -engine name=JingWeiExperimental cmd="./jing-wei" proto=xboard initstr="personality data/personality.txt" dir="./bin" -engine conf=$(OPPONENT) -each tc=0/0:$(LEVEL_IN_SECONDS) timemargin=100000 restart=on -openings file="bin/data/openings.epd" format=epd order=random -concurrency 6 -sprt $(SPRT) -rounds 5000000 -games 2 -repeat -ratinginterval 1 $(RESIGN)

opponents-sprt: compile

	cutechess-cli -engine name=JingWeiExperimental cmd="./jing-wei" proto=xboard initstr="personality data/personality.txt" dir="./bin" tc=$(LEVEL) -engine conf=$(OPPONENT) tc=0/0:1+0.01 -each timemargin=100000 restart=on -openings file="bin/data/openings.epd" format=epd order=random -concurrency 6 -sprt $(SPRT) -rounds 5000000 -games 2 -repeat -ratinginterval 1 $(RESIGN)

opponents-test: compile

	cutechess-cli -engine name=JingWeiExperimental cmd="./jing-wei" proto=xboard initstr="personality data/personality.txt" dir="./bin" tc=$(LEVEL) -engine conf=$(OPPONENT) tc=0/0:1+0.01 -each timemargin=100000 restart=on -openings file="bin/data/openings.epd" format=epd order=random -concurrency 6 -rounds 5000000 -games 2 -repeat -ratinginterval 1 $(RESIGN)

sprt-nodes: compile

	cutechess-cli -engine name=JingWeiExperimental cmd="./jing-wei" initstr="personality data/personality.txt" -engine name=JingWeiControl cmd="./jing-wei-old" -each proto=xboard st=10 nodes=$(NODE_COUNT) dir="./bin" timemargin=100000 book="bin/data/varied.bin" bookdepth=12 restart=on -recover -concurrency 12 -sprt $(SPRT) -rounds 5000000 -games 2 -repeat -ratinginterval 1 $(RESIGN)

sprt-level: compile

	cutechess-cli -engine name=JingWeiExperimental cmd="./jing-wei" initstr="personality data/personality.txt" -engine name=JingWeiControl cmd="./jing-wei-old" -each proto=xboard tc=$(LEVEL) dir="./bin" timemargin=100000 restart=on -openings file="bin/data/openings.epd" format=epd order=random -concurrency 6 -sprt $(SPRT) -rounds 5000000 -games 2 -repeat -ratinginterval 1 $(RESIGN)

sprt-time: compile

	cutechess-cli -engine name=JingWeiExperimental cmd="./jing-wei" initstr="personality data/personality.txt" -engine name=JingWeiControl cmd="./jing-wei-old" -each proto=xboard st=$(TIME_IN_SECONDS) dir="./bin" timemargin=100000 restart=on -openings file="bin/data/openings.epd" format=epd order=random -concurrency 6 -sprt $(SPRT) -rounds 5000000 -games 2 -repeat -ratinginterval 1 $(RESIGN)

test: compile

	cutechess-cli -engine name=JingWeiExperimental cmd="./jing-wei" initstr="personality data/personality.txt" -engine name=JingWeiControl cmd="./jing-wei-old" -each proto=xboard tc=$(LEVEL) dir="./bin" timemargin=100000 restart=on -openings file="bin/data/openings.epd" format=epd order=random -concurrency 6 -rounds 5000000 -games 2 -repeat -ratinginterval 1 $(RESIGN)

test-data-gen: compile

	cutechess-cli -engine name=JingWeiExperimental cmd="./jing-wei" initstr="personality data/personality.txt" -engine name=JingWeiControl cmd="./jing-wei-old" -each proto=xboard tc=$(LEVEL) dir="./bin" timemargin=100000 restart=on -openings file="bin/data/openings.epd" format=epd order=random -concurrency 6 -rounds 5000000 -games 2 -repeat -ratinginterval 1

update-old: compile

	rm ./bin/jing-wei-old
	cp ./bin/jing-wei ./bin/jing-wei-old
