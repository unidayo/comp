#define NDEBUG

#include <cstdio>
#include <cstring>
#include <climits>
#include <cmath>

#include <iostream>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <functional>
#include <ranges>

using namespace std;

template<typename T>
struct MaxValue {
	int index = -1;
	T value;

	MaxValue() {
		value = {};
	}
	MaxValue(const T& defaultValue) : value(defaultValue) {}

	void next(int nextIndex, const T& nextValue) {
		if (value < nextValue) {
			value = nextValue;
			index = nextIndex;
		}
	}

	bool hasValue() const {
		return index >= 0;
	}
};



constexpr int MAX_N = 7;
constexpr int MAX_M = 8;
constexpr int MAX_K = 5;
constexpr int T = 1000;
constexpr int MAX_DOUBLE_UP = 20;

constexpr int SINGLE_ATTACK = 0;
constexpr int ALL_ATTACK = 1;
constexpr int SINGLE_ERASE = 2;
constexpr int ALL_ERASE = 3;
constexpr int DOUBLE_UP = 4;
constexpr int CARD_NUMBER = 5;




constexpr ranges::iota_view<int, int> range(int count) {
	return ranges::iota_view<int, int>(0, count);
}

constexpr ranges::iota_view<int, int> range(size_t count) {
	return ranges::iota_view<int, int>(0, (int)count);
}

template<typename T, size_t C>
constexpr ranges::iota_view<int, int> range(const array<T, C>& v) {
	return ranges::iota_view<int, int>(0, (int)v.size());
}

template<typename T>
constexpr ranges::iota_view<int, int> range(const vector<T>& v) {
	return ranges::iota_view<int, int>(0, (int)v.size());
}

struct Card {
	int type;
	int power;
};

struct CardPrice {
	Card card;
	int price;
	bool canNotSelect;
};

struct Mob {
	int hp;
	int money;
};

struct WorldState {
	int doubleUp;
	array<Card, MAX_N> cards;
	array<Mob, MAX_M> mobs;
};

struct World {
	int n;
	int m;
	int k;
	WorldState state;
};

struct Input {
	array<Mob, MAX_M> mobs;
	long long money;
	vector<CardPrice> cardPrices;
};

inline int ConfigIndex(int n, int m, int k) {
	int index = k - 2;
	int v;
	if (m <= 2) {
		v = 0;
	} else if (m <= 4) {
		v = 1;
	} else {
		v = 2;
	}
	index += v * 4;
	if (n >= 4) {
		index += 12;
	}
	return index;
}



struct Config {
	double newMobScore; 
	double moneyRate;
	double doubleUpMoney;
	double moneyThreshold;
	double minMoneyRate;
	double damageRate;
	double mobHpDecay;
	double mobHpScore;
	double mobHpThreshold;
	double minMobHpRate;
	double minMobScore;
	double preMobScoreRate;
	double attackBase;
	double attackEfMax;
	double attackEfRate;
	double allAttackRate;
	double singleEraseScore;
	double allEraseScore;
	double doubleUpScore;
	double doubleUpThreshold;
	double useDoubleUpScore;
	double csRate;
	double msRate;
	double useDoubleUpThreshold;
	double mobRemainThreshold;
	double mobRemainRate;
	double moneyRemainThreshold;
	double moneyRemainRate;
	double damageRemainThreshold;
	double damageRemainRate;
	double newMobRemainThreshold;
	double newMobRemainRate;
};


constexpr Config ConfigTable[] = {
	{1.785, 0.2105, 0.44, 293, 24.6, 0.86, 0.0011, 0.19, 9.4, 0.941, -1.1, 0.996, 0.922, 3.2, 0.057, 0.98, 1.55, -1.3, 127, 230, 154, 0.4375, 0.8525, 270, 191, -0.0003725, 191, 0.000455, 151, -0.00089, 171, 0.0001565},
{1.715, 0.2129, 0.57, 273, 24.6, 0.85, 0.0011, 0.19, 6.6, 0.844, -1.1, 0.994, 0.918, 3.8, 0.05, 0.96, 1.35, -1.3, 140, 190, 158.5, 0.4375, 0.8525, 225, 174, -0.0004225, 171, 0.000355, 131, -0.00097, 186, 0.000176},
{1.725, 0.2219, 0.63, 283, 24.6, 0.85, 0.0007, 0.21, 4.8, 0.747, -1.1, 0.994, 0.918, 3.2, 0.05, 1, 1.35, -1, 137, 170, 167.5, 0.4375, 0.8525, 180, 171, -0.0003225, 171, 0.000355, 131, -0.00069, 171, 0.0001565},
{1.715, 0.2243, 0.63, 283, 24.6, 0.87, 0.0007, 0.21, 4, 0.65, -1.1, 0.994, 0.918, 3.2, 0.05, 0.96, 1.35, -0.7, 142, 130, 176.5, 0.5375, 0.8525, 165, 171, -0.0004225, 171, 0.000355, 131, -0.00109, 221, 0.0001775},
{1.515, 0.2105, 0.67, 312, 24.6, 0.87, 0.0011, 0.19, 7.4, 0.961, -1.1, 0.996, 0.92, 3.8, 0.05, 0.99, 1.7, -1.3, 143, 240, 155.5, 0.45, 0.86, 300, 171, -0.0004225, 189, 0.00043, 131, -0.00109, 171, 0.0001865},
{1.485, 0.2135, 0.63, 283, 24.6, 0.87, 0.0007, 0.19, 5.6, 0.844, -1.1, 0.996, 0.918, 3.2, 0.05, 0.95, 1.35, -1.3, 140, 170, 173.5, 0.4375, 0.8525, 210, 171, -0.0003225, 171, 0.000355, 131, -0.00097, 171, 0.0001565},
{1.465, 0.2159, 0.71, 293, 24.6, 0.87, 0.0007, 0.19, 4.8, 0.777, -1.1, 0.994, 0.918, 3.8, 0.057, 0.95, 1.35, -0.7, 145, 150, 173.5, 0.4375, 0.8825, 180, 171, -0.0004225, 171, 0.000355, 131, -0.00109, 171, 0.0001565},
{1.465, 0.2189, 0.63, 301, 24.6, 0.87, 0.0007, 0.19, 4, 0.65, -1.1, 0.994, 0.918, 3.5, 0.064, 0.99, 1.35, -0.7, 150, 130, 191.5, 0.45, 0.8525, 150, 236, -0.0004225, 171, 0.000355, 134, -0.00109, 171, 0.0001565},
{1.465, 0.2099, 0.67, 337, 24.8, 0.87, 0.0007, 0.19, 6.4, 0.961, -1.1, 0.996, 0.918, 3.2, 0.064, 0.92, 1.7, -1, 153, 250, 182.5, 0.45, 0.8525, 240, 174, -0.0004075, 180, 0.000385, 131, -0.00109, 180, 0.0001565},
{1.465, 0.2129, 0.77, 313, 24.6, 0.85, 0.0007, 0.19, 5.6, 0.844, -1.1, 0.996, 0.918, 3.2, 0.05, 0.92, 1.35, -1.3, 158, 210, 191.5, 0.4375, 0.8525, 210, 191, -0.0003925, 174, 0.000455, 131, -0.00109, 191, 0.0001565},
{1.465, 0.2159, 0.65, 323, 28.6, 0.85, 0.0007, 0.17, 4.8, 0.747, -1.1, 0.994, 0.938, 3.8, 0.064, 0.92, 1.35, -1.3, 155, 150, 209.5, 0.4375, 0.8525, 195, 171, -0.0004225, 171, 0.000355, 131, -0.00109, 171, 0.0001565},
{1.465, 0.2189, 0.83, 323, 24.6, 0.87, 0.0007, 0.19, 4, 0.65, -2.8, 0.996, 0.918, 3.2, 0.05, 0.92, 1.35, -1.3, 152, 130, 200.5, 0.4375, 0.8525, 150, 171, -0.0004225, 171, 0.000355, 131, -0.00103, 171, 0.0001565},
{1.77, 0.212, 0.43, 273, 24.6, 0.87, 0.0011, 0.19, 9.4, 0.971, -1.1, 0.996, 0.92, 3.2, 0.05, 1, 1.55, 1, 135, 250, 155, 0.5625, 0.8525, 270, 189, -0.000365, 174, 0.000355, 131, -0.00063, 171, 0.000167},
{1.715, 0.2129, 0.43, 273, 24.6, 0.87, 0.0011, 0.19, 8.6, 0.844, -1.1, 0.996, 0.928, 3.5, 0.057, 1, 1.35, 0.4, 140, 170, 159.5, 0.4375, 0.8525, 240, 171, -0.000415, 171, 0.000355, 131, -0.00097, 178.5, 0.0002265},
{1.715, 0.2183, 0.53, 287, 24.6, 0.87, 0.0011, 0.19, 4.8, 0.747, -1.1, 0.994, 0.918, 3.2, 0.064, 1, 1.35, 1, 137, 170, 159.5, 0.4375, 0.8525, 210, 171, -0.0004225, 171, 0.000355, 131, -0.00069, 171, 0.0001565},
{1.715, 0.2189, 0.65, 283, 24.6, 0.87, 0.0011, 0.19, 4, 0.65, -1.1, 0.994, 0.918, 3.2, 0.057, 1, 1.35, 1, 142, 140, 177.5, 0.4375, 0.8525, 150, 186, -0.0004225, 171, 0.000355, 131, -0.00103, 171, 0.0001565},
{1.475, 0.2102, 0.53, 301, 25.6, 0.87, 0.0011, 0.19, 6.4, 0.941, -1.1, 0.994, 0.92, 3.2, 0.05, 0.99, 1.7, 1, 135, 230, 156.5, 0.5375, 0.8525, 270, 171, -0.0004225, 184.5, 0.000385, 131, -0.00109, 209, 0.0002},
{1.465, 0.2129, 0.53, 283, 24.6, 0.87, 0.0011, 0.19, 5.6, 0.844, -1.1, 0.994, 0.918, 3.8, 0.05, 0.99, 1.35, 0.4, 140, 170, 156.5, 0.4375, 0.8525, 240, 221, -0.0004075, 171, 0.000355, 131, -0.00109, 171, 0.0001565},
{1.475, 0.2159, 0.53, 293, 24.6, 0.87, 0.0007, 0.17, 4.8, 0.767, -2.8, 0.994, 0.938, 3.2, 0.064, 0.95, 1.35, 0.4, 137, 150, 183.5, 0.4375, 0.8525, 195, 171, -0.0004225, 171, 0.000355, 131, -0.00109, 236, 0.0002265},
{1.515, 0.2189, 0.73, 293, 24.6, 0.87, 0.0007, 0.19, 4, 0.65, -1.1, 0.994, 0.918, 3.2, 0.05, 0.99, 1.35, 1, 142, 130, 183.5, 0.4375, 0.8525, 150, 171, -0.0004225, 171, 0.000355, 131, -0.00109, 178.5, 0.0002265},
{1.47, 0.2099, 0.53, 317, 24.8, 0.85, 0.0011, 0.19, 6.4, 0.941, -1.1, 0.994, 0.918, 3.2, 0.05, 0.92, 1.55, 0.7, 137, 210, 165.5, 0.5375, 0.8525, 270, 174, -0.0003725, 191, 0.000455, 154, -0.00089, 191, 0.000167},
{1.465, 0.2129, 0.77, 313, 24.6, 0.85, 0.0007, 0.19, 5.6, 0.844, -1.1, 0.996, 0.918, 3.8, 0.064, 0.92, 1.35, 1, 150, 170, 174.5, 0.4375, 0.8525, 240, 171, -0.0004225, 171, 0.000355, 131, -0.00109, 186, 0.0001565},
{1.465, 0.2171, 0.83, 323, 25.4, 0.87, 0.0007, 0.19, 5.8, 0.747, -1.1, 0.996, 0.918, 3.2, 0.064, 0.96, 1.5, 0.4, 155, 150, 183.5, 0.4375, 0.8525, 180, 171, -0.0004225, 171, 0.000385, 131, -0.00109, 171, 0.0001565},
{1.475, 0.2195, 0.77, 323, 25.4, 0.85, 0.0007, 0.19, 4, 0.65, -1.1, 0.994, 0.918, 3.2, 0.05, 0.92, 1.35, 1, 142, 130, 192.5, 0.4375, 0.8525, 150, 171, -0.0004225, 171, 0.000355, 131, -0.00089, 171, 0.0001565},


};






union Select {
	int val;
	struct {
		signed char buyIndex;
		signed char useIndex;
		signed char useTarget;
	};
};

struct ScoreMoney {
	double score;
	long long money;

	ScoreMoney operator+(const ScoreMoney& right) const {
		return ScoreMoney{ score + right.score, money + right.money };
	}
};

struct State {
	int doubleUp;
	array<Card, MAX_N> cards;
	array<Mob, MAX_M> mobs;
	int useIndex;
	int turn;
	double remain;
	long long money;
	double rate; 
	double scoreRate; 
	double baseMoneyRate;
	double moneyRate; 
	double baseDamageRate;
	double damageRate; 
	double newMobScore;

	double mobRemainRate;

	double remainRate(double th, double rate) {
		if (remain < th) {
			return 1.0 + (th - remain) * rate;
		} else {
			return 1.0;
		}
	}
};

struct Operation {

	World w;
	Config config;

	function<void(int, int)> useCard;
	function<void(int)> buyCard;
	Input* input;

	State state;


	void firstPhase() {
		int configIndex = ConfigIndex(w.n, w.m, w.k);
			config = ConfigTable[configIndex];

		state.doubleUp = w.state.doubleUp;
		state.cards = w.state.cards;
		state.mobs = w.state.mobs;
		state.useIndex = -1;
		state.money = 0;

		state.turn = 0;
		state.remain = T;
		state.rate = 1;
		state.scoreRate = 1;
		state.baseMoneyRate = config.moneyRate;
		state.moneyRate = state.baseMoneyRate;
		state.baseDamageRate = config.damageRate * state.moneyRate;
		state.damageRate = state.baseDamageRate;
		state.newMobScore = config.newMobScore;

		state.mobRemainRate = 1;

	}

	double getScore(ScoreMoney sm, bool doubleUp) {
		double money = state.money + sm.money;
		if (doubleUp) {
			money *= config.doubleUpMoney;
		}
		return sm.score + moneyScore(money);
	}

	double moneyScore(double money) {
		double score = money * state.moneyRate;
		double m = money * state.scoreRate;
		if (m < config.moneyThreshold) {
			double r = 1.0 - m / config.moneyThreshold;
			r = r * r;
			r = r * r;
			score -= r * r * config.minMoneyRate;
		}
		return score;
	}

	double mobScore(Mob mob) {
		double hp = mob.hp * state.scoreRate;
		double base = mob.money * state.moneyRate * (1.0 - hp * config.mobHpDecay);
		double cost = config.mobHpScore * hp; 
		double score = base - cost;

		if (hp < config.mobHpThreshold) {
			score += state.newMobScore * config.minMobHpRate * (1.0 - hp / config.mobHpThreshold);
		}
		score *= state.mobRemainRate;
		score = max(score, config.minMobScore); 
		return score;
	}

	ScoreMoney attackScore(Mob mob, int power) {
		double preScore = mobScore(mob) * config.preMobScoreRate;
		if (mob.hp <= power) {
			return  { state.newMobScore - preScore, mob.money };
		} else {
			mob.hp -= power;
			double score = mobScore(mob) - preScore;
			score *= config.attackBase + std::min(config.attackEfMax, (((double)mob.money / mob.hp) - 1.0)) * config.attackEfRate;
			return { score, 0 };
		}
	}

	double eraseScore(Mob mob) {
		return state.newMobScore - mobScore(mob);
	}

	double cardScore(const array<int, CARD_NUMBER>& cardCounts, Card card, bool newCard) {
		switch (card.type) {
		case SINGLE_ATTACK:
		{
			return state.damageRate * card.power;
		}
		case ALL_ATTACK:
		{
			return config.allAttackRate * state.damageRate * card.power * w.m;
		}
		case SINGLE_ERASE:
		{
			int count = cardCounts[SINGLE_ERASE];
			if (newCard) {
				count++;
			}
			if (count <= 1) {
				return config.singleEraseScore;
			} else {
				return -100;
			}
		}
		case ALL_ERASE:
		{
			int count = cardCounts[ALL_ERASE];
			if (newCard) {
				count++;
			}
			if (count <= 1) {
				return config.allEraseScore;
			} else {
				return -100;
			}
		}
		default:
		{
			double score = config.doubleUpScore;
			if (state.remain <= config.doubleUpThreshold) {
				score *= state.remain / config.doubleUpThreshold;
			}
			return score;
		}
		}
	}

	double maxBuyScore(const array<int, CARD_NUMBER>& cardCounts, Select& out, ScoreMoney current, bool doubleUp) {
		MaxValue<double> maxScore(-9e9);
		for (int i = 0; i < w.k; i++) {
			const CardPrice& cp = input->cardPrices[i];
			if (cp.canNotSelect) {
				continue;
			}
			ScoreMoney sm = { cardScore(cardCounts, cp.card, true) , -cp.price };
			double score = getScore(current + sm, doubleUp);
			maxScore.next(i, score);
		}
		out.buyIndex = maxScore.index;
		return maxScore.value;
	}

	ScoreMoney useScore(Card card, int target, bool isBuy) {
		switch (card.type) {
		case SINGLE_ATTACK:
		{
			return attackScore(state.mobs[target], card.power);
		}
		case ALL_ATTACK:
		{
			ScoreMoney s = {};
			for (int t = 0; t < w.m; t++) {
				s = s + attackScore(state.mobs[t], card.power);
			}
			return s;
		}
		case SINGLE_ERASE:
		{
			return { eraseScore(state.mobs[target]), 0 };
		}
		case ALL_ERASE:
		{
			double score = 0;
			for (int t = 0; t < w.m; t++) {
				score += eraseScore(state.mobs[t]);
			}
			return { score, 0 };
		}
		default:
		{
			double cs = 0;
			array<int, CARD_NUMBER> cardCounts = {};
			for (int i = 0; i < w.n; i++) {
				Card card = state.cards[i];
				if (card.type == SINGLE_ATTACK || card.type == ALL_ATTACK) {
					cs += cardScore(cardCounts, card, false);
				}
			}
			double preMs = 0;
			double newMs = 0;
			for (int i = 0; i < w.m; i++) {
				Mob m = state.mobs[i];
				preMs += mobScore(m);
				m.hp /= 2;
				m.money /= 2;
				newMs += mobScore(m);
			}
			double score = config.useDoubleUpScore - config.csRate * cs + config.msRate * (newMs - preMs);
			if (state.remain <= config.useDoubleUpThreshold) {
				if (isBuy) {
					score *= state.remain / config.useDoubleUpThreshold;
				} else {
					score *= 2.0 - state.remain / config.useDoubleUpThreshold;
				}
			}

			return { score, 0 };
		}
		}
	}

	void action() {
		array<int, CARD_NUMBER> cardCounts = {};
		for (int i = 0; i < w.n; i++) {
			if (i != state.useIndex) {
				int type = state.cards[i].type;
				cardCounts[type] += 1;
			}
		}


		MaxValue<double> maxScore(-9e9);

		for (int i : range(w.n)) {
			if (i == state.useIndex) {
				continue;
			}
			Card card = state.cards[i];
			cardCounts[card.type]--; 
			double baseScore = -cardScore(cardCounts, card, false); 
			Select select = {};
			select.buyIndex = -1;
			select.useIndex = i;
			if (card.type == SINGLE_ATTACK || card.type == SINGLE_ERASE) {
				for (int j : range(w.m)) {
					ScoreMoney sm = useScore(card, j, false);
					sm.score += baseScore;
					double score = maxBuyScore(cardCounts, select, sm, false);
					select.useTarget = j;
					maxScore.next(select.val, score);
				}
			} else {
				ScoreMoney sm = useScore(card, 0, false);
				sm.score += baseScore;
				double score = maxBuyScore(cardCounts, select, sm, card.type == DOUBLE_UP);
				maxScore.next(select.val, score);
			}
			cardCounts[card.type]++; 
		}
		for (int i : range(w.k)) {
			CardPrice cp = input->cardPrices[i];
			if (cp.canNotSelect) {
				continue;
			}
			Select select = {};
			select.buyIndex = i;
			select.useIndex = state.useIndex;
			if (cp.card.type == SINGLE_ATTACK || cp.card.type == SINGLE_ERASE) {
				for (int j : range(w.m)) {
					ScoreMoney sm = useScore(cp.card, j, true);
					sm.money -= cp.price;
					double score = getScore(sm, false);
					select.useTarget = j;
					maxScore.next(select.val, score);
				}
			} else {
				ScoreMoney sm = useScore(cp.card, 0, true);
				sm.money -= cp.price;
				double score = getScore(sm, cp.card.type == DOUBLE_UP);
				maxScore.next(select.val, score);
			}
		}


		Select selected;
		selected.val = maxScore.index;
		const CardPrice& cp = input->cardPrices[selected.buyIndex];
		state.cards[state.useIndex] = cp.card;

		cardCounts[cp.card.type] += 1;

		if (cardCounts[DOUBLE_UP] >= 1) {
			if (state.doubleUp + cardCounts[DOUBLE_UP] >= MAX_DOUBLE_UP) {
				for (int i : range(w.n)) {
					Card card = state.cards[i];
					if (card.type == DOUBLE_UP) {
						selected.useIndex = i;
						selected.useTarget = 0;
						break;
					}
				}
			}
		}
		cardCounts[state.cards[selected.useIndex].type] -= 1;

		buyCard(selected.buyIndex);
		useProc(selected.useIndex, selected.useTarget);

		for (CardPrice& p : input->cardPrices) {
			if (state.money < p.price) {
				p.canNotSelect = true;
				continue;
			}
			if (p.card.type == DOUBLE_UP) {
				if (state.doubleUp + cardCounts[DOUBLE_UP] >= MAX_DOUBLE_UP) {
					p.canNotSelect = true;
					continue;
				}
			}
			p.canNotSelect = false;
		}
	}

	void useProc(int useIndex, int useTarget) {
		Card use = state.cards[useIndex];
		if (use.type == DOUBLE_UP) {
			state.doubleUp++;
			state.rate *= 2;
			state.scoreRate *= 0.5;
			state.baseMoneyRate *= 0.5;
			state.baseDamageRate *= 0.5;
		}
		state.useIndex = useIndex;
		useCard(useIndex, useTarget);

		state.mobs = input->mobs;
		state.money = input->money;
	}

	void secondPhase() {
		{
			useProc(0, 0);

			for (CardPrice& p : input->cardPrices) {
				if (state.money < p.price) {
					p.canNotSelect = true;
					continue;
				}
				p.canNotSelect = false;
			}
		}

		for (int i = 0; i < T - 1; i++) {
			state.turn = i + 1;
			state.remain = T - state.turn;
			state.moneyRate = state.baseMoneyRate * state.remainRate(config.moneyRemainThreshold, config.moneyRemainRate);
			state.damageRate = state.baseDamageRate * state.remainRate(config.damageRemainThreshold, config.damageRemainRate);
			state.newMobScore = config.newMobScore * state.remainRate(config.newMobRemainThreshold, config.newMobRemainRate);
			state.mobRemainRate = state.remainRate(config.mobRemainThreshold, config.mobRemainRate);
			action();
		}

		buyCard(0);
	}


};
int main() {
	Operation* operation = new Operation();
	World& w = operation->w;

	int t;
	scanf("%d %d %d %d", &w.n, &w.m, &w.k, &t);

	Input input = {};
	input.cardPrices.resize(w.k);
	operation->input = &input;

	for (int i = 0; i < w.n; i++) {
		Card& c = w.state.cards[i];
		scanf("%d %d", &c.type, &c.power);
	}

	for (int i = 0; i < w.m; i++) {
		Mob& m = w.state.mobs[i];
		scanf("%d %d", &m.hp, &m.money);
	}

	operation->useCard = [&](int index, int target) {
		printf("%d %d\n", index, target);
		fflush(stdout);
		for (int i = 0; i < w.m; i++) {
			Mob& m = input.mobs[i];
			scanf("%d %d", &m.hp, &m.money);
		}
		cin >> input.money;
		for (int i = 0; i < w.k; i++) {
			CardPrice& cp = input.cardPrices[i];
			scanf("%d %d %d", &cp.card.type, &cp.card.power, &cp.price);
		}
	};

	operation->buyCard = [](int index) {
		printf("%d\n", index);
		fflush(stdout);
	};

	operation->firstPhase();
	operation->secondPhase();

	return 0;
}
