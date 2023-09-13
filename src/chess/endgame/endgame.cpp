/*
    Jing Wei, the rebirth of the chess engine I started in 2010
    Copyright(C) 2019-2023 Chris Florin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <cassert>
#include <cmath>

#include <map>

#include "../hash/hash.h"

#include "endgame.h"
#include "function.h"

#include "eval/kk.h"

#include "eval/kxk.h"

#include "eval/kxkx.h"
#include "eval/kxxk.h"

#include "eval/knxkx.h"
#include "eval/kbxkx.h"
#include "eval/krxkx.h"
#include "eval/kqxkx.h"

static std::map<std::string, ChessEndgame::EndgameFunctionType> chessEndgameMap = {
    { "K7/8/8/8/8/8/8/7k w - - 0 1", kk },
    { "k7/8/8/8/8/8/8/7K w - - 0 1", kk },

    { "KP6/8/8/8/8/8/8/7k w - - 0 1", kpk },
    { "kp6/8/8/8/8/8/8/7K w - - 0 1", kpk },
    { "KN6/8/8/8/8/8/8/7k w - - 0 1", knk },
    { "kn6/8/8/8/8/8/8/7K w - - 0 1", knk },
    { "KB6/8/8/8/8/8/8/7k w - - 0 1", kbk },
    { "kb6/8/8/8/8/8/8/7K w - - 0 1", kbk },
    { "KR6/8/8/8/8/8/8/7k w - - 0 1", krk },
    { "kr6/8/8/8/8/8/8/7K w - - 0 1", krk },
    { "KQ6/8/8/8/8/8/8/7k w - - 0 1", kqk },
    { "kq6/8/8/8/8/8/8/7K w - - 0 1", kqk },

    { "KN6/8/8/8/8/8/8/6pk w - - 0 1", knkp },
    { "kn6/8/8/8/8/8/8/6PK w - - 0 1", knkp },
    { "KN6/8/8/8/8/8/8/6nk w - - 0 1", knkn },
//    { "kn6/8/8/8/8/8/8/6NK w - - 0 1", knkn },

    { "KB6/8/8/8/8/8/8/6pk w - - 0 1", kbkp },
    { "kb6/8/8/8/8/8/8/6PK w - - 0 1", kbkp },
    { "KB6/8/8/8/8/8/8/6nk w - - 0 1", kbkn },
    { "kb6/8/8/8/8/8/8/6NK w - - 0 1", kbkn },
    { "KB6/8/8/8/8/8/8/6bk w - - 0 1", kbkb },
//    { "kb6/8/8/8/8/8/8/6BK w - - 0 1", kbkb },

    { "KR6/8/8/8/8/8/8/6pk w - - 0 1", krkp },
    { "kr6/8/8/8/8/8/8/6PK w - - 0 1", krkp },
    { "KR6/8/8/8/8/8/8/6nk w - - 0 1", krkn },
    { "kr6/8/8/8/8/8/8/6NK w - - 0 1", krkn },
    { "KR6/8/8/8/8/8/8/6bk w - - 0 1", krkb },
    { "kr6/8/8/8/8/8/8/6BK w - - 0 1", krkb },
    { "KR6/8/8/8/8/8/8/6rk w - - 0 1", krkr },
//    { "kr6/8/8/8/8/8/8/6RK w - - 0 1", krkr },

    { "KRP5/8/8/8/8/8/8/7k w - - 0 1", krpk },
    { "krp5/8/8/8/8/8/8/7K w - - 0 1", krpk },
    { "KRN5/8/8/8/8/8/8/7k w - - 0 1", krnk },
    { "krn5/8/8/8/8/8/8/7K w - - 0 1", krnk },
    { "KRB5/8/8/8/8/8/8/7k w - - 0 1", krbk },
    { "krb5/8/8/8/8/8/8/7K w - - 0 1", krbk },
    { "KRR5/8/8/8/8/8/8/7k w - - 0 1", krrk },
    { "krr5/8/8/8/8/8/8/7K w - - 0 1", krrk },

    { "KQ6/8/8/8/8/8/8/6pk w - - 0 1", kqkp },
    { "kq6/8/8/8/8/8/8/6PK w - - 0 1", kqkp },
    { "KQ6/8/8/8/8/8/8/6nk w - - 0 1", kqkn },
    { "kq6/8/8/8/8/8/8/6NK w - - 0 1", kqkn },
    { "KQ6/8/8/8/8/8/8/6bk w - - 0 1", kqkb },
    { "kq6/8/8/8/8/8/8/6BK w - - 0 1", kqkb },
    { "KQ6/8/8/8/8/8/8/6rk w - - 0 1", kqkr },
    { "kq6/8/8/8/8/8/8/6RK w - - 0 1", kqkr },
    { "KQ6/8/8/8/8/8/8/6qk w - - 0 1", kqkq },
//    { "kq6/8/8/8/8/8/8/6QK w - - 0 1", kqkq },

    { "KNP5/8/8/8/8/8/8/7k w - - 0 1", knpk },
    { "knp5/8/8/8/8/8/8/7K w - - 0 1", knpk },
    { "KNN5/8/8/8/8/8/8/7k w - - 0 1", knnk },
    { "knn5/8/8/8/8/8/8/7K w - - 0 1", knnk },

    { "KBP5/8/8/8/8/8/8/7k w - - 0 1", kbpk },
    { "kbp5/8/8/8/8/8/8/7K w - - 0 1", kbpk },
    { "KBN5/8/8/8/8/8/8/7k w - - 0 1", kbnk },
    { "kbn5/8/8/8/8/8/8/7K w - - 0 1", kbnk },
    { "KBB5/8/8/8/8/8/8/7k w - - 0 1", kbbk },
    { "kbb5/8/8/8/8/8/8/7K w - - 0 1", kbbk },

    { "KRP5/8/8/8/8/8/8/7k w - - 0 1", krpk },
    { "krp5/8/8/8/8/8/8/7K w - - 0 1", krpk },
    { "KRN5/8/8/8/8/8/8/7k w - - 0 1", krnk },
    { "krn5/8/8/8/8/8/8/7K w - - 0 1", krnk },
    { "KRB5/8/8/8/8/8/8/7k w - - 0 1", krbk },
    { "krb5/8/8/8/8/8/8/7K w - - 0 1", krbk },
    { "KRR5/8/8/8/8/8/8/7k w - - 0 1", krrk },
    { "krr5/8/8/8/8/8/8/7K w - - 0 1", krrk },

    { "KQP5/8/8/8/8/8/8/7k w - - 0 1", kqpk },
    { "kqp5/8/8/8/8/8/8/7K w - - 0 1", kqpk },
    { "KQN5/8/8/8/8/8/8/7k w - - 0 1", kqnk },
    { "kqn5/8/8/8/8/8/8/7K w - - 0 1", kqnk },
    { "KQB5/8/8/8/8/8/8/7k w - - 0 1", kqbk },
    { "kqb5/8/8/8/8/8/8/7K w - - 0 1", kqbk },
    { "KQR5/8/8/8/8/8/8/7k w - - 0 1", kqrk },
    { "kqr5/8/8/8/8/8/8/7K w - - 0 1", kqrk },
    { "KQQ5/8/8/8/8/8/8/7k w - - 0 1", kqqk },
    { "kqq5/8/8/8/8/8/8/7K w - - 0 1", kqqk },

    { "KNP5/8/8/8/8/8/8/6pk w - - 0 1", knpkp },
    { "knp5/8/8/8/8/8/8/6PK w - - 0 1", knpkp },
    { "KNP5/8/8/8/8/8/8/6nk w - - 0 1", knpkn },
    { "knp5/8/8/8/8/8/8/6NK w - - 0 1", knpkn },
    { "KNP5/8/8/8/8/8/8/6bk w - - 0 1", knpkb },
    { "knp5/8/8/8/8/8/8/6BK w - - 0 1", knpkb },

    { "KNN5/8/8/8/8/8/8/6pk w - - 0 1", knnkp },
    { "knn5/8/8/8/8/8/8/6PK w - - 0 1", knnkp },
    { "KNN5/8/8/8/8/8/8/6nk w - - 0 1", knnkn },
    { "knn5/8/8/8/8/8/8/6NK w - - 0 1", knnkn },
    { "KNN5/8/8/8/8/8/8/6bk w - - 0 1", knnkb },
    { "knn5/8/8/8/8/8/8/6BK w - - 0 1", knnkb },
    { "KNN5/8/8/8/8/8/8/6rk w - - 0 1", knnkr },
    { "knn5/8/8/8/8/8/8/6RK w - - 0 1", knnkr },

    { "KBB5/8/8/8/8/8/8/6pk w - - 0 1", kbbkp },
    { "kbb5/8/8/8/8/8/8/6PK w - - 0 1", kbbkp },
    { "KBB5/8/8/8/8/8/8/6nk w - - 0 1", kbbkn },
    { "kbb5/8/8/8/8/8/8/6NK w - - 0 1", kbbkn },
    { "KBB5/8/8/8/8/8/8/6bk w - - 0 1", kbbkb },
    { "kbb5/8/8/8/8/8/8/6BK w - - 0 1", kbbkb },
    { "KBB5/8/8/8/8/8/8/6rk w - - 0 1", kbbkr },
    { "kbb5/8/8/8/8/8/8/6RK w - - 0 1", kbbkr },

    { "KBB5/8/8/8/8/8/8/6pk w - - 0 1", kbbkp },
    { "kbb5/8/8/8/8/8/8/6PK w - - 0 1", kbbkp },
    { "KBB5/8/8/8/8/8/8/6nk w - - 0 1", kbbkn },
    { "kbb5/8/8/8/8/8/8/6NK w - - 0 1", kbbkn },
    { "KBB5/8/8/8/8/8/8/6bk w - - 0 1", kbbkb },
    { "kbb5/8/8/8/8/8/8/6BK w - - 0 1", kbbkb },
    { "KBB5/8/8/8/8/8/8/6rk w - - 0 1", kbbkr },
    { "kbb5/8/8/8/8/8/8/6RK w - - 0 1", kbbkr },

    { "KBP5/8/8/8/8/8/8/6pk w - - 0 1", kbpkp },
    { "kbp5/8/8/8/8/8/8/6PK w - - 0 1", kbpkp },
    { "KBP5/8/8/8/8/8/8/6nk w - - 0 1", kbpkn },
    { "kbp5/8/8/8/8/8/8/6NK w - - 0 1", kbpkn },
    { "KBP5/8/8/8/8/8/8/6bk w - - 0 1", kbpkb },
    { "kbp5/8/8/8/8/8/8/6BK w - - 0 1", kbpkb },
    { "KBP5/8/8/8/8/8/8/6rk w - - 0 1", kbpkr },
    { "kbp5/8/8/8/8/8/8/6RK w - - 0 1", kbpkr },

    { "KBPP4/8/8/8/8/8/8/7k w - - 0 1", kbppk },
    { "kbpp4/8/8/8/8/8/8/7K w - - 0 1", kbppk },

    { "KRP5/8/8/8/8/8/8/6pk w - - 0 1", krpkp },
    { "krp5/8/8/8/8/8/8/6PK w - - 0 1", krpkp },
    { "KRP5/8/8/8/8/8/8/6nk w - - 0 1", krpkn },
    { "krp5/8/8/8/8/8/8/6NK w - - 0 1", krpkn },
    { "KRP5/8/8/8/8/8/8/6bk w - - 0 1", krpkb },
    { "krp5/8/8/8/8/8/8/6BK w - - 0 1", krpkb },
    { "KRP5/8/8/8/8/8/8/6rk w - - 0 1", krpkr },
    { "krp5/8/8/8/8/8/8/6RK w - - 0 1", krpkr },

    { "KRN5/8/8/8/8/8/8/6pk w - - 0 1", krnkp },
    { "krn5/8/8/8/8/8/8/6PK w - - 0 1", krnkp },
    { "KRN5/8/8/8/8/8/8/6nk w - - 0 1", krnkn },
    { "krn5/8/8/8/8/8/8/6NK w - - 0 1", krnkn },
    { "KRN5/8/8/8/8/8/8/6bk w - - 0 1", krnkb },
    { "krn5/8/8/8/8/8/8/6BK w - - 0 1", krnkb },
    { "KRN5/8/8/8/8/8/8/6rk w - - 0 1", krnkr },
    { "krn5/8/8/8/8/8/8/6RK w - - 0 1", krnkr },

    { "KRB5/8/8/8/8/8/8/6pk w - - 0 1", krbkp },
    { "krb5/8/8/8/8/8/8/6PK w - - 0 1", krbkp },
    { "KRB5/8/8/8/8/8/8/6nk w - - 0 1", krbkn },
    { "krb5/8/8/8/8/8/8/6NK w - - 0 1", krbkn },
    { "KRB5/8/8/8/8/8/8/6bk w - - 0 1", krbkb },
    { "krb5/8/8/8/8/8/8/6BK w - - 0 1", krbkb },
    { "KRB5/8/8/8/8/8/8/6rk w - - 0 1", krbkr },
    { "krb5/8/8/8/8/8/8/6RK w - - 0 1", krbkr },

    { "KRRB4/8/8/8/8/8/8/5rrk w - - 0 1", krrbkrr },
    { "krrb4/8/8/8/8/8/8/5RRK w - - 0 1", krrbkrr },

    { "KRR5/8/8/8/8/8/8/6pk w - - 0 1", krrkp },
    { "krr5/8/8/8/8/8/8/6PK w - - 0 1", krrkp },
    { "KRR5/8/8/8/8/8/8/6nk w - - 0 1", krrkn },
    { "krr5/8/8/8/8/8/8/6NK w - - 0 1", krrkn },
    { "KRR5/8/8/8/8/8/8/6bk w - - 0 1", krrkb },
    { "krr5/8/8/8/8/8/8/6BK w - - 0 1", krrkb },
    { "KRR5/8/8/8/8/8/8/6rk w - - 0 1", krrkr },
    { "krr5/8/8/8/8/8/8/6RK w - - 0 1", krrkr },
    { "KRR5/8/8/8/8/8/8/6qk w - - 0 1", krrkq },
    { "krr5/8/8/8/8/8/8/6QK w - - 0 1", krrkq },

    { "KQP5/8/8/8/8/8/8/6pk w - - 0 1", kqpkp },
    { "kqp5/8/8/8/8/8/8/6PK w - - 0 1", kqpkp },
    { "KQP5/8/8/8/8/8/8/6nk w - - 0 1", kqpkn },
    { "kqp5/8/8/8/8/8/8/6NK w - - 0 1", kqpkn },
    { "KQP5/8/8/8/8/8/8/6bk w - - 0 1", kqpkb },
    { "kqp5/8/8/8/8/8/8/6BK w - - 0 1", kqpkb },
    { "KQP5/8/8/8/8/8/8/6rk w - - 0 1", kqpkr },
    { "kqp5/8/8/8/8/8/8/6RK w - - 0 1", kqpkr },
    { "KQP5/8/8/8/8/8/8/6qk w - - 0 1", kqpkq },
    { "kqp5/8/8/8/8/8/8/6QK w - - 0 1", kqpkq },

    { "KQP5/8/8/8/8/8/8/5pqk w - - 0 1", kqpkqp },
    { "kqp5/8/8/8/8/8/8/5PQK w - - 0 1", kqpkqp },

    { "KQPP4/8/8/8/8/8/8/6qk w - - 0 1", kqppkq },
    { "kqpp4/8/8/8/8/8/8/6QK w - - 0 1", kqppkq },

    { "KQN5/8/8/8/8/8/8/6pk w - - 0 1", kqnkp },
    { "kqn5/8/8/8/8/8/8/6PK w - - 0 1", kqnkp },
    { "KQN5/8/8/8/8/8/8/6nk w - - 0 1", kqnkn },
    { "kqn5/8/8/8/8/8/8/6NK w - - 0 1", kqnkn },
    { "KQN5/8/8/8/8/8/8/6bk w - - 0 1", kqnkb },
    { "kqn5/8/8/8/8/8/8/6BK w - - 0 1", kqnkb },
    { "KQN5/8/8/8/8/8/8/6rk w - - 0 1", kqnkr },
    { "kqn5/8/8/8/8/8/8/6RK w - - 0 1", kqnkr },
    { "KQN5/8/8/8/8/8/8/6qk w - - 0 1", kqnkq },
    { "kqn5/8/8/8/8/8/8/6QK w - - 0 1", kqnkq },

    { "KQB5/8/8/8/8/8/8/6pk w - - 0 1", kqbkp },
    { "kqb5/8/8/8/8/8/8/6PK w - - 0 1", kqbkp },
    { "KQB5/8/8/8/8/8/8/6nk w - - 0 1", kqbkn },
    { "kqb5/8/8/8/8/8/8/6NK w - - 0 1", kqbkn },
    { "KQB5/8/8/8/8/8/8/6bk w - - 0 1", kqbkb },
    { "kqb5/8/8/8/8/8/8/6BK w - - 0 1", kqbkb },
    { "KQB5/8/8/8/8/8/8/6rk w - - 0 1", kqbkr },
    { "kqb5/8/8/8/8/8/8/6RK w - - 0 1", kqbkr },
    { "KQB5/8/8/8/8/8/8/6qk w - - 0 1", kqbkq },
    { "kqb5/8/8/8/8/8/8/6QK w - - 0 1", kqbkq },

    { "KQB5/8/8/8/8/8/8/5pqk w - - 0 1", kqbkqp },
    { "kqb5/8/8/8/8/8/8/5PQK w - - 0 1", kqbkqp },

    { "KQR5/8/8/8/8/8/8/6pk w - - 0 1", kqrkp },
    { "kqr5/8/8/8/8/8/8/6PK w - - 0 1", kqrkp },
    { "KQR5/8/8/8/8/8/8/6nk w - - 0 1", kqrkn },
    { "kqr5/8/8/8/8/8/8/6NK w - - 0 1", kqrkn },
    { "KQR5/8/8/8/8/8/8/6bk w - - 0 1", kqrkb },
    { "kqr5/8/8/8/8/8/8/6BK w - - 0 1", kqrkb },
    { "KQR5/8/8/8/8/8/8/6rk w - - 0 1", kqrkr },
    { "kqr5/8/8/8/8/8/8/6RK w - - 0 1", kqrkr },
    { "KQR5/8/8/8/8/8/8/6qk w - - 0 1", kqrkq },
    { "kqr5/8/8/8/8/8/8/6QK w - - 0 1", kqrkq },

    { "KQQ5/8/8/8/8/8/8/6pk w - - 0 1", kqqkp },
    { "kqq5/8/8/8/8/8/8/6PK w - - 0 1", kqqkp },
    { "KQQ5/8/8/8/8/8/8/6nk w - - 0 1", kqqkn },
    { "kqq5/8/8/8/8/8/8/6NK w - - 0 1", kqqkn },
    { "KQQ5/8/8/8/8/8/8/6bk w - - 0 1", kqqkb },
    { "kqq5/8/8/8/8/8/8/6BK w - - 0 1", kqqkb },
    { "KQQ5/8/8/8/8/8/8/6rk w - - 0 1", kqqkr },
    { "kqq5/8/8/8/8/8/8/6RK w - - 0 1", kqqkr },
    { "KQQ5/8/8/8/8/8/8/6qk w - - 0 1", kqqkq },
    { "kqq5/8/8/8/8/8/8/6QK w - - 0 1", kqqkq },

    { "KQ6/8/8/8/8/8/8/5npk w - - 0 1", kqknp },
    { "kq6/8/8/8/8/8/8/5NPK w - - 0 1", kqknp },
    { "KQ6/8/8/8/8/8/8/5nnk w - - 0 1", kqknn },
    { "kq6/8/8/8/8/8/8/5NNK w - - 0 1", kqknn },

    { "KQ6/8/8/8/8/8/8/5bpk w - - 0 1", kqkbp },
    { "kq6/8/8/8/8/8/8/5BPK w - - 0 1", kqkbp },
    { "KQ6/8/8/8/8/8/8/5bnk w - - 0 1", kqkbn },
    { "kq6/8/8/8/8/8/8/5BNK w - - 0 1", kqkbn },
    { "KQ6/8/8/8/8/8/8/5bbk w - - 0 1", kqkbb },
    { "kq6/8/8/8/8/8/8/5BBK w - - 0 1", kqkbb },

    { "KQ6/8/8/8/8/8/8/5rpk w - - 0 1", kqkrp },
    { "kq6/8/8/8/8/8/8/5RPK w - - 0 1", kqkrp },
    { "KQ6/8/8/8/8/8/8/5rnk w - - 0 1", kqkrn },
    { "kq6/8/8/8/8/8/8/5RNK w - - 0 1", kqkrn },
    { "KQ6/8/8/8/8/8/8/5rbk w - - 0 1", kqkrb },
    { "kq6/8/8/8/8/8/8/5RBK w - - 0 1", kqkrb },

    { "KBP5/8/8/8/8/8/8/5bpk w - - 0 1", kbpkbp },
    { "kbp5/8/8/8/8/8/8/5BPK w - - 0 1", kbpkbp },
    { "KBPP4/8/8/8/8/8/8/5bpk w - - 0 1", kbppkbp },
    { "kbpp4/8/8/8/8/8/8/5BPK w - - 0 1", kbppkbp },
    { "KBPP4/8/8/8/8/8/8/4bppk w - - 0 1", kbppkbpp },
    { "kbpp4/8/8/8/8/8/8/4BPPK w - - 0 1", kbppkbpp },
    { "KBPPP3/8/8/8/8/8/8/5bpk w - - 0 1", kbpppkbp },
    { "kbppp3/8/8/8/8/8/8/5BPK w - - 0 1", kbpppkbp },
    { "KBPPP3/8/8/8/8/8/8/4bppk w - - 0 1", kbpppkbpp },
    { "kbppp3/8/8/8/8/8/8/4BPPK w - - 0 1", kbpppkbpp },

    { "KRP5/8/8/8/8/8/8/5prk w - - 0 1", krpkrp },
    { "krp5/8/8/8/8/8/8/5PRK w - - 0 1", krpkrp },
    { "KRPP4/8/8/8/8/8/8/6rk w - - 0 1", krppkr },
    { "krpp4/8/8/8/8/8/8/6RK w - - 0 1", krppkr },
    { "KRPP4/8/8/8/8/8/8/5rpk w - - 0 1", krppkrp },
    { "krpp4/8/8/8/8/8/8/5RPK w - - 0 1", krppkrp },
    { "KRPP4/8/8/8/8/8/8/4rppk w - - 0 1", krppkrpp },
    { "krpp4/8/8/8/8/8/8/4RPPK w - - 0 1", krppkrpp },
    { "KRPPP3/8/8/8/8/8/8/4rppk w - - 0 1", krpppkrpp },
    { "krppp3/8/8/8/8/8/8/4RPPK w - - 0 1", krpppkrpp },
};

void InitializeEndgame(ChessEndgame& endgame)
{
    for (auto const& [key, val] : chessEndgameMap) {
        ChessBoard board;

        board.initFromFen(key);
        endgame.add(board, val);
    }
}
